import * as vscode from 'vscode';
import * as crypto from 'crypto';
import { execFile, ChildProcess } from 'child_process';
import * as fs from 'fs';
import * as path from 'path';
import { parseDiagnosticLine } from './diagnosticParser';

// Re-export for convenience
export { parseDiagnosticLine } from './diagnosticParser';

function severityToVscode(
  severity: 'error' | 'warning' | 'note'
): vscode.DiagnosticSeverity {
  switch (severity) {
    case 'error':
      return vscode.DiagnosticSeverity.Error;
    case 'warning':
      return vscode.DiagnosticSeverity.Warning;
    case 'note':
      return vscode.DiagnosticSeverity.Information;
  }
}

const COMPILE_TIMEOUT_MS = 10_000;

export class MetalCompiler implements vscode.Disposable {
  private diagnosticCollection: vscode.DiagnosticCollection;
  private outputChannel: vscode.OutputChannel;
  private disposables: vscode.Disposable[] = [];
  private inflight: ChildProcess | null = null;
  private enabled = false;

  constructor(outputChannel: vscode.OutputChannel) {
    this.outputChannel = outputChannel;
    this.diagnosticCollection =
      vscode.languages.createDiagnosticCollection('metal-compiler');

    // Watch for configuration changes
    this.disposables.push(
      vscode.workspace.onDidChangeConfiguration((e) => {
        if (e.affectsConfiguration('metal-lsp.compileDiagnostics')) {
          const setting = vscode.workspace
            .getConfiguration('metal-lsp')
            .get<string>('compileDiagnostics', 'off');
          if (setting === 'onSave') {
            this.enable();
          } else {
            this.disable();
          }
        }
      })
    );

    // Clear diagnostics when a document is closed
    this.disposables.push(
      vscode.workspace.onDidCloseTextDocument((doc) => {
        if (doc.languageId === 'metal') {
          this.diagnosticCollection.delete(doc.uri);
        }
      })
    );
  }

  /** Start watching for file saves. */
  enable(): void {
    if (this.enabled) {
      return;
    }
    // No-op on non-macOS
    if (process.platform !== 'darwin') {
      this.outputChannel.appendLine(
        'Compile diagnostics not available on this platform (macOS only).'
      );
      return;
    }
    this.enabled = true;
    this.disposables.push(
      vscode.workspace.onDidSaveTextDocument((doc) => {
        if (doc.languageId === 'metal') {
          this.compileFile(doc.uri).then(
            () => {},
            (err) => {
              this.outputChannel.appendLine(
                `Compile error: ${err instanceof Error ? err.message : String(err)}`
              );
            }
          );
        }
      })
    );
    this.outputChannel.appendLine('Compile-on-save diagnostics enabled.');
  }

  /** Stop watching. */
  disable(): void {
    if (!this.enabled) {
      return;
    }
    this.enabled = false;
    this.cancelInflight();
    this.diagnosticCollection.clear();
    this.outputChannel.appendLine('Compile-on-save diagnostics disabled.');
  }

  /** Compile a single file and return diagnostics. */
  async compileFile(uri: vscode.Uri): Promise<vscode.Diagnostic[]> {
    if (process.platform !== 'darwin') {
      return [];
    }

    const setting = vscode.workspace
      .getConfiguration('metal-lsp')
      .get<string>('compileDiagnostics', 'off');
    if (setting === 'off') {
      return [];
    }

    // Cancel any in-flight compilation (debounce)
    this.cancelInflight();

    const filePath = uri.fsPath;
    const hash = crypto.createHash('md5').update(filePath).digest('hex');
    const outputPath = path.join('/tmp', `metal-lsp-${hash}.air`);

    const diagnostics = await new Promise<vscode.Diagnostic[]>(
      (resolve, reject) => {
        const proc = execFile(
          'xcrun',
          ['metal', '-c', filePath, '-o', outputPath, `-std=${this.getMetalStdVersion()}`],
          { timeout: COMPILE_TIMEOUT_MS },
          (error, _stdout, stderr) => {
            this.inflight = null;
            if (error && error.killed) {
              // Process was killed (cancelled or timed out)
              resolve([]);
              return;
            }

            const result = this.parseStderr(stderr, filePath);
            resolve(result);
          }
        );
        this.inflight = proc;
      }
    );

    // Clean up .air output (best-effort)
    fs.promises.unlink(outputPath).catch(() => {});

    this.diagnosticCollection.set(uri, diagnostics);
    return diagnostics;
  }

  private parseStderr(
    stderr: string,
    targetFile: string
  ): vscode.Diagnostic[] {
    const diagnostics: vscode.Diagnostic[] = [];

    for (const line of stderr.split('\n')) {
      const parsed = parseDiagnosticLine(line);
      if (!parsed) {
        continue;
      }

      // Only include diagnostics for the target file
      const parsedPath = path.resolve(parsed.file);
      const resolvedTarget = path.resolve(targetFile);
      if (parsedPath !== resolvedTarget) {
        continue;
      }

      // Metal compiler lines/cols are 1-based, VSCode is 0-based
      const range = new vscode.Range(
        parsed.line - 1,
        parsed.col - 1,
        parsed.line - 1,
        parsed.col - 1
      );

      const diag = new vscode.Diagnostic(
        range,
        parsed.message,
        severityToVscode(parsed.severity)
      );
      diag.source = 'Metal Compiler';
      diagnostics.push(diag);
    }

    return diagnostics;
  }

  private getMetalStdVersion(): string {
    return vscode.workspace
      .getConfiguration('metal-lsp')
      .get<string>('metalStdVersion', 'metal3.0');
  }

  private cancelInflight(): void {
    if (this.inflight) {
      this.inflight.kill();
      this.inflight = null;
    }
  }

  dispose(): void {
    this.cancelInflight();
    this.diagnosticCollection.dispose();
    for (const d of this.disposables) {
      d.dispose();
    }
    this.disposables = [];
  }
}
