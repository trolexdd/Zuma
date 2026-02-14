# 🐸 Zuma - EIS Edition

<div align="center">

![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![OpenGL](https://img.shields.io/badge/OpenGL-2.0-green?style=for-the-badge&logo=opengl)
![GLFW](https://img.shields.io/badge/GLFW-3.3-orange?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-lightgrey?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

*A complete recreation of the classic Zuma puzzle game with procedural graphics and intelligent AI*

[Features](#-features) • [Installation](#-installation) • [Controls](#-controls) • [Building](#-building-from-source) • [Contributing](#-contributing)

---

</div>

## 📖 About

**Zuma - EIS Edition** is a faithful recreation of the beloved PopCap puzzle game, built entirely from scratch in pure C with OpenGL. Every visual element is procedurally generated—from the animated frog to the glowing portal effects. The game features three difficulty levels, an intelligent AI bot that can play for you, and smooth 60 FPS gameplay.

### ✨ What Makes This Special

- 🎨 **100% Procedural Graphics** - No textures, all visuals rendered with OpenGL primitives
- 🤖 **Smart AI Bot** - Watch the AI play or learn strategies (Press `B`)
- 🐸 **Hand-Crafted Frog Character** - Detailed character design with expressive eyes
- 💫 **Beautiful Particle Effects** - Satisfying visual feedback for every match
- 📊 **Comprehensive Statistics** - Score tracking, combo system, and accuracy metrics
- 🎯 **Three Difficulty Levels** - Each with unique path designs and speeds

---

## 🎮 Features

### Core Gameplay
- **Classic Match-3 Mechanics** - Match 3+ balls of the same color to destroy them
- **Combo System** - Chain reactions multiply your score
- **Progressive Difficulty** - Three levels: Easy (Spiral), Medium (Wave), Hard (Figure-8)
- **Smart Ball Spawning** - Balls appear gradually, not all at once
- **Smooth Physics** - Ball collision and chain movement feel natural

### Visual Features
- ✨ Gradient-shaded balls with realistic highlights
- 🌀 Animated portal with pulsating glow effects
- 💥 Particle explosion system (200 simultaneous particles)
- ⭐ Procedural starfield background
- 🎨 Custom bitmap font rendering (5x7 pixel characters)
- 🐸 Fully animated frog with eye tracking
- 📏 Progress bar showing danger level

### AI Bot System
The bot uses intelligent target selection:
- Analyzes the entire ball chain
- Prioritizes creating 3+ matches (highest priority)
- Considers both current and next ball colors
- Accounts for ball distance to portal (danger zone)
- Makes strategic decisions in real-time

Press **B** to toggle the AI on/off and watch it play!

---

## 🎯 Game Modes

| Difficulty | Speed | Colors | Path Type | Balls |
|------------|-------|--------|-----------|-------|
| **🟢 Easy** | 1.0x (35 u/s) | 3 | Spiral | 12 |
| **🟡 Medium** | 1.6x (55 u/s) | 4 | Wave | 18 |
| **🔴 Hard** | 2.1x (75 u/s) | 5 | Figure-8 | 25 |

### Scoring System
- **Base Score**: 10 points per ball destroyed
- **Combo Multiplier**: Each consecutive match increases multiplier
- **Formula**: `Points = Balls × 10 × Combo`

Example: Destroying 4 balls with a 3x combo = **120 points**!

---

## 🎮 Controls

| Input | Action |
|-------|--------|
| **Mouse Movement** | Aim the frog's mouth |
| **Left Click** | Shoot ball |
| **B** | Toggle AI Bot ON/OFF |
| **H** | Get strategic hint |
| **P** | Pause/Resume game |
| **ESC** | Return to menu |

---

## 🚀 Installation

### Windows

#### Option 1: Pre-compiled Binary
Download `zuma.exe` from [Releases](../../releases) and run it!

#### Option 2: Compile from Source
```cmd
# Using Visual Studio Developer Command Prompt
cl main.c /I"C:\path\to\glfw\include" /link glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib

# Run
main.exe
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential libglfw3-dev libgl1-mesa-dev

# Compile
gcc main.c -lglfw -lGL -lm -o zuma

# Run
./zuma
```

### macOS

```bash
# Install GLFW via Homebrew
brew install glfw

# Compile
gcc main.c -lglfw -framework OpenGL -lm -o zuma

# Run
./zuma
```

---

## 🛠️ Building from Source

### Prerequisites
- C Compiler (GCC 4.9+, Clang 3.5+, or MSVC 2015+)
- GLFW 3.3 or later
- OpenGL 2.0+ support

### Quick Build

**Using Make:**
```bash
make          # Build the game
make run      # Build and run
make clean    # Remove build files
```

**Using CMake:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
./zuma
```

**Manual Compilation:**
```bash
# Linux/macOS
gcc main.c -lglfw -lGL -lm -o zuma

# Windows (MinGW)
gcc main.c -lglfw3 -lopengl32 -lgdi32 -o zuma.exe
```

---

## 📊 Technical Highlights

### Architecture
```
┌─────────────────────────────────────┐
│       Game State Machine            │
├─────────────────────────────────────┤
│  MENU → LEVEL_SELECT → PLAYING     │
│    ↑         ↓            ↓         │
│    └─── WON/LOST ────────┘          │
└─────────────────────────────────────┘
```

### Code Statistics
- **Lines of Code**: ~1,850
- **Functions**: 50+
- **Rendering System**: Immediate Mode OpenGL
- **Memory Usage**: < 10 MB
- **Target FPS**: 60

### Path Generation
Three mathematically-generated paths:
1. **Spiral** - Archimedean spiral from edge to center
2. **Wave** - Sinusoidal oscillations along a spiral
3. **Figure-8** - Complex Lissajous curve pattern

### AI Algorithm
```python
Priority Calculation:
├─ Current ball + 2+ matches  → Priority: 100+
├─ Current ball + 1 match     → Priority: 50+
├─ Next ball + 2+ matches     → Priority: 80+
└─ Distance bonus             → +distance/10
```

---

## 🐛 Known Issues & Limitations

- **No Sound**: OpenGL doesn't handle audio (consider adding OpenAL in the future)
- **AI Limitations**: Bot can't predict chain reactions (intentional for difficulty)
- **Visual Glitches**: Balls may briefly overlap during rapid spawning (cosmetic only)

---

## 🗺️ Roadmap

### Planned Features
- [ ] 🔊 Sound effects and background music (OpenAL integration)
- [ ] 💾 High score persistence (save to file)
- [ ] 🎁 Power-ups system (bombs, slow-motion, reverse)
- [ ] 🌈 More color schemes and themes
- [ ] 📱 Touch controls for mobile platforms
- [ ] 🏆 Achievement system
- [ ] 🎨 Modern OpenGL renderer (shaders)
- [ ] 🌍 More level designs

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add some AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### Contribution Ideas
- 🎵 Add sound effects using OpenAL
- 🎨 Create alternative themes
- 🤖 Improve AI algorithm
- 📚 Write better documentation
- 🐛 Fix bugs and issues
- ✨ Add new visual effects

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Original Game**: Zuma by PopCap Games (2003)
- **Libraries**: GLFW for windowing, OpenGL for graphics
- **Inspiration**: Classic puzzle games and procedural art

---

## 💡 Tips & Tricks

- **Create Combos**: Try to set up chain reactions for massive points
- **Use the Hint**: Press `H` when stuck for strategic advice
- **Watch the Bot**: Enable AI mode (`B`) to learn advanced strategies
- **Plan Ahead**: Always check the "NEXT" ball indicator
- **Danger Zone**: The progress bar turns red when balls are near the portal
- **Accuracy Matters**: Your accuracy percentage is shown after each game

---

## 📞 Contact & Support

- **Issues**: [Report bugs or request features](../../issues)
- **Discussions**: [Join the community discussions](../../discussions)
- **Pull Requests**: [Contribute code improvements](../../pulls)

---

<div align="center">

### ⭐ Star this repository if you enjoyed the game! ⭐

Made with ❤️ using C and OpenGL

**[⬆ Back to Top](#-zuma---eis-edition)**

</div>
