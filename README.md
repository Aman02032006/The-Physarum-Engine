# The Physarum Engine

> A high-performance, agent-based artificial life simulation of *Physarum polycephalum* (slime mold) built in C++ and OpenGL.

## About The Project
**The Physarum Engine** is an exploration of emergent biological behaviors through the lens of systems programming. It simulates the growth, network optimization, and environmental adaptation of slime mold using a highly optimized agent-based architecture. 

By combining a custom OpenGL rendering pipeline with environmental diffusion (blur/decay) and sensory-driven steering algorithms, this engine demonstrates how simple local rules can generate incredibly complex, self-organizing organic webs.

### Tech Stack & Architecture
* **Language:** C++17
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Windowing & Input:** GLFW3
* **Extension Loader:** GLAD
* **Build System:** CMake

**Architectural Highlights:**
* **Cache-Aware Memory:** Abandons standard 2D `std::vector` abstractions in favor of flattened, 1D raw pointer arrays allocated on the heap to maximize CPU cache hit rates during physics updates.
* **Double-Buffered Environment:** Implements a strict read/write buffer swap for the pheromone grid to completely eliminate read/write data races (tearing) during the diffusion step.
* **Zero-Allocation Main Loop:** All memory (agent structs, grid state, texture buffers) is pre-allocated at startup, ensuring deterministic performance and preventing heap fragmentation during the 60 FPS simulation loop.
* **Custom Shader Pipeline:** Utilizes custom GLSL Vertex and Fragment shaders to map the 1D pheromone array directly into VRAM, bypassing expensive pixel-by-pixel CPU rendering.

---

## 📈 Development Progress Log

### Week 1: Systems Architecture & Memory Control
The foundation of the engine. We established the core memory model by mathematically mapping a 2D grid structure onto a contiguous 1D array (`float*`). This week focused heavily on pointer arithmetic, manual memory management (`new` / `delete[]`), and boundary validation to create a safe, high-speed environment for our future agents.

### Week 2: The Graphics Pipeline & Swarm Mechanics
Implemented the OpenGL wrapper and shaders to visualize our memory grid. We introduced the `Agent` struct and spawned 10,000 independent particles onto the screen. Agents were given basic trigonometric movement (`cos`/`sin`) and hard boundary-collision logic to keep them within the viewport.
* **Result:** A highly optimized "bouncing swarm" of pixels resembling television static.

https://github.com/user-attachments/assets/24caacb1-5b43-4524-91eb-32d11dd448fc

### Week 3: Artificial Life & The Organic Web
The critical transition from bouncing particles to an artificial organism. 
1. **The Environment:** Implemented a 3x3 convolution blur and decay matrix across the grid to simulate pheromone diffusion and evaporation. Used a Double-Buffer architecture to prevent directional tearing.
2. **The AI:** Equipped agents with a 3-point continuous sensory system (Forward, Left, Right). Implemented the discrete Jeff Jones steering algorithm, complete with symmetry-breaking logic for head-on collisions. 
* **Result:** Agents now leave trails and actively seek out the trails of others, creating a positive feedback loop that self-organizes the chaotic swarm into stable, biological networks and veins.

https://github.com/user-attachments/assets/38f69967-57ec-45d5-8c1e-58e7ed7ac32d

### Week 4: Environmental Interaction & Mazes
This week introduced a major feature: the ability to define the simulation environment using external image files.
1.  **Image-Based Terrain:** Integrated the `stb_image.h` library to load grayscale images and map them onto the simulation grid. Dark pixels become impassable `OBSTACLE` terrain, while light pixels remain open `AIR`. This allows for the creation of complex mazes and boundaries.
2.  **Static Food & Home:** Added `FOOD` and `HOME` as distinct `TerrainType` enumerations. Agents now have dedicated logic in their `detectState` and `senseAndSteer` methods to interact with these zones, turning around upon reaching food (to return home) or home (to search for more food).
3.  **Command-Line Loading:** The engine now accepts a file path as a command-line argument, allowing users to dynamically load different maze configurations at runtime without recompiling.

*   **Result:** The slime mold now intelligently navigates complex environments. When presented with a maze, its emergent web-like structure efficiently finds the shortest path between home and multiple food sources, demonstrating the network optimization behavior seen in real-world *Physarum polycephalum*.

---

## Compilation & Build Instructions

This project uses CMake to ensure cross-platform compatibility. 

### Prerequisites
* **Windows:** MSYS2 (MinGW 64-bit) with `gcc`, `cmake`, and `glfw` installed.
* **Linux:** `build-essential`, `cmake`, `libglfw3-dev`, `libgl1-mesa-dev`
* **macOS:** Xcode Command Line Tools, `cmake`, `glfw` (via Homebrew)

### Build Steps

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/Aman02032006/Physarum-Engine.git](https://github.com/Aman02032006/Physarum-Engine.git)
   cd Physarum-Engine
   ```

2. **Generate the Build System:**

   * Windows (MinGW):
     ```Bash
     cmake -G "MinGW Makefiles" -S . -B build
     ```
   * Linux / macOS:
     ```Bash
     cmake -S . -B build
     ```
3. **Compile the Engine:**

   ```Bash
   cmake --build build
   ```
4. **Run the Simulation:**

   * Windows: `.\build\PhysarumEngine.exe`
   * Linux / macOS: `./build/PhysarumEngine`

Developed by Aman Srivastava, Garvit Bhutani, Naina Bhalla and Piyush Verma.
