import { afterEach, vi } from "vitest";

function createCanvasContextStub(canvas) {
  const noop = () => {};
  const gradient = {
    addColorStop: noop,
  };

  return new Proxy(
    {
      canvas,
      globalAlpha: 1,
      lineWidth: 1,
      measureText: (text = "") => ({
        width: String(text).length * 8,
      }),
      createLinearGradient: () => gradient,
      createRadialGradient: () => gradient,
      createPattern: () => ({}),
      getImageData: () => ({
        data: new Uint8ClampedArray(4),
      }),
      putImageData: noop,
      setLineDash: noop,
      getLineDash: () => [],
      save: noop,
      restore: noop,
      beginPath: noop,
      closePath: noop,
      moveTo: noop,
      lineTo: noop,
      arc: noop,
      fill: noop,
      stroke: noop,
      clearRect: noop,
      fillRect: noop,
      strokeRect: noop,
      translate: noop,
      scale: noop,
      rotate: noop,
      setTransform: noop,
      resetTransform: noop,
      drawImage: noop,
      fillText: noop,
      strokeText: noop,
    },
    {
      get(target, property) {
        if (property in target) {
          return target[property];
        }

        return noop;
      },
      set(target, property, value) {
        target[property] = value;
        return true;
      },
    },
  );
}

if (typeof window !== "undefined" && typeof HTMLCanvasElement !== "undefined") {
  Object.defineProperty(HTMLCanvasElement.prototype, "getContext", {
    configurable: true,
    writable: true,
    value: function getContext() {
      return createCanvasContextStub(this);
    },
  });
}

afterEach(() => {
  window.sessionStorage.clear();
  vi.clearAllMocks();
  vi.restoreAllMocks();
  document.body.innerHTML = "";
});
