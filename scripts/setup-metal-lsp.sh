#!/usr/bin/env bash
# Metal LSP — .clangd config generator for Claude Code plugin
# Generates/updates .clangd in the current project to configure clangd for .metal files.
# Uses bundled stubs by default; on macOS with Xcode, uses real SDK headers.

set -euo pipefail

MARKER_BEGIN="# --- metal-lsp:begin ---"
MARKER_END="# --- metal-lsp:end ---"

# Resolve plugin root (headers/ and stubs/ live here)
if [ -n "${CLAUDE_PLUGIN_ROOT:-}" ]; then
    PLUGIN_ROOT="$CLAUDE_PLUGIN_ROOT"
else
    # Fallback: script is in scripts/, plugin root is one level up
    PLUGIN_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
fi

COMPAT_HEADER="$PLUGIN_ROOT/headers/metal_compat.h"
STUBS_DIR="$PLUGIN_ROOT/stubs"

# Verify bundled files exist
if [ ! -f "$COMPAT_HEADER" ]; then
    echo "metal-lsp: error: compat header not found at $COMPAT_HEADER" >&2
    exit 1
fi
if [ ! -d "$STUBS_DIR" ]; then
    echo "metal-lsp: error: stubs directory not found at $STUBS_DIR" >&2
    exit 1
fi

# Detect macOS SDK headers (prefer real SDK over stubs)
HEADERS_PATH="$STUBS_DIR"
if [ "$(uname -s)" = "Darwin" ]; then
    SDK_PATH=""
    if [ -x /usr/bin/xcrun ]; then
        SDK_PATH=$(/usr/bin/xcrun --show-sdk-path 2>/dev/null || true)
    fi
    if [ -z "$SDK_PATH" ] && command -v xcrun &>/dev/null; then
        SDK_PATH=$(xcrun --show-sdk-path 2>/dev/null || true)
    fi
    if [ -n "$SDK_PATH" ] && [ -d "$SDK_PATH/usr/include/metal" ]; then
        HEADERS_PATH="$SDK_PATH/usr/include/metal"
    fi
fi

# Find .clangd in current directory
CLANGD_FILE=".clangd"

# Read existing content
CONTENT=""
if [ -f "$CLANGD_FILE" ]; then
    CONTENT=$(cat "$CLANGD_FILE")
fi

# Check if user has their own Metal config outside our markers
check_user_config() {
    local without_markers
    without_markers=$(echo "$CONTENT" | sed "/$MARKER_BEGIN/,/$MARKER_END/d")
    echo "$without_markers" | grep -q 'PathMatch:.*\\\.metal' && return 0
    return 1
}

if [ -n "$CONTENT" ] && check_user_config; then
    exit 0
fi

# Generate the Metal config block
BLOCK="${MARKER_BEGIN}
If:
  PathMatch: .*\\.metal

CompileFlags:
  Add:
    - -xc++
    - -std=c++17
    - -Wno-unknown-attributes
    - -Wno-reserved-user-defined-literal
    - -include
    - ${COMPAT_HEADER}
    - -isystem
    - ${HEADERS_PATH}
${MARKER_END}"

# Write block to a temp file for sed replacement
BLOCK_FILE=$(mktemp)
echo "$BLOCK" > "$BLOCK_FILE"

# Update or create
if echo "$CONTENT" | grep -q "$MARKER_BEGIN"; then
    # Replace existing marker block using sed
    # Delete lines between markers (inclusive), then insert new block at the right spot
    sed "/$MARKER_BEGIN/,/$MARKER_END/d" "$CLANGD_FILE" > "${CLANGD_FILE}.tmp"
    # If the file is now empty or only whitespace, just write the block
    if [ -z "$(cat "${CLANGD_FILE}.tmp" | tr -d '[:space:]')" ]; then
        mv "$BLOCK_FILE" "$CLANGD_FILE"
        rm -f "${CLANGD_FILE}.tmp"
    else
        # Append block to remaining content
        echo "" >> "${CLANGD_FILE}.tmp"
        cat "$BLOCK_FILE" >> "${CLANGD_FILE}.tmp"
        mv "${CLANGD_FILE}.tmp" "$CLANGD_FILE"
        rm -f "$BLOCK_FILE"
    fi
else
    # Append new block
    if [ -n "$CONTENT" ]; then
        # Ensure trailing newline before appending
        if [ -n "$(tail -c 1 "$CLANGD_FILE" 2>/dev/null)" ]; then
            echo "" >> "$CLANGD_FILE"
        fi
        echo "" >> "$CLANGD_FILE"
        cat "$BLOCK_FILE" >> "$CLANGD_FILE"
    else
        mv "$BLOCK_FILE" "$CLANGD_FILE"
    fi
    rm -f "$BLOCK_FILE"
fi
