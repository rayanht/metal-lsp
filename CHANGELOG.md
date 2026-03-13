# Changelog

All notable changes to the Metal LSP extension will be documented in this file.

## [1.4.0] — 2026-03-13

### Multi-File Shader Projects

The workspace root is now automatically added as an include path (`-I`), so `#include "shared/types.h"` resolves out of the box when `shared/` is at your project root. No configuration needed.

### Additional Extension Settings

- `metal-lsp.includePaths` — extra include directories (added as `-isystem` paths to both clangd and `xcrun metal`)
- `metal-lsp.compileFlags` — extra compile flags passed to both clangd and the Metal compiler

Changing either setting automatically regenerates the `.clangd` config and restarts clangd.

### Housekeeping

- Claude Code plugin files (`.claude-plugin/`, `hooks/`, `scripts/`, `.lsp.json`) excluded from VS Code extension package
- Show Status command now displays configured include paths and compile flags

## [1.3.0] — 2026-03-10

### Code Snippets

7 tab-completable templates for common Metal patterns:

- `kernel` — compute kernel with buffer binding and thread position
- `vertex` — vertex function with output struct and `[[position]]`
- `fragment` — fragment function with `[[stage_in]]`
- `struct-vertex` — vertex input struct with `[[attribute(n)]]` bindings
- `struct-varying` — vertex-to-fragment interpolation struct
- `threadgroup-reduce` — full threadgroup parallel reduction pattern
- `simdgroup-matmul` — simdgroup 8×8 matrix multiply-accumulate

### Configurable Metal Standard Version

New `metal-lsp.metalStdVersion` setting — choose `metal1.0` through `metal4.0` for compile-on-save diagnostics (default: `metal3.0`). Previously hardcoded to `metal3.0`.

### Language Configuration

- Auto-indent after `{`, outdent on `}`
- Folding markers: `#pragma mark`, `// region` / `// endregion`
- Block comment continuation: `* ` auto-inserted on Enter inside `/** */`
- Improved word selection for identifiers

### Claude Code Plugin

Metal LSP is now available as a Claude Code plugin:

```bash
/plugin marketplace add rayanht/metal-lsp
/plugin install metal-lsp@rayanht-metal-lsp
```

Provides clangd-powered diagnostics, hover docs, and go-to-definition for `.metal` files directly in Claude Code CLI sessions.

## [1.2.0] — 2026-03-10

### Extended Math Functions

- **Pi-multiplied trig** — `sinpi`, `cospi`, `tanpi`, `acospi`, `asinpi`, `atanpi`, `atan2pi`
- **Additional exp/log** — `exp10`, `expm1`, `log1p`
- **Power/root** — `cbrt`, `hypot`, `pown`, `rootn`
- **Remainder/rounding** — `remainder`, `rint`, `copysign`, `fdim`
- **Gamma/error** — `lgamma`, `tgamma`, `erf`, `erfc`
- All with float, half, and vector overloads

### Data Packing/Unpacking

Full suite of normalized integer packing functions:

- `pack_float_to_snorm2x16` / `unpack_snorm2x16_to_float` (and unorm variants)
- `pack_float_to_snorm4x8` / `unpack_snorm4x8_to_float` (and unorm variants)
- Per-component: `pack_float_to_snorm_short`, `pack_float_to_unorm_ushort`, `pack_float_to_snorm_char`, `pack_float_to_unorm_uchar` (and unpacking counterparts)
- Half-precision variants of all packing functions

### Render Target Functions

- `render_target_read<T>` / `render_target_write<T>` — tile-based deferred rendering
- `get_render_target_sample_count` — MSAA sample count query
- `get_fragment_size` — variable rasterization rate support
- `get_num_color_attachments`, `is_front_face`

### Volatile Compound Assignment

- Added volatile-qualified `+=`, `-=`, `*=`, `/=` operators on `vec2`, `vec3`, `vec4` — fixes `output[tid] += ...` through device pointers

### TextMate Grammar Expansion

Major expansion of syntax highlighting coverage:

- **Built-in functions** — 100+ Metal stdlib functions now highlighted (math, geometric, SIMD, quad, atomic, derivative, interpolation, packing, render target)
- **All texture types** — `texture1d`, `texture1d_array`, `texture2d_ms`, `texture2d_ms_array`, `texture3d`, `texturecube`, `texturecube_array`, depth variants, `texture_buffer`
- **Packed vector types** — `packed_float3`, `packed_half4`, etc.
- **Ray tracing types** — `ray`, `intersector`, `intersection_result`, acceleration structures
- **Mesh/imageblock types** — `mesh_grid_properties`, `imageblock`, `imageblock_slice`
- **SIMD types** — `simdgroup_matrix`, `simdgroup_event`
- **Enum types** — `memory_order`, `mem_flags`, `access`, sampler enums

## [1.1.0] — 2026-03-09

### Stub Expansion

Major expansion of Metal stdlib stub coverage, validated against a real-world 3700-line Gaussian splatting shader and a 26-kernel test library.

- **Derivative functions** — `dfdx`, `dfdy`, `fwidth` with coarse/fine variants
- **Interpolation functions** — `interpolate_at_center`, `interpolate_at_centroid`, `interpolate_at_sample`, `interpolate_at_offset`
- **Ray tracing types** — `ray`, `intersection_result`, `intersector<Tags...>`, `primitive_acceleration_structure`, `instance_acceleration_structure`, `intersection_function_table`
- **Mesh shader types** — `mesh_grid_properties`, `topology` enum, `mesh<V, P, MaxVerts, MaxPrims, Topo>`
- **Imageblock types** — `imageblock<T>`, `imageblock_slice<T>` for tile-based deferred rendering
- **Visible function tables** — `visible_function_table<Fn>` with callable `operator[]`
- **Matrix improvements** — `inverse()` for all float/half square matrices, volatile copy/assign for device pointer access
- **Array type** — `array<T, N>` for argument buffers
- **SIMD additions** — `simdgroup_event` (async copy), `simd_vote::vote_t`, `simd_ballot` returning `vote_t`
- **Texture additions** — `texture2d_ms_array`
- **Missing math** — `powr()` (positive-base power)

### Volatile Fixes

Systematic fix for the `device → volatile` address space mapping throughout all stubs:

- Volatile copy constructors on `vec2`, `vec3`, `vec4`, and `matrix`
- `const volatile`-qualified arithmetic operators on all vector types
- Volatile conversion operators on all packed vector types
- Volatile load/store overloads for `simdgroup_load`/`simdgroup_store`
- Volatile assignment operator on `matrix`

### Other Improvements

- SFINAE-constrained scalar-vector operators — `double + float3` now works correctly (common in shader code)
- Fixed Mocha test runner UI mismatch (`tdd` vs `bdd`)
- Fixed publisher ID in integration test
- Test kernel library: 26 kernels across compute, graphics, and advanced Metal patterns
- Vendored real-world Gaussian splatting shader for validation

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
