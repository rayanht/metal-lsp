# Changelog

All notable changes to the Metal LSP extension will be documented in this file.

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
