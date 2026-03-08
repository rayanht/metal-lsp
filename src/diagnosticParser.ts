/**
 * Pure parsing logic for Metal compiler diagnostic output.
 * Kept separate from compiler.ts so it can be unit tested without vscode.
 */

export interface ParsedDiagnostic {
  file: string;
  line: number;
  col: number;
  severity: 'error' | 'warning' | 'note';
  message: string;
}

/**
 * Parse a single line of Metal compiler output into a diagnostic descriptor.
 * Metal compiler format (same as clang): <file>:<line>:<col>: error|warning|note: <message>
 */
export function parseDiagnosticLine(line: string): ParsedDiagnostic | null {
  const match = line.match(/^(.+?):(\d+):(\d+):\s+(error|warning|note):\s+(.+)$/);
  if (!match) {
    return null;
  }
  return {
    file: match[1],
    line: parseInt(match[2], 10),
    col: parseInt(match[3], 10),
    severity: match[4] as 'error' | 'warning' | 'note',
    message: match[5],
  };
}
