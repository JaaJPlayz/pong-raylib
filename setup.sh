#!/bin/bash

# Raylib Pong Game - Automatic Setup Script
# This script installs system raylib and sets up the Pong game

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on Linux
check_system() {
    print_status "Checking system compatibility..."

    if [[ "$OSTYPE" != "linux-gnu"* ]]; then
        print_error "This script is designed for Linux systems"
        exit 1
    fi

    local missing_packages=()

    if ! command -v gcc &> /dev/null; then
        missing_packages+=("gcc")
    fi

    if ! command -v make &> /dev/null; then
        missing_packages+=("make")
    fi

    if [ ${#missing_packages[@]} -ne 0 ]; then
        print_error "Missing required packages: ${missing_packages[*]}"
        print_status "Install them with:"
        echo "  Ubuntu/Debian: sudo apt install ${missing_packages[*]}"
        echo "  Fedora: sudo dnf install ${missing_packages[*]}"
        echo "  Arch: sudo pacman -S ${missing_packages[*]}"
        exit 1
    fi

    print_success "System compatibility check passed"
}

# Install system raylib and required dependencies
install_dependencies() {
    print_status "Installing system raylib and dependencies..."

    if command -v apt &> /dev/null; then
        sudo apt update
        sudo apt install -y build-essential libraylib-dev
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y gcc make raylib-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --needed base-devel raylib
    else
        print_warning "Could not detect package manager. Please install raylib development libraries manually."
        exit 1
    fi

    print_success "Dependencies installed"
}

# Create compile_commands.json for LSP
setup_lsp() {
    print_status "Setting up LSP configuration..."

    cat > compile_commands.json << EOF
[
  {
    "directory": "$(pwd)",
    "command": "gcc -o ./bin/main ./src/main.c -lGL -lm -ldl -lX11 -lpthread -lXrandr -lXext -lraylib",
    "file": "./src/main.c"
  }
]
EOF

    print_success "LSP configuration created"
}

# Create bin directory and compile game
compile_game() {
    print_status "Creating bin directory..."
    mkdir -p bin

    print_status "Compiling Pong game..."
    if make compile; then
        print_success "Game compiled successfully"
    else
        print_error "Compilation failed"
        exit 1
    fi
}

# Test the game
test_game() {
    print_status "Testing game installation..."

    if [ -f "bin/main" ]; then
        print_success "Game executable found"
        print_status "You can now run the game with:"
        echo "  make run"
        echo "  ./bin/main"
    else
        print_error "Game executable not found"
        exit 1
    fi
}

# Main installation function
main() {
    print_status "Starting Raylib Pong Game installation..."

    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cd "$SCRIPT_DIR"

    check_system
    install_dependencies
    setup_lsp
    compile_game
    test_game

    print_success "Installation completed successfully!"
    echo ""
    echo -e "${GREEN}Raylib Pong Game is ready to play!${NC}"
    echo ""
    echo "Quick start commands:"
    echo "  make run                      # Compile and run"
    echo "  ./bin/main                    # Run directly"
    echo ""
}

# Run main function
main "$@"
