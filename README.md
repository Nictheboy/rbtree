# RBTree

This project, ```rbtree-nictheboy```, is a simple implementation of a Red-Black Tree in C++.
It is written in C++17 and uses CMake as its build system.

The only purpose of this project is for me to learn Red-Black Tree.

Use the following command to clone the repository with dependencies:
```bash
git clone https://github.com/Nictheboy/rbtree.git --recursive
```

## How to compile

You can use GCC, Clang or MSVC to build the project on Linux (or other UNIX-like OS) or Windows. Unit tests are not available for Windows.

### Linux

You can compile the project on UNIX-like OS with the following commands:
```bash
mkdir build
cd build
cmake ..
cmake --build . -j
ctest
```

### Windows

You can use Visual Studio to compile this repository. VS supports CMake very well, and we have tested to build this CMake project with MSVC.
