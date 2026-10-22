import {
  Matrix4,
  Vector3,
  MathUtils,
  AnimationClip,
  NumberKeyframeTrack,
  Mesh,
  type Scene,
  type BufferGeometry,
  type Material,
  type Texture,
  type KeyframeTrack,
} from "three";

/** Bake the game's ColladaMax translate/axis-angle/scale channels to matrices.
 * Three's ColladaLoader only animates matrices. Source assets remain untouched.
 * Empty-target engine event channels and material UV channels are not transforms.
 */
export function prepareCollada(source: string): string {
  const doc = new DOMParser().parseFromString(source, "application/xml");
  if (doc.querySelector("parsererror")) throw new Error("Invalid Collada XML");
  // Castle includes unused planes with literal NaN transforms. They cannot be
  // rendered and must not poison bounds for the rest of the scene.
  for (const node of doc.querySelectorAll("node")) {
    const transforms = [...node.children].filter((e) =>
      ["matrix", "translate", "rotate", "scale"].includes(e.localName),
    );
    if (
      transforms.some((e) =>
        (e.textContent || "")
          .trim()
          .split(/\s+/)
          .some((v) => !Number.isFinite(Number(v))),
      )
    )
      node.remove();
  }
  const elements = new Map(
    [...doc.querySelectorAll("[id]")].map((e) => [e.id, e]),
  );
  // Irrlicht vertex <extra> metadata is not an input; Three treats every
  // vertices child as an input and would attempt to read its missing source.
  for (const vertices of doc.querySelectorAll("vertices"))
    for (const child of [...vertices.children])
      if (child.localName !== "input") child.remove();
  // Images are decoded to PNG by the local API before the browser sees them.
  for (const image of doc.querySelectorAll("library_images image init_from"))
    image.textContent = (image.textContent || "").replace(
      /\.(tga|bmp|pvr)$/i,
      ".png",
    );
  // Spline helpers (e.g. Hydra's path) have no renderable mesh geometry.
  for (const instance of doc.querySelectorAll("instance_geometry")) {
    const geometry = elements.get(
      (instance.getAttribute("url") || "").replace(/^#/, ""),
    );
    if (geometry && !geometry.querySelector("mesh")) instance.remove();
  }
  const targetGeometryIds = new Set<string>();
  for (const controller of doc.querySelectorAll("controller")) {
    const morph = controller.querySelector("morph");
    if (!morph) continue;
    morph
      .querySelector("IDREF_array")
      ?.textContent?.trim()
      .split(/\s+/)
      .forEach((id) => targetGeometryIds.add(id));
    for (const instance of doc.querySelectorAll("instance_controller"))
      if (instance.getAttribute("url") === `#${controller.id}`) {
        const geometry = doc.createElementNS(
          doc.documentElement.namespaceURI,
          "instance_geometry",
        );
        geometry.setAttribute("url", morph.getAttribute("source") || "");
        for (const child of [...instance.children])
          if (child.localName === "bind_material")
            geometry.appendChild(child.cloneNode(true));
        instance.replaceWith(geometry);
      }
    controller.remove();
  }
  // Build target buffers through the loader too, then remove these helper nodes.
  const visual = doc.querySelector("library_visual_scenes visual_scene");
  for (const id of targetGeometryIds) {
    const node = doc.createElementNS(doc.documentElement.namespaceURI, "node");
    node.setAttribute("id", `editor_target_${id}`);
    node.setAttribute("name", "__editor_morph_target");
    const instance = doc.createElementNS(
      doc.documentElement.namespaceURI,
      "instance_geometry",
    );
    instance.setAttribute("url", `#${id}`);
    node.appendChild(instance);
    visual?.appendChild(node);
  }
  const floats = (e?: Element | null) =>
    e?.textContent?.trim().split(/\s+/).map(Number) || [];
  const lookup = (reference: string | null) =>
    elements.get((reference || "").replace(/^#/, ""));
  interface Channel {
    sid: string;
    member: number;
    times: number[];
    values: number[];
    stride: number;
    interpolation: string[];
  }
  const groups = new Map<Element, Channel[]>();
  for (const channel of doc.querySelectorAll("library_animations channel")) {
    const target = channel.getAttribute("target") || "",
      slash = target.indexOf("/");
    if (slash < 0) continue;
    const node = elements.get(target.slice(0, slash));
    if (node?.localName !== "node") continue;
    const address = target.slice(slash + 1),
      sid = address.split(/[.(]/)[0];
    const transform = [...node.children].find(
      (e) => e.getAttribute("sid") === sid,
    );
    if (
      !transform ||
      !["matrix", "translate", "rotate", "scale"].includes(transform.localName)
    )
      continue;
    const sampler = lookup(channel.getAttribute("source"));
    const input = (semantic: string) =>
      lookup(
        sampler
          ?.querySelector(`input[semantic="${semantic}"]`)
          ?.getAttribute("source") || "",
      );
    const data = input("OUTPUT"),
      times = floats(input("INPUT")?.querySelector("float_array")),
      values = floats(data?.querySelector("float_array"));
    if (!times.length || !values.length) continue;
    const memberName = address.split(".")[1];
    const member = memberName
      ? ["X", "Y", "Z", "ANGLE"].indexOf(memberName)
      : -1;
    const item: Channel = {
      sid,
      member,
      times,
      values,
      stride: Number(
        data?.querySelector("accessor")?.getAttribute("stride") || 1,
      ),
      interpolation:
        input("INTERPOLATION")
          ?.querySelector("Name_array")
          ?.textContent?.trim()
          .split(/\s+/) || [],
    };
    groups.set(node, [...(groups.get(node) || []), item]);
  }
  const sample = (channel: Channel, time: number) => {
    let lo = 0,
      hi = channel.times.length - 1;
    while (lo < hi) {
      const mid = Math.ceil((lo + hi) / 2);
      if (channel.times[mid] <= time) lo = mid;
      else hi = mid - 1;
    }
    const next = Math.min(lo + 1, channel.times.length - 1),
      span = channel.times[next] - channel.times[lo];
    const mix =
      channel.interpolation[lo] === "STEP" || !span
        ? 0
        : Math.max(0, Math.min(1, (time - channel.times[lo]) / span));
    return Array.from(
      { length: channel.stride },
      (_, i) =>
        channel.values[lo * channel.stride + i] * (1 - mix) +
        channel.values[next * channel.stride + i] * mix,
    );
  };
  const animations: string[] = [];
  let index = 0;
  for (const [node, channels] of groups) {
    const transforms = [...node.children].filter((e) =>
      ["matrix", "translate", "rotate", "scale"].includes(e.localName),
    );
    const times = [...new Set(channels.flatMap((c) => c.times))].sort(
        (a, b) => a - b,
      ),
      output: number[] = [];
    const total = new Matrix4(),
      part = new Matrix4(),
      axis = new Vector3();
    for (const time of times) {
      total.identity();
      for (const transform of transforms) {
        let values = floats(transform);
        for (const channel of channels)
          if (channel.sid === transform.getAttribute("sid")) {
            const sampled = sample(channel, time);
            if (channel.member < 0) values = sampled;
            else values[channel.member] = sampled[0];
          }
        switch (transform.localName) {
          case "matrix":
            part.fromArray(values).transpose();
            break;
          case "translate":
            part.makeTranslation(values[0], values[1], values[2]);
            break;
          case "rotate":
            axis.set(values[0], values[1], values[2]).normalize();
            part.makeRotationAxis(axis, MathUtils.degToRad(values[3]));
            break;
          case "scale":
            part.makeScale(values[0], values[1], values[2]);
            break;
        }
        total.multiply(part);
      }
      output.push(...total.clone().transpose().elements);
    }
    const matrix = doc.createElementNS(
      doc.documentElement.namespaceURI,
      "matrix",
    );
    matrix.setAttribute("sid", "editor_matrix");
    // Rest pose must retain the original transform, including unanimated parts.
    const rest = new Matrix4();
    for (const transform of transforms) {
      const v = floats(transform);
      switch (transform.localName) {
        case "matrix":
          part.fromArray(v).transpose();
          break;
        case "translate":
          part.makeTranslation(v[0], v[1], v[2]);
          break;
        case "rotate":
          part.makeRotationAxis(
            axis.set(v[0], v[1], v[2]).normalize(),
            MathUtils.degToRad(v[3]),
          );
          break;
        case "scale":
          part.makeScale(v[0], v[1], v[2]);
          break;
      }
      rest.multiply(part);
    }
    matrix.textContent = rest.transpose().elements.join(" ");
    node.insertBefore(matrix, transforms[0]);
    transforms.forEach((e) => e.remove());
    const id = `editor_animation_${index++}`,
      nodeId = node.getAttribute("id");
    animations.push(
      `<animation id="${id}"><source id="${id}_input"><float_array id="${id}_times" count="${times.length}">${times.join(" ")}</float_array><technique_common><accessor source="#${id}_times" count="${times.length}" stride="1"><param name="TIME" type="float"/></accessor></technique_common></source><source id="${id}_output"><float_array id="${id}_matrices" count="${output.length}">${output.join(" ")}</float_array><technique_common><accessor source="#${id}_matrices" count="${times.length}" stride="16"><param name="TRANSFORM" type="float4x4"/></accessor></technique_common></source><sampler id="${id}_sampler"><input semantic="INPUT" source="#${id}_input"/><input semantic="OUTPUT" source="#${id}_output"/></sampler><channel source="#${id}_sampler" target="${nodeId}/editor_matrix"/></animation>`,
    );
  }
  doc
    .querySelectorAll("library_animations,library_animation_clips")
    .forEach((e) => e.remove());
  return new XMLSerializer()
    .serializeToString(doc)
    .replace(
      "</COLLADA>",
      `<library_animations>${animations.join("")}</library_animations></COLLADA>`,
    );
}

/** Add native gate morph weights and material UV curves after loading geometry. */
export function finishCollada(
  scene: Scene,
  libraryObject: object,
  source: string,
): (time: number, step: boolean) => void {
  const library = libraryObject as {
    geometries: Record<
      string,
      { build?: Record<string, { data: BufferGeometry }> }
    >;
    materials: Record<
      string,
      { url: string; build?: Material & { map?: Texture; alphaMap?: Texture } }
    >;
  };
  const doc = new DOMParser().parseFromString(source, "application/xml");
  const elements = new Map(
    [...doc.querySelectorAll("[id]")].map((e) => [e.id, e]),
  );
  const ref = (r: string | null | undefined) =>
    elements.get((r || "").replace(/^#/, ""));
  const floats = (e?: Element | null) =>
    e?.textContent?.trim().split(/\s+/).map(Number) || [];
  const curves = [...doc.querySelectorAll("library_animations channel")].map(
    (channel) => {
      const sampler = ref(channel.getAttribute("source"));
      const input = (semantic: string) =>
        ref(
          sampler
            ?.querySelector(`input[semantic="${semantic}"]`)
            ?.getAttribute("source"),
        );
      return {
        target: channel.getAttribute("target") || "",
        times: floats(input("INPUT")?.querySelector("float_array")),
        values: floats(input("OUTPUT")?.querySelector("float_array")),
      };
    },
  );
  const tracks: KeyframeTrack[] = [...(scene.animations[0]?.tracks || [])];
  for (const morph of doc.querySelectorAll("controller morph")) {
    const base =
      library.geometries[(morph.getAttribute("source") || "").slice(1)]?.build;
    const targetSource = ref(
      morph
        .querySelector('targets input[semantic="MORPH_TARGET"]')
        ?.getAttribute("source"),
    );
    const targets =
      targetSource
        ?.querySelector("IDREF_array")
        ?.textContent?.trim()
        .split(/\s+/) || [];
    const weightId = (
      morph
        .querySelector('targets input[semantic="MORPH_WEIGHT"]')
        ?.getAttribute("source") || ""
    ).slice(1);
    for (const [kind, geometry] of Object.entries(base || {})) {
      const attributes = targets.map(
        (id) => library.geometries[id]?.build?.[kind]?.data.attributes.position,
      );
      if (
        attributes.some(
          (a) => !a || a.count !== geometry.data.attributes.position.count,
        )
      )
        throw new Error("Incompatible Collada morph target topology");
      geometry.data.morphAttributes.position = attributes.map((a) =>
        a!.clone(),
      );
      scene.traverse((node) => {
        if (!(node instanceof Mesh) || node.geometry !== geometry.data) return;
        node.updateMorphTargets();
        targets.forEach((_, i) => {
          const curve = curves.find((c) => c.target === `${weightId}(${i})`);
          if (curve?.times.length)
            tracks.push(
              new NumberKeyframeTrack(
                `${node.uuid}.morphTargetInfluences[${i}]`,
                curve.times,
                curve.values,
              ),
            );
        });
      });
    }
  }
  const helpers: Mesh[] = [];
  scene.traverse((n) => {
    if (n.name === "__editor_morph_target") helpers.push(n as Mesh);
  });
  helpers.forEach((n) => n.removeFromParent());
  if (tracks.length)
    scene.animations = [new AnimationClip("default", -1, tracks)];
  const uv = curves.filter(
    (c) =>
      /\/(offsetU2?|offsetV2?|repeatU|repeatV|rotateUV)$/.test(c.target) &&
      c.times.length &&
      c.values.length === c.times.length,
  );
  return (time, step) => {
    for (const curve of uv) {
      const [effect, , property] = curve.target.split("/");
      let index = 0;
      while (index + 1 < curve.times.length && curve.times[index + 1] <= time)
        index++;
      const next = Math.min(index + 1, curve.times.length - 1),
        span = curve.times[next] - curve.times[index];
      const mix =
        step || !span
          ? 0
          : Math.max(0, Math.min(1, (time - curve.times[index]) / span));
      const value = curve.values[index] * (1 - mix) + curve.values[next] * mix;
      for (const material of Object.values(library.materials))
        if (material.url === effect && material.build) {
          const map = property.endsWith("2")
            ? material.build.alphaMap
            : material.build.map;
          if (!map) continue;
          if (property.startsWith("offsetU")) map.offset.x = value;
          else if (property.startsWith("offsetV")) map.offset.y = value;
          else if (property === "repeatU") map.repeat.x = value;
          else if (property === "repeatV") map.repeat.y = value;
          else if (property === "rotateUV")
            map.rotation = MathUtils.degToRad(value);
        }
    }
  };
}

export function sliceAnimation(
  source: AnimationClip,
  name: string,
  start: number,
  end: number,
  step: boolean,
): AnimationClip {
  const duration = Math.max(0, end - start);
  const tracks = source.tracks.map((track) => {
    const copy = track.clone();
    const interpolant = step
      ? track.InterpolantFactoryMethodDiscrete()
      : track.InterpolantFactoryMethodLinear();
    const times = [
      start,
      ...Array.from(track.times).filter((t) => t > start && t < end),
      ...(end > start ? [end] : []),
    ];
    copy.times = new Float32Array(times.map((t) => t - start));
    copy.values = new Float32Array(
      times.flatMap((t) => Array.from(interpolant.evaluate(t))),
    );
    return copy;
  });
  return new AnimationClip(name, duration || 0.001, tracks);
}
