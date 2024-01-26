# Obsidian
Ultra-fast Lua 5.3 for the web.

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