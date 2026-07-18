(function () {
  // Live LED preview of the shelf clock.
  // Polls /getleds (2 hex chars brightness + 6 hex chars per LED) and renders
  // the physical layout: 7 digit columns x 2 cubby rows. Real digits sit at
  // columns 0/2/4/6 (= digit 6/4/2/0), the shared "fake" digits 5/3/1 between
  // them only own the three horizontal segments of their column - their
  // verticals are the neighbouring real digits' strips.
  //
  // Wiring (from the wiring plan): segment n occupies LEDs n*L .. n*L+L-1.
  // LED direction per segment role: top = left->right, middle/bottom =
  // right->left, upper-left/lower-left = bottom->top, upper-right/lower-right
  // = top->bottom. The 14 spotlights follow after the segment LEDs, zig-zag
  // from the LEFT: col6 upper,lower / col5 lower,upper / col4 upper,lower ...
  const NUM_SEGMENTS = 37;
  const NUM_SPOTS = 14;

  const PAD = 14;      // canvas padding
  const W = 100;       // cubby width
  const H = 90;        // cubby height
  const THICKNESS = 8; // segment line width
  const POLL_MS = 500;

  let canvas = null;
  let ctx = null;
  let segGeo = null;
  let fetching = false;

  // Per-segment line geometry {x0,y0,x1,y1}, endpoints ordered in LED
  // direction so per-LED gradients flow the same way as on the real clock
  function buildGeometry() {
    const segs = new Array(NUM_SEGMENTS);
    const set = (n, x0, y0, x1, y1) => { segs[n] = { x0, y0, x1, y1 }; };
    for (let c = 0; c < 7; c++) {
      const x = PAD + c * W;
      const yTop = PAD;
      const yMid = PAD + H;
      const yBot = PAD + 2 * H;
      if (c % 2 === 0) {
        // Real digit column. Segment roles within a digit (base = digit*5):
        // +0 lower-right, +1 bottom, +2 lower-left, +3 upper-left,
        // +4 top, +5 upper-right, +6 middle
        const b = (6 - c) * 5;
        set(b + 4, x, yTop, x + W, yTop);         // top: left->right
        set(b + 6, x + W, yMid, x, yMid);         // middle: right->left
        set(b + 1, x + W, yBot, x, yBot);         // bottom: right->left
        set(b + 3, x, yMid, x, yTop);             // upper-left: bottom->top
        set(b + 5, x + W, yTop, x + W, yMid);     // upper-right: top->bottom
        set(b + 2, x, yBot, x, yMid);             // lower-left: bottom->top
        set(b + 0, x + W, yMid, x + W, yBot);     // lower-right: top->bottom
      } else {
        // Fake digit column: only bottom (+0), top (+1), middle (+2)
        const b = (6 - c) * 5 + 2;
        set(b + 1, x, yTop, x + W, yTop);         // top: left->right
        set(b + 2, x + W, yMid, x, yMid);         // middle: right->left
        set(b + 0, x + W, yBot, x, yBot);         // bottom: right->left
      }
    }
    return segs;
  }

  function colorAt(hex, ledIndex) {
    const o = 2 + ledIndex * 6;
    return [
      parseInt(hex.substr(o, 2), 16),
      parseInt(hex.substr(o + 2, 2), 16),
      parseInt(hex.substr(o + 4, 2), 16)
    ];
  }

  // Approximate what the eye sees on the real strip: FastLED applies the
  // TypicalLEDStrip correction (255,176,240) on output, and LEDs have a
  // different gamma than a monitor - lift midtones to compensate
  function displayColor(r, g, b) {
    return [
      Math.round(255 * Math.pow(r / 255, 0.7)),
      Math.round(255 * Math.pow((g * 176 / 255) / 255, 0.7)),
      Math.round(255 * Math.pow((b * 240 / 255) / 255, 0.7))
    ];
  }

  function render(hex) {
    // Detect LEDs per segment from the payload so the preview matches
    // any LEDS_PER_SEGMENT firmware build (7 per instructions, 9 here)
    const numLeds = Math.floor((hex.length - 2) / 6);
    const ledsPerSeg = Math.round((numLeds - NUM_SPOTS) / NUM_SEGMENTS);
    if (ledsPerSeg < 1 || numLeds < NUM_SEGMENTS * ledsPerSeg + NUM_SPOTS) return;
    const segmentLeds = NUM_SEGMENTS * ledsPerSeg;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#101016';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Cubby interiors: subtle back wall so the shelf structure stays visible
    for (let c = 0; c < 7; c++) {
      for (let rw = 0; rw < 2; rw++) {
        ctx.fillStyle = 'rgba(255,255,255,0.025)';
        ctx.fillRect(PAD + c * W + 5, PAD + rw * H + 5, W - 10, H - 10);
      }
    }

    // Spotlights: downlight sitting at the cubby ceiling, light falls into
    // the compartment. Wired zig-zag from the left: even columns upper
    // cubby first, odd columns lower cubby first.
    for (let i = 0; i < NUM_SPOTS; i++) {
      const [rr, rg, rb] = colorAt(hex, segmentLeds + i);
      if (rr + rg + rb === 0) continue;
      const [r, g, b] = displayColor(rr, rg, rb);
      const col = i >> 1;
      const row = (i % 2) ^ (col % 2);
      const x = PAD + col * W;
      const y = PAD + row * H;
      const cx = x + W / 2;
      const rgb = r + ',' + g + ',' + b;

      ctx.save();
      ctx.beginPath();
      ctx.rect(x + 7, y + 7, W - 14, H - 14);
      ctx.clip();  // keep the light inside this cubby, away from the segments

      // ambient bounce light filling the whole compartment
      ctx.fillStyle = 'rgba(' + rgb + ',0.10)';
      ctx.fillRect(x, y, W, H);

      // cone of light from the ceiling, fading towards the floor
      const grad = ctx.createRadialGradient(cx, y + H * 0.12, 2, cx, y + H * 0.12, H * 0.95);
      grad.addColorStop(0, 'rgba(' + rgb + ',0.65)');
      grad.addColorStop(0.35, 'rgba(' + rgb + ',0.28)');
      grad.addColorStop(1, 'rgba(' + rgb + ',0)');
      ctx.fillStyle = grad;
      ctx.fillRect(x, y, W, H);

      // the lamp itself: small bright dot at the ceiling
      ctx.fillStyle = 'rgba(255,255,255,0.9)';
      ctx.beginPath();
      ctx.arc(cx, y + 7, 2.5, 0, Math.PI * 2);
      ctx.fill();

      ctx.restore();
    }

    // Segments: each LED drawn as a sub-line along the segment
    const inset = THICKNESS * 0.9; // keep corners of adjacent segments apart
    ctx.lineCap = 'round';
    ctx.lineWidth = THICKNESS;
    for (let s = 0; s < NUM_SEGMENTS; s++) {
      const gm = segGeo[s];
      const dx = gm.x1 - gm.x0;
      const dy = gm.y1 - gm.y0;
      const len = Math.hypot(dx, dy);
      const ux = (dx / len) * inset;
      const uy = (dy / len) * inset;
      const x0 = gm.x0 + ux, y0 = gm.y0 + uy;
      const x1 = gm.x1 - ux, y1 = gm.y1 - uy;

      // does this segment show anything?
      let lit = false;
      for (let k = 0; k < ledsPerSeg; k++) {
        const [r, g, b] = colorAt(hex, s * ledsPerSeg + k);
        if (r + g + b > 0) { lit = true; break; }
      }

      if (lit) {
        // dark backing so the segment stands out against lit cubbies
        ctx.shadowBlur = 0;
        ctx.strokeStyle = 'rgba(10,10,14,0.85)';
        ctx.lineWidth = THICKNESS + 5;
        ctx.beginPath();
        ctx.moveTo(x0, y0);
        ctx.lineTo(x1, y1);
        ctx.stroke();
        ctx.lineWidth = THICKNESS;
      }

      for (let k = 0; k < ledsPerSeg; k++) {
        const [rr, rg, rb] = colorAt(hex, s * ledsPerSeg + k);
        const t0 = k / ledsPerSeg;
        const t1 = (k + 1) / ledsPerSeg;
        ctx.beginPath();
        ctx.moveTo(x0 + (x1 - x0) * t0, y0 + (y1 - y0) * t0);
        ctx.lineTo(x0 + (x1 - x0) * t1, y0 + (y1 - y0) * t1);
        if (rr + rg + rb === 0) {
          ctx.shadowBlur = 0;
          ctx.strokeStyle = lit ? 'rgba(255,255,255,0.10)' : 'rgba(255,255,255,0.07)';
        } else {
          const [r, g, b] = displayColor(rr, rg, rb);
          ctx.shadowColor = 'rgb(' + r + ',' + g + ',' + b + ')';
          ctx.shadowBlur = 14;
          ctx.strokeStyle = 'rgb(' + r + ',' + g + ',' + b + ')';
        }
        ctx.stroke();
      }
    }
    ctx.shadowBlur = 0;
  }

  async function poll() {
    if (fetching || document.hidden) return;
    fetching = true;
    try {
      const res = await fetch('/getleds', { cache: 'no-store' });
      if (res.ok) render(await res.text());
    } catch (e) {
      // clock unreachable - just keep trying
    } finally {
      fetching = false;
    }
  }

  document.addEventListener('DOMContentLoaded', function () {
    canvas = document.getElementById('ledPreview');
    if (!canvas) return;
    canvas.width = 7 * W + 2 * PAD;
    canvas.height = 2 * H + 2 * PAD;
    ctx = canvas.getContext('2d');
    segGeo = buildGeometry();
    poll();
    setInterval(poll, POLL_MS);
  });
})();
