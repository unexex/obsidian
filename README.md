# Obsidian
A light-weight, extremely fast Lua VM written in WASM, and JavaScript. It is a fork of Fengari, and is compatible with Fengari's API.

| Fengari | Obsidian |
|---------|----------|
| Written in JS | Written in TS & AS |
| JS & Webpack seperate | JS & Webpack included |
| Node support | Node, browser, bun.sh support |
| 100% JS | TS & AS that compiles to WASM |
| Import JS modules | Import JS & PY modules |
| Interpret, Bytecode | Interpret, Bytecode, Compile to WASM (experimental) |
| 
## Speed:
| Test | Fengari | Lua | Luau | LuaJIT | Obsidian | Obsidian Compiled |
|------|---------|-----|------|--------|----------|-------------------|
| Print | 88ms | 3ms | 

# HTML:
## Compile:
```bash
npm run build::wasm
```
## Usage:
```html
<!DOCTYPE html>
<html>
<body>
<script>
  fetch('index.wasm')
  .then(response => response.arrayBuffer())
  .then(bytes => WebAssembly.instantiate(bytes))
</script>

<script type="application/ob">
    print("Hello, world!")
</script>
</body>
</html>
```