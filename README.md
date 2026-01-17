# TSP Solver - Hungarian Algorithm with Subtour Patching

An interactive visualization of the Traveling Salesman Problem (TSP) solver using the Hungarian Algorithm combined with iterative subtour elimination.

## Demo

<video src="Demo.mp4" controls width="800"></video>

**Features:**
- Interactive city placement via mouse clicks
- Step-by-step visualization of the Hungarian algorithm
- Animated traveling salesman on the final tour
- Real-time distance matrix display
- Fullscreen OpenGL rendering

## Libraries Used

- **FreeGLUT** - OpenGL Utility Toolkit for window management and rendering
- **OpenGL** - Graphics rendering API
- **nlohmann/json** - JSON parsing for city data (header-only library)
- **munkres-cpp** - Hungarian algorithm implementation for assignment problems

## How It Works

### Algorithm Implementation

1. **Distance Matrix Construction**
   - Calculates Euclidean distances between all city pairs
   - Creates a cost matrix for the assignment problem

2. **Hungarian Algorithm**
   - Solves the assignment problem to find minimum cost perfect matching
   - Uses the Munkres algorithm (O(n³) complexity)
   - Produces initial tour assignments

3. **Subtour Elimination**
   - Detects disconnected cycles in the assignment
   - Iteratively breaks subtours by forbidding specific edges
   - Re-runs Hungarian algorithm with modified costs
   - Continues until a single Hamiltonian cycle is found

4. **Visualization**
   - Each step shows the current assignment and detected subtours
   - Different colors represent different subtours
   - Final tour displayed in green with total distance

### Technical Details

- **Coordinate System**: Pixel coordinates normalized to OpenGL's [-1, 1] range
- **Animation**: Timer-based animation at ~60 FPS for salesman movement
- **Interactive UI**: Real-time window resizing with proper coordinate transformation

## How to Build

### Prerequisites
- MinGW-w64 or similar GCC compiler for Windows
- FreeGLUT library (included)

### Compilation

Run the build script:
```bash
build.bat
```

This compiles all source files and links with FreeGLUT and OpenGL libraries.

### macOS Build Instructions

1. **Install Dependencies** (using [Homebrew](https://brew.sh/)):
   ```bash
   brew install freeglut
   ```

2. **Compile**:
   ```bash
   clang++ -std=c++17 -o ComputerGraphics \
     ComputerGraphics.cpp City.cpp TSPAlgorithm.cpp MatrixPanel.cpp RenderUtils.cpp munkres-cpp/src/munkres.cpp \
     -I. -Imunkres-cpp/src \
     -L/opt/homebrew/lib -L/usr/local/lib \
     -I/opt/homebrew/include -I/usr/local/include \
     -lfreeglut -framework OpenGL
   ```

3. **Run**:
   ```bash
   ./ComputerGraphics
   ```

## Runtime Requirements

To run the compiled executable, ensure these files are in the same directory as ComputerGraphics.exe:

**Required Files:**
- reeglut.dll - FreeGLUT runtime library
- cities.json - Initial city data (can be empty: [])

**Optional Files:**
- Additional JSON files with city data

**Directory Structure:**
```
ComputerGraphics/
├── ComputerGraphics.exe    ← Main executable
├── freeglut.dll            ← Required!
└── cities.json             ← Required!
```

⚠️ **Important:** The program will not run without reeglut.dll in the same directory!

## How to Run

```bash
ComputerGraphics.exe
```

The program opens in fullscreen mode and loads cities from `cities.json`.

### Controls

| Key | Action |
|-----|--------|
| **S** | Solve TSP using Hungarian Algorithm |
| **N** | Next step in visualization |
| **P** | Previous step |
| **M** | Toggle distance matrix panel |
| **A** | Animate traveling salesman (on final tour) |
| **F** | Toggle animation speed (SLOW ⟷ FAST) |
| **Click** | Add new city at cursor position |
| **Q/ESC** | Quit application |

## City Data Format

Cities can be defined in `cities.json`:

```json
[
  {"x": 150, "y": 200, "name": "City0"},
  {"x": 400, "y": 300, "name": "City1"},
  {"x": 250, "y": 450, "name": "City2"}
]
```

Coordinates are in pixels. The `name` field is optional.

## Project Structure

```
Computer Graphics/
├── ComputerGraphics.cpp    # Main application and OpenGL rendering
├── TSPAlgorithm.cpp/hpp    # Hungarian algorithm and subtour patching
├── City.cpp/hpp            # City data structure
├── MatrixPanel.cpp/hpp     # Distance matrix visualization
├── RenderUtils.cpp/hpp     # Text rendering utilities
├── cities.json             # City data file
├── build.bat               # Build script
├── freeglut.dll            # FreeGLUT runtime library
└── munkres-cpp/            # Hungarian algorithm implementation
```

## Credits

**Algorithm:** Hungarian Algorithm (Kuhn-Munkres) for optimal assignment
**Library Authors:**
- FreeGLUT Team - OpenGL Utility Toolkit
- Nlohmann - Modern C++ JSON library
- munkres-cpp contributors - C++ implementation of Hungarian algorithm

**TSP Approach:** Assignment problem formulation with iterative subtour patching

---

**Academic Project; ComputerGraphics (III/II)** - Traveling Salesman Problem Visualization

**Submitted by:**
- **Prabin Aryal** - CE2022-08
- **Dikshit Bhatta** - CE2022-13
