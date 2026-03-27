#!/bin/bash

# Raylib Pong Game - Uninstall Script

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_status "Uninstalling Raylib Pong Game..."

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

rm -rf bin/
rm -f compile_commands.json

print_success "Raylib Pong Game has been uninstalled."
echo ""
echo -e "${RED}Note:${NC} System raylib was not removed. To remove it:"
echo "  Ubuntu/Debian: sudo apt remove libraylib-dev"
echo "  Fedora:        sudo dnf remove raylib-devel"
echo "  Arch:          sudo pacman -R raylib"
