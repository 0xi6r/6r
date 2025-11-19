# What if I wrote my own file editor?

## 6r, A Simple Windows TUI File Editor

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
- **Batch script**
- 
## Build Using build.bat
```bash

build.bat run      # Fastest way to test
build.bat clean    # Clean up before git commit
build.bat rebuild  # Force recompile everything
```

this is just a simple implemtation, things are breaking and I'll look into them later.
feel free to contribute, no strings attached.
