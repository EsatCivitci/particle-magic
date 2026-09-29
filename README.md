# 🌌 Particle Magic — GPU Compute Particle System

A real-time gravitational particle simulation running up to 10,000,000 particles in modern OpenGL via Compute Shaders, Shader Storage Buffer Objects (SSBOs), and additive color blending[cite: 3].

Developed for **CENG 469: Computer Graphics II** at Middle East Technical University[cite: 3].

---

## 📖 Technical Report & Documentation

Read the full technical breakdown, numerical integration schemes, and stress-test benchmarks:

* 👉 **[Read Directly on GitHub (`docs/index.md`)](docs/index.md)**

---

## 🎬 Demo Video

[![Particle Simulation in 3D Space using OpenGL](https://img.youtube.com/vi/x6esdj_qDn8/maxresdefault.jpg)](https://www.youtube.com/watch?v=x6esdj_qDn8)

👉 **[Watch the full Demo on YouTube](https://www.youtube.com/watch?v=x6esdj_qDn8)**

---

## 🛠️ Key Highlights

* **GPU Compute Dispatch**: Fully offloads particle position integration, velocity calculations, and age lifecycles to OpenGL compute shaders[cite: 3].
* **Dynamic Attractor Mechanics**: Support for up to 12 configurable gravitational attractors with adjustable masses[cite: 3].
* **Visual Styling & Blending**: Age-dependent color gradients combined with additive blending (`GL_SRC_ALPHA`, `GL_ONE`) to produce glowing cosmic ribbons[cite: 3].
* **Interactive Controls**:
  * `W` / `S` — Scale simulation speed ($\Delta t$)[cite: 3].
  * `R` — Freeze / unfreeze simulation dynamics[cite: 3].
  * `T` — Toggle on-screen UI text[cite: 3].
  * `G` — Toggle mouse click mode (**Origin** vs. **Attractor**)[cite: 3].
  * `Left Click` — Reposition particle origin or spawn a new attractor[cite: 3].
  * `Right Click` — Delete the last placed attractor[cite: 3].
  * `Scroll Wheel` — Adjust new attractor mass ($10 \le M \le 100$)[cite: 3].

---

## ⚡ Performance Benchmarks

Execution format: `./main PointCount PointSize`[cite: 3]

| Particle Count / Point Size[cite: 5] | Size = 1[cite: 5] | Size = 10[cite: 5] | Size = 25[cite: 5] | Size = 50[cite: 5] |
| :--- | :---: | :---: | :---: | :---: |
| **10**[cite: 5] | 950 fps[cite: 5] | 950 fps[cite: 5] | 950 fps[cite: 5] | 950 fps[cite: 5] |
| **1000**[cite: 5] | 950 fps[cite: 5] | 900 fps[cite: 5] | 800 fps[cite: 5] | 600 fps[cite: 5] |
| **100000**[cite: 5] | 450 fps[cite: 5] | 210 fps[cite: 5] | 90 fps[cite: 5] | 30 fps[cite: 5] |
| **1000000**[cite: 5] | 110 fps[cite: 5] | 40 fps[cite: 5] | 13 fps[cite: 5] | 6 fps[cite: 5] |
| **10000000**[cite: 5] | 20 fps[cite: 5] | 8 fps[cite: 5] | 3 fps[cite: 5] | 1 fps[cite: 5] |

---

## 🚀 Build & Run

```bash
make
./main 1000000 1
