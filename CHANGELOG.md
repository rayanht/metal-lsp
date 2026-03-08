# Changelog

All notable changes to the Metal LSP extension will be documented in this file.

## [1.0.0] — 2026-03-08

Initial release.

### Features

- **Language registration** — `.metal` files recognized as "Metal" with TextMate syntax highlighting
- **Autocomplete** — full Metal stdlib completion via clangd (real SDK headers on macOS, bundled stubs elsewhere)
- **Inline diagnostics** — real-time error/warning squiggles from clangd
- **Hover documentation** — doc comments from MSL spec on stdlib types and functions
- **Go-to-definition** — jump to declarations in SDK headers or stubs
- **Compile-on-save** — opt-in real Metal compiler diagnostics via `xcrun metal` (macOS only)
- **Automatic configuration** — generates `.clangd` config with marker-based injection, respects user edits
- **Multi-root workspaces** — configures each folder independently
- **clangd auto-discovery** — finds clangd from settings, vscode-clangd extension, PATH, or common locations

### Stub Coverage

Bundled stubs (~2200 lines) covering vectors, matrices, textures, math, atomics, SIMD, samplers, barriers, and common/geometric/relational/integer functions.
