# Raylib Pong Game

A classic Pong game implemented in C using the Raylib game development library.

## Features

- **Classic Pong Gameplay**: Two-player Pong with player vs bot
- **Customizable Difficulty**: Four difficulty levels (easy, medium, hard, impossible)
- **Configurable Win Conditions**: Set custom points required to win
- **Sound Effects**: Paddle hits and scoring sounds
- **Winner Screen**: Displays winner with 5-second countdown before closing
- **Progressive Difficulty**: Ball speed increases with each paddle hit
- **Imperfect Bot AI**: Bot makes mistakes on easier difficulties

## Controls

- **W**: Move left paddle up
- **S**: Move left paddle down
- **ESC**: Exit game

## Installation

### Quick Setup (Recommended)

**Automatic Setup Script:**
```bash
# Clone or download the project
# Navigate to project directory
cd pong-raylib

# Run the automatic setup script
./setup.sh
```

The setup script will:
- ✅ Check system compatibility
- ✅ Install all required dependencies
- ✅ Download and compile Raylib automatically
- ✅ Update Makefile for local Raylib
- ✅ Configure LSP support
- ✅ Compile the game
- ✅ Create uninstall script

**Uninstallation:**
```bash
# To completely remove the game and Raylib
./uninstall.sh
```

### Manual Installation

#### Prerequisites
- GCC compiler
- Raylib library (included in `lib/` directory)
- Linux system with X11 support

#### Building
```bash
# Compile the game
make compile

# Or compile and run in one command
make compile-run

# Clean build artifacts
make clean
```

## Usage

### Basic Usage

```bash
# Run with default settings (5 points to win, medium difficulty)
./bin/main
```

### Command Line Arguments

```bash
# Set custom win score
./bin/main --win-req 10

# Set difficulty level
./bin/main --difficulty easy

# Combine both options
./bin/main --win-req 7 --difficulty hard
```

### Difficulty Levels

| Level | Paddle Speed | Ball Speed | Bot Reaction | Bot Error Chance |
|--------|---------------|------------|--------------|-----------------|
| Easy   | 1.8x         | 1.5x       | 50%          | 8%              |
| Medium | 1.2x         | 2.0x       | 70%          | 2%              |
| Hard   | 0.8x         | 2.5x       | 90%          | 1%              |
| Impossible | 0.6x      | 3.0x       | 100%         | 0%              |

## Game Mechanics

### Scoring
- First player to reach the required score wins
- Ball resets to center after each point
- Ball shoots toward the player who was scored on

### Ball Physics
- Ball speed increases by 10% with each paddle hit (max 2.0x)
- Random Y-velocity variation after paddle hits
- Ball bounces off top and bottom walls

### Bot AI
- Tracks ball position with center-aligned paddle
- Reaction delay based on difficulty
- Random error chance for imperfect behavior
- Speed limitations to make game beatable

## Project Structure

```
pong-raylib/
├── src/
│   └── main.c          # Main game source code
├── lib/
│   └── raylib/         # Raylib library source
├── bin/
│   └── main            # Compiled executable
├── Makefile            # Build configuration
├── compile_commands.json # LSP configuration
└── README.md           # This file
```

## Code Structure

The game is organized into several key components:

### Data Structures
- **Paddle**: Player paddle with position, size, speed, and color
- **Ball**: Game ball with position, velocity, and visual properties
- **GameState**: Complete game state including scores and settings
- **DifficultySettings**: Configuration for different difficulty levels

### Core Functions
- `initializeGame()`: Sets up game state and objects
- `gameLoop()`: Main game loop handling input, physics, and rendering
- `handlePlayerInput()`: Processes keyboard input for left paddle
- `handleBotAI()`: Controls right paddle with imperfect AI
- `updateBall()`: Updates ball position and handles wall collisions
- `handlePaddleCollision()`: Detects and responds to paddle hits
- `handleScoring()`: Manages scoring and ball resets
- `drawGame()`: Renders all game elements
- `displayWinner()`: Shows winner screen with countdown

### Utility Functions
- `getDifficultySettings()`: Returns difficulty-specific parameters
- `initializeSounds()`: Generates sound effects programmatically
- `parseArguments()`: Handles command line argument parsing
- `resetBall()`: Resets ball to center with random angle

## Development

### Code Style
- Well-documented with comprehensive function comments
- Modular design with clear separation of concerns
- Consistent naming conventions
- Type-safe structure usage

### Building from Source
```bash
# Ensure Raylib is available in lib/raylib/
make compile
```

### Running Tests
```bash
# Test different difficulty levels
./bin/main --difficulty easy
./bin/main --difficulty hard
./bin/main --difficulty impossible

# Test different win conditions
./bin/main --win-req 1
./bin/main --win-req 10
```

## Troubleshooting

### Common Issues

**Game doesn't start:**
- Ensure Raylib is properly compiled in `lib/raylib/`
- Check that all required system libraries are installed
- Verify executable permissions on `bin/main`

**No sound:**
- Check that audio system is working
- Ensure PulseAudio or ALSA is available

**Performance issues:**
- Try lower resolution by modifying `SCREEN_WIDTH` and `SCREEN_HEIGHT`
- Reduce sound quality in `initializeSounds()`

### Compilation Errors
```bash
# Clean and rebuild
make clean
make compile

# Check Raylib installation
ls lib/raylib/src/
```

## License

This project is for educational purposes. Raylib is licensed under the zlib/libpng license.

## Contributing

Feel free to fork and modify this project. Potential improvements:
- Two-player mode (human vs human)
- Power-ups and special effects
- Menu system
- High score tracking
- Different game modes

## Acknowledgments

- **Raylib**: Simple and easy-to-use game development library
- **C Programming**: Classic game development language
- **Pong**: Original arcade game by Atari (1972)
