declare module "postject" {
  export function inject(
    filename: string,
    resourceName: string,
    data: Buffer,
    options: { sentinelFuse: string },
  ): Promise<void>;
}
