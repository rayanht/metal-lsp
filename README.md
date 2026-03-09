# Metal LSP

Full language server features for Metal Shading Language in VSCode and Cursor.

Autocomplete, inline diagnostics, hover documentation, and go-to-definition for `.metal` files. Powered by clangd with Metal-aware headers.

## Features

| Feature | macOS | Linux/Windows |
|---------|-------|---------------|
| Autocomplete (stdlib + user code) | Full (real SDK headers) | Full (stubs) |
| Inline diagnostics | Full | Best-effort (C++ mode) |
| Hover documentation | From MSL spec | From MSL spec |
| Go-to-definition | Yes | Yes (into stubs) |
| Syntax highlighting | TextMate + semantic | TextMate + semantic |
| Compile-on-save diagnostics | Yes (opt-in, via `xcrun metal`) | No |

## Requirements

- **clangd** — install via the [clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd), Homebrew (`brew install llvm`), or your system package manager
- **macOS** (optional): Xcode or Command Line Tools for real Metal SDK headers. Without them, the extension falls back to bundled stubs.

## Installation

Install from the [VS Code Marketplace](https://marketplace.visualstudio.com/items?itemName=rayanhtt.metal-lsp). The extension activates automatically when your workspace contains `.metal` files.

Also available on [Open VSX](https://open-vsx.org/) for VSCodium and other editors.

## How It Works

The extension configures clangd to understand Metal by:

1. Registering `.metal` files as the "Metal" language
2. Running its own clangd language client scoped to Metal files
3. Generating a `.clangd` config that tells clangd to parse `.metal` as C++ with Metal-compatible flags
4. Providing a compat header that maps Metal keywords (`device`, `constant`, `kernel`, etc.) to C++ type qualifiers
5. Bundling stub headers so `#include <metal_stdlib>` resolves — on macOS with Xcode, real SDK headers are used instead

## Settings

| Setting | Default | Description |
|---------|---------|-------------|
| `metal-lsp.compileDiagnostics` | `"off"` | `"off"` or `"onSave"` — run `xcrun metal` for real compiler diagnostics (macOS only) |

## Commands

- **Metal LSP: Configure** — regenerate `.clangd` configuration
- **Metal LSP: Show Status** — show diagnostic info (header source, SDK path, configured folders)
- **Metal LSP: Restart Language Server** — restart the clangd language client
- **Metal LSP: Open Documentation** — open Apple's MSL spec

## License

MIT
