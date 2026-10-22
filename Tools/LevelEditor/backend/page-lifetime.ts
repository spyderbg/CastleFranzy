import type { ServerResponse } from "node:http";

/** Each open page holds a stream; a refresh can reconnect during the grace period. */
export class PageLifetime {
  private readonly connections = new Set<ServerResponse>();
  private timer?: ReturnType<typeof setTimeout>;
  private disposed = false;

  constructor(
    private readonly onLastPageClose: () => void,
    private readonly busy: () => boolean = () => false,
    private readonly graceMs = 2000,
  ) {}

  connect(response: ServerResponse): void {
    if (this.disposed) {
      response.end();
      return;
    }
    clearTimeout(this.timer);
    this.connections.add(response);
    response.writeHead(200, {
      "Content-Type": "text/event-stream",
      "Cache-Control": "no-store",
      Connection: "keep-alive",
      "X-Content-Type-Options": "nosniff",
    });
    response.write("event: connected\ndata: {}\n\n");
    const keepalive = setInterval(
      () => response.write(": keepalive\n\n"),
      15000,
    );
    keepalive.unref();
    response.once("close", () => {
      clearInterval(keepalive);
      this.connections.delete(response);
      if (!this.disposed && this.connections.size === 0) this.scheduleClose();
    });
  }

  private scheduleClose(): void {
    clearTimeout(this.timer);
    this.timer = setTimeout(() => {
      if (this.disposed || this.connections.size) return;
      // Closing the browser must not interrupt a save or game command.
      if (this.busy()) this.scheduleClose();
      else this.onLastPageClose();
    }, this.graceMs);
  }

  dispose(): void {
    this.disposed = true;
    clearTimeout(this.timer);
    for (const response of this.connections) response.end();
    this.connections.clear();
  }
}
