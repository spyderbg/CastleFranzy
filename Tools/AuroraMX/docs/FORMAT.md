# Castle Frenzy sprite format

The authoritative inputs are the original `.sprite` documents, `Data/Sprites/GP_export.fft`, `Tools/AuroraGT/bsprite_format.txt`, and the game's `ASprite::Load` implementation. AuroraMX implements the GP_Data export independently; it never reads an existing `.bsprite` to generate a new one.

The textual format stores image records, ordered modules, ordered frames containing collision rectangles and module/hyperframe placements, and ordered animations containing timed frame placements. Source IDs remain stable when rows move; export resolves IDs to the current zero-based row indices. Module image references are image **indices**, not image IDs. Legacy files include nonzero image IDs even when all modules reference image index zero.

The binary is little endian:

| Section | Fields |
| --- | --- |
| Header | `u16 0x05DF`, `u32 0x0001853B` |
| Modules | `u16 count`, then `u8 type` and `u16 x,y,width,height` per image module |
| Frame modules | `u16 count`, then `u16 module/frame index`, `i16 x,y`, `u8 flags` |
| Collision rectangles | `u16 count`, then `i16 x,y,width,height` per rectangle |
| Frames | `u16 count`, then `u16 number of placements`, `i16 first placement`, `u8 rectangle count` |
| Frame bounds | `i16 x,y,width,height` for each frame |
| Animation steps | `u16 count`, then `u8 frame index`, `u8 ticks`, `i8 x,y`, `u8 flags` |
| Animations | `u16 count`, then `u16 number of steps`, `i16 first step` |

Flags are horizontal flip `1`, vertical flip `2`, 90° rotation `4`, and hyperframe `16`. Animation frame indices use flag bits 6–7 for the upper two bits, matching the game loader. Rectangles in `.sprite` use x1/y1/x2/y2; binary rectangles use x/y/width/height. Bounding boxes are computed from the frame's placements, independently of collision rectangles. The compiler checks field widths instead of silently wrapping values.

`GP_export.fft` includes a final `sprite.module_images` directive, but its flags do not enable embedded images and the game only consumes the metadata above. Some historical exports contain additional indexed image data after this metadata. AuroraMX exports the external-atlas form. Its conformance test locates the metadata boundary by independently walking the legacy file's counts, compares every metadata byte, and separately reports full-file matches.

Header exports preserve AuroraGT's naming behavior: named rows use their descriptions, unnamed rows use their row index, prefixes and TOUPPER are applied, and duplicate macro names are commented instead of redefining the first one. Tests compare the generated macro/value maps against all three committed headers.

Text saves preserve untouched source bytes. Structured edits serialize all known records and keep unknown top-level blocks intact; explicit source edits are validated and saved verbatim. PNG/TGA/BMP pixels remain separate files and are not embedded by this profile.
