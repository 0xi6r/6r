# Simple Windows TUI File Editor

A lightweight, modular text editor for Windows command line with a terminal user interface (TUI). Built in C with a focus on simplicity, modularity, and ease of use.

## Features

- 📝 **Line-based text editing** with line numbers
- ⌨️ **Intuitive keyboard controls** (arrow keys, Home, End, Page Up/Down)
- 💾 **File operations** (open, save, save-as, new)
- 🎨 **Status bar** showing file info and cursor position
- 🏗️ **Modular architecture** for easy maintenance and extension
- ⚡ **Lightweight** - minimal dependencies, fast startup
- 🔧 **Cross-platform build** - Makefile and batch script support

## Requirements

- **Windows OS** (uses Windows Console API)
- **GCC compiler** (MinGW or similar)
- **GNU Make** (optional - batch script alternative provided)

## Installation

### Option 1: Using Makefile (Linux/WSL/MinGW)
```bash
make          # Build
make run      # Build and run
make clean    # Clean artifacts

## Option 2: Using build.bat
build.bat run      # Fastest way to test
build.bat clean    # Clean up before git commit
build.bat rebuild  # Force recompile everything