# Obsidian
Lua to WASM compiler with JS syntax and additional features like:
- [x] defers
- [x] freezable tables
- [x] enums
- [x] Easy for loops, does not require pairs
- [x] JS libraries supported
- [x] Pool allocator (toggleable)

## Example:
```js
var a = 1
var b = 2
var c = a + b
```
```bash
ob ex.ob -o file.wasm
```

## Todo:
- Windows installer

## dependencies:
- emscripten
- gcc/clang & make for compiling the compiler
- clang for compiling files
- WASI SDK for compiling files