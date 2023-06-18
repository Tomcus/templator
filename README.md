# Templator

"Fast" text templating library written in C.

## Building

### Requirements

* `cmake`
* `c compiler`
* `conan`

### Instructions

```sh
# Initialize conan profile - needs to be done once
conan profile detect --force
# Install conan packages to build directory
conan install . --output=folder=build --build=missing

# Configure project
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
# Build the project
cd ..
cmake --build build -j
```

## Why C?

To improve C knowledge, read "Fluent C" book, and check how to create wrappers for other languages.

## TODO list

Features:

* [ ] If statements
* [ ] Loops
* [ ] Template templates - inheritance with recursive insertion.
* [ ] Functions
  * [ ] `hex`, `oct`, `bin`
  * [ ] `lower_case`, `upper_case`
* [ ] User defined function

Wrappers:

* [ ] C++
* [ ] Rust
* [ ] Go
* [ ] Lua
* [ ] Python
