import * as fs from 'fs';
import * as path from 'path';
import { SdkInfo } from './sdk';

export const MARKER_BEGIN = '# --- metal-lsp:begin ---';
export const MARKER_END = '# --- metal-lsp:end ---';

export function generateMetalBlock(sdkInfo: SdkInfo): string {
  const lines = [
    MARKER_BEGIN,
    'If:',
    '  PathMatch: .*\\.metal',
    '',
    'CompileFlags:',
    '  Add:',
    '    - -xc++',
    '    - -std=c++17',
    '    - -Wno-unknown-attributes',
    '    - -include',
    `    - ${sdkInfo.compatHeaderPath}`,
    '    - -isystem',
    `    - ${sdkInfo.headersPath}`,
    MARKER_END,
  ];
  return lines.join('\n');
}

/**
 * Checks if the .clangd content has a user-written PathMatch: .*\.metal block
 * that is NOT inside metal-lsp markers. If so, the user has taken ownership.
 */
export function hasUserMetalConfig(content: string): boolean {
  // Remove the marker block(s) first, then check for metal PathMatch
  const withoutMarkers = removeMarkerBlock(content);
  return /PathMatch:\s*\.\*\\\.metal/.test(withoutMarkers);
}

function removeMarkerBlock(content: string): string {
  const beginIdx = content.indexOf(MARKER_BEGIN);
  if (beginIdx === -1) {
    return content;
  }
  const endIdx = content.indexOf(MARKER_END, beginIdx);
  if (endIdx === -1) {
    return content;
  }
  const before = content.substring(0, beginIdx);
  const after = content.substring(endIdx + MARKER_END.length);
  // Recurse in case there are multiple marker blocks
  return removeMarkerBlock(before + after);
}

export async function updateClangdConfig(
  folderPath: string,
  sdkInfo: SdkInfo
): Promise<'created' | 'updated' | 'skipped'> {
  const clangdPath = path.join(folderPath, '.clangd');
  let content = '';

  try {
    content = await fs.promises.readFile(clangdPath, 'utf-8');
  } catch {
    // File doesn't exist — will create
  }

  if (hasUserMetalConfig(content)) {
    return 'skipped';
  }

  const newBlock = generateMetalBlock(sdkInfo);

  const beginIdx = content.indexOf(MARKER_BEGIN);
  const endIdx = content.indexOf(MARKER_END);

  if (beginIdx !== -1 && endIdx !== -1) {
    // Replace existing marker block
    const before = content.substring(0, beginIdx);
    const after = content.substring(endIdx + MARKER_END.length);
    const updated = before + newBlock + after;
    await fs.promises.writeFile(clangdPath, updated, 'utf-8');
    return 'updated';
  }

  // Append new block
  if (content.length > 0 && !content.endsWith('\n')) {
    content += '\n';
  }
  if (content.length > 0) {
    content += '\n';
  }
  content += newBlock + '\n';
  await fs.promises.writeFile(clangdPath, content, 'utf-8');
  return 'created';
}

export async function removeClangdConfig(
  folderPath: string
): Promise<'removed' | 'not-found'> {
  const clangdPath = path.join(folderPath, '.clangd');
  let content: string;

  try {
    content = await fs.promises.readFile(clangdPath, 'utf-8');
  } catch {
    return 'not-found';
  }

  const beginIdx = content.indexOf(MARKER_BEGIN);
  const endIdx = content.indexOf(MARKER_END);

  if (beginIdx === -1 || endIdx === -1) {
    return 'not-found';
  }

  // Remove everything between markers (inclusive), plus any trailing newline
  let before = content.substring(0, beginIdx);
  let after = content.substring(endIdx + MARKER_END.length);

  // Clean up extra blank lines left behind
  if (after.startsWith('\n')) {
    after = after.substring(1);
  }

  const result = (before + after).trim();

  if (result.length === 0) {
    await fs.promises.unlink(clangdPath);
  } else {
    await fs.promises.writeFile(clangdPath, result + '\n', 'utf-8');
  }

  return 'removed';
}
