# Environment Setup Guide (macOS M4)

This guide helps you set up your development environment on Apple Silicon Mac for the CS-6200 preparation curriculum.

## Prerequisites Checklist

- [ ] Xcode Command Line Tools
- [ ] Homebrew
- [ ] GCC/Clang compiler
- [ ] GDB or LLDB debugger
- [ ] Docker Desktop (for Linux-specific tools)
- [ ] VS Code with C/C++ extension
- [ ] Git

---

## Step 1: Xcode Command Line Tools

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Verify installation
clang --version
```

You should see output showing the Apple clang version.

---

## Step 2: Homebrew

```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add to PATH (for Apple Silicon)
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"

# Verify
brew --version
```

---

## Step 3: Install Development Tools

```bash
# Install GCC (optional, clang works for most exercises)
brew install gcc

# Install GDB (limited on macOS, but useful reference)
brew install gdb

# Install other useful tools
brew install make cmake wget 
# make is gmake on macOS
```

### Note on GDB vs LLDB
On macOS, LLDB (Apple's debugger) is the native option and works without code signing issues. GDB requires code signing on macOS. For most exercises, you can use either:

```bash
# Using LLDB (recommended on macOS)
lldb ./program

# LLDB commands are similar but slightly different:
# GDB: break main  →  LLDB: breakpoint set --name main
# GDB: run         →  LLDB: run
# GDB: bt          →  LLDB: thread backtrace
# GDB: print var   →  LLDB: print var (same!)
# GDB: next        →  LLDB: next (same!)
# GDB: step        →  LLDB: step (same!)
```

---

## Step 4: Docker Desktop (Critical for M4 Mac)

Docker is **essential** for this curriculum because several tools don't work on Apple Silicon:
- **Valgrind** - memory leak detection
- **Address Sanitizer** - some features limited
- **POSIX Message Queues** - not available on macOS

### Install Docker Desktop

1. Download from [Docker Desktop for Mac](https://www.docker.com/products/docker-desktop/)
2. Choose "Apple Silicon" version
3. Install and start Docker Desktop
4. Verify installation:

```bash
docker --version
docker run hello-world
```

### Build the Development Container

From the curriculum root directory:

```bash
# Build the container
cd docker
docker build -t gios-prep .

# Test the container
docker run --rm gios-prep gcc --version
```

### Using the Development Container

```bash
# Interactive shell in container (mounts current directory)
docker run -it --rm -v $(pwd):/code -w /code gios-prep bash

# Run a specific command
docker run --rm -v $(pwd):/code -w /code gios-prep make

# Run Valgrind on a program
docker run --rm -v $(pwd):/code -w /code gios-prep valgrind ./program

# Compile and run with Address Sanitizer
docker run --rm -v $(pwd):/code -w /code gios-prep \
    bash -c "gcc -fsanitize=address -g program.c -o program && ./program"
```

---

## Step 5: VS Code Setup

### Install VS Code
Download from [Visual Studio Code](https://code.visualstudio.com/)

### Install Extensions

```bash
# Install via command line
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension ms-vscode.makefile-tools
code --install-extension vadimcn.vscode-lldb
code --install-extension ms-azuretools.vscode-docker
```

Or install manually:
1. C/C++ (Microsoft)
2. CMake Tools
3. Makefile Tools
4. CodeLLDB (for debugging)
5. Docker

### Configure VS Code for C Development

Create `.vscode/c_cpp_properties.json` in your project:

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**",
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include"
            ],
            "defines": [],
            "macFrameworkPath": [
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-arm64"
        }
    ],
    "version": 4
}
```

Create `.vscode/tasks.json` for build tasks:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build with clang",
            "type": "shell",
            "command": "clang",
            "args": [
                "-g",
                "-Wall",
                "-Wextra",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

---

## Step 6: GitHub Codespaces (Backup Option)

If Docker gives you trouble, GitHub Codespaces provides a free Linux VM:

1. Push this repository to GitHub
2. Open the repository on github.com
3. Click "Code" → "Codespaces" → "Create codespace"
4. You'll have a full Linux environment with all tools pre-installed

---

## Step 7: Verify Your Setup

Run the verification script:

```bash
cd 01_c_foundations/01_environment
make test
```

Or manually verify:

```bash
# Compiler
clang --version

# Debugger
lldb --version

# Docker
docker run --rm gios-prep echo "Docker works"

# Build a test program
echo '#include <stdio.h>
int main() { printf("Setup complete!\\n"); return 0; }' > test.c
clang -Wall -g test.c -o test
./test
rm test test.c
```

---

## Compilation Flags Reference

Throughout the curriculum, you'll use these flags:

```bash
# Standard debug build
clang -Wall -Wextra -g program.c -o program

# With optimization (for performance testing)
clang -Wall -Wextra -O2 program.c -o program

# With threading (pthreads)
clang -Wall -Wextra -g -pthread program.c -o program

# With math library
clang -Wall -Wextra -g program.c -o program -lm

# Address Sanitizer (use Docker on M4)
docker run --rm -v $(pwd):/code -w /code gios-prep \
    gcc -fsanitize=address -g program.c -o program
```

### Flag Meanings
- `-Wall` - Enable all common warnings
- `-Wextra` - Enable extra warnings
- `-g` - Include debug symbols (for GDB/LLDB)
- `-O2` - Optimization level 2
- `-pthread` - Enable POSIX threads
- `-fsanitize=address` - Enable Address Sanitizer (memory errors)

---

## M4 Mac Limitations & Workarounds

### 1. Valgrind Not Available
**Use Docker:**
```bash
docker run --rm -v $(pwd):/code -w /code gios-prep valgrind --leak-check=full ./program
```

### 2. Address Sanitizer Limited
**Use Docker for full support:**
```bash
docker run --rm -v $(pwd):/code -w /code gios-prep \
    bash -c "gcc -fsanitize=address -g test.c -o test && ./test"
```

### 3. POSIX Message Queues Not Available
Exercises in `03_concurrency_ipc/07_message_queues/` must be run in Docker:
```bash
docker run -it --rm -v $(pwd):/code -w /code gios-prep bash
```

### 4. GDB Code Signing Required
Use LLDB instead, or run GDB in Docker:
```bash
# LLDB on macOS (no signing needed)
lldb ./program

# GDB in Docker
docker run -it --rm -v $(pwd):/code -w /code gios-prep gdb ./program
```

---

## Troubleshooting

### "xcrun: error: invalid active developer path"
```bash
xcode-select --install
```

### Docker: "Cannot connect to the Docker daemon"
- Open Docker Desktop application
- Wait for it to start (whale icon in menu bar)
- Try again

### Permission denied when running Docker
```bash
# Add yourself to docker group (then log out and back in)
sudo dscl . -append /Groups/docker GroupMembership $(whoami)
```

### VS Code not finding headers
- Verify c_cpp_properties.json is correct
- Reload VS Code window (Cmd+Shift+P → "Reload Window")

---

## Quick Reference Card

```bash
# Compile
clang -Wall -Wextra -g file.c -o program

# Debug with LLDB
lldb ./program
(lldb) breakpoint set --name main
(lldb) run
(lldb) next
(lldb) print variable
(lldb) thread backtrace

# Memory check (Docker)
docker run --rm -v $(pwd):/code -w /code gios-prep valgrind --leak-check=full ./program

# Interactive Linux shell (Docker)
docker run -it --rm -v $(pwd):/code -w /code gios-prep bash
```

---

You're now ready to start the curriculum! Head to [01_c_foundations/README.md](01_c_foundations/README.md) to begin.
