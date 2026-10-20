import { PNG } from "pngjs";

function decodeBmp(data: Buffer): PNG {
  if (data.length < 54) throw new Error("Truncated BMP header");
  const offset = data.readUInt32LE(10),
    header = data.readUInt32LE(14),
    width = data.readInt32LE(18),
    signedHeight = data.readInt32LE(22),
    height = Math.abs(signedHeight),
    bits = data.readUInt16LE(28),
    compression = data.readUInt32LE(30);
  if (
    header < 40 ||
    14 + header > data.length ||
    offset < 14 + header ||
    width <= 0 ||
    height === 0 ||
    width * height > 32_000_000 ||
    ![1, 4, 8, 16, 24, 32].includes(bits) ||
    ![0, 3].includes(compression) ||
    (compression === 3 && bits < 16)
  )
    throw new Error(
      "BMP preview supports uncompressed indexed/RGB images and bitfield RGB images",
    );
  const stride = Math.ceil((width * bits) / 32) * 4;
  if (offset + stride * height > data.length)
    throw new Error("Truncated BMP pixels");
  let red = bits === 16 ? 0x7c00 : 0xff0000,
    green = bits === 16 ? 0x3e0 : 0xff00,
    blue = bits === 16 ? 0x1f : 0xff,
    alpha = 0;
  if (compression === 3) {
    if (data.length < 66) throw new Error("Truncated BMP color masks");
    red = data.readUInt32LE(54);
    green = data.readUInt32LE(58);
    blue = data.readUInt32LE(62);
    if (header >= 56) alpha = data.readUInt32LE(66);
  } else if (bits === 32) {
    for (let y = 0; y < height && !alpha; y++)
      for (let x = 0; x < width; x++)
        if (data[offset + y * stride + x * 4 + 3]) {
          alpha = 0xff000000;
          break;
        }
  }
  const channel = (value: number, mask: number) => {
    if (!mask) return 0;
    let shift = 0;
    while (((mask >>> shift) & 1) === 0 && shift < 31) shift++;
    return Math.round((((value & mask) >>> shift) * 255) / (mask >>> shift));
  };
  const image = new PNG({ width, height });
  for (let y = 0; y < height; y++)
    for (let x = 0; x < width; x++) {
      const start = offset + (signedHeight < 0 ? y : height - y - 1) * stride,
        at = (y * width + x) * 4;
      let r: number,
        g: number,
        b: number,
        a = 255;
      if (bits <= 8) {
        const index =
          bits === 8
            ? data[start + x]
            : bits === 4
              ? (data[start + (x >> 1)] >> ((1 - (x & 1)) * 4)) & 15
              : (data[start + (x >> 3)] >> (7 - (x & 7))) & 1;
        const palette = 14 + header + index * 4;
        if (palette + 3 > offset) throw new Error("Invalid BMP palette index");
        b = data[palette];
        g = data[palette + 1];
        r = data[palette + 2];
      } else if (bits === 24) {
        const i = start + x * 3;
        b = data[i];
        g = data[i + 1];
        r = data[i + 2];
      } else {
        const value =
          bits === 16
            ? data.readUInt16LE(start + x * 2)
            : data.readUInt32LE(start + x * 4);
        r = channel(value, red);
        g = channel(value, green);
        b = channel(value, blue);
        if (alpha) a = channel(value, alpha);
      }
      image.data[at] = r;
      image.data[at + 1] = g;
      image.data[at + 2] = b;
      image.data[at + 3] = a;
    }
  return image;
}

export function decodeImage(data: Buffer): PNG {
  if (data[0] === 66 && data[1] === 77) return decodeBmp(data);
  if (
    data.subarray(0, 8).equals(Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]))
  ) {
    if (
      data.length < 24 ||
      data.readUInt32BE(16) * data.readUInt32BE(20) > 32_000_000
    )
      throw new Error("Image previews are limited to 32 megapixels");
    return PNG.sync.read(data);
  }
  if (data.length < 18) throw new Error("Image is truncated");
  const id = data[0],
    mapped = data[1],
    type = data[2],
    mapFirst = data.readUInt16LE(3),
    mapLength = data.readUInt16LE(5),
    mapBits = data[7];
  const width = data.readUInt16LE(12),
    height = data.readUInt16LE(14),
    bits = data[16],
    descriptor = data[17];
  if (
    ![1, 2, 3, 9, 10, 11].includes(type) ||
    !width ||
    !height ||
    ![8, 16, 24, 32].includes(bits) ||
    width * height > 32_000_000
  )
    throw new Error(
      "Preview supports PNG, BMP, and TGA (indexed, grayscale, RGB/RGBA, including TGA RLE)",
    );
  let offset = 18 + id;
  const read = (bytes: number) => {
    if (offset + bytes > data.length) throw new Error("Truncated TGA pixels");
    const b = data.subarray(offset, offset + bytes);
    offset += bytes;
    return b;
  };
  const color = (b: Buffer, depth: number): number[] => {
    if (depth === 8) return [b[0], b[0], b[0], 255];
    if (depth === 16 || depth === 15) {
      const n = b.readUInt16LE();
      return [
        (((n >> 10) & 31) * 255) / 31,
        (((n >> 5) & 31) * 255) / 31,
        ((n & 31) * 255) / 31,
        depth === 15 || n & 32768 ? 255 : 0,
      ];
    }
    return [b[2], b[1], b[0], depth === 32 ? b[3] : 255];
  };
  const palette: number[][] = [];
  if (mapped)
    for (let i = 0; i < mapLength; i++)
      palette[mapFirst + i] = color(read(Math.ceil(mapBits / 8)), mapBits);
  const image = new PNG({ width, height });
  let index = 0;
  const pixel = () => {
    const bytes = read(Math.ceil(bits / 8));
    if (type === 1 || type === 9) {
      const n = bits === 8 ? bytes[0] : bytes.readUInt16LE();
      if (!palette[n]) throw new Error("Invalid TGA palette index");
      return palette[n];
    }
    if (type === 3 || type === 11)
      return [bytes[0], bytes[0], bytes[0], bits === 16 ? bytes[1] : 255];
    const p = color(bytes, bits);
    if (!(descriptor & 15) && bits === 32) p[3] = 255;
    return p;
  };
  const put = (p: number[]) => {
    const x = index % width,
      y = Math.floor(index / width);
    const target =
      ((descriptor & 32 ? y : height - y - 1) * width +
        (descriptor & 16 ? width - x - 1 : x)) *
      4;
    for (let c = 0; c < 4; c++) image.data[target + c] = p[c];
    index++;
  };
  while (index < width * height) {
    if (type < 9) put(pixel());
    else {
      const header = read(1)[0],
        count = (header & 127) + 1;
      if (index + count > width * height)
        throw new Error("Invalid TGA RLE packet");
      if (header & 128) {
        const p = pixel();
        for (let i = 0; i < count; i++) put(p);
      } else for (let i = 0; i < count; i++) put(pixel());
    }
  }
  return image;
}
export function previewImage(
  data: Buffer,
  transparent?: number,
  alpha?: Buffer,
): Buffer {
  const image = decodeImage(data),
    mask = alpha && decodeImage(alpha);
  if (mask && (mask.width !== image.width || mask.height !== image.height))
    throw new Error("Alpha image dimensions do not match");
  for (let i = 0; i < image.data.length; i += 4) {
    if (mask) image.data[i + 3] = mask.data[i];
    if (
      transparent !== undefined &&
      ((image.data[i] << 16) | (image.data[i + 1] << 8) | image.data[i + 2]) ===
        (transparent & 0xffffff)
    )
      image.data[i + 3] = 0;
  }
  return PNG.sync.write(image);
}
