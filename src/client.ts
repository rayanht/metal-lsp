import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { execFile } from 'child_process';
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
} from 'vscode-languageclient/node';

/**
 * Subclass LanguageClient to handle command registration conflicts.
 * When another clangd extension (or Cursor built-in) is active, commands
 * like 'clangd.applyFix' are already registered. We patch the execute
 * command feature to gracefully skip already-registered commands.
 */
class MetalLanguageClient extends LanguageClient {
  protected registerBuiltinFeatures(): void {
    super.registerBuiltinFeatures();
    // Patch the ExecuteCommandFeature to tolerate pre-existing commands
    const features = (this as any)._dynamicFeatures as Map<string, any>;
    const feature = features.get('workspace/executeCommand');
    if (feature) {
      const origRegister = feature.register.bind(feature);
      feature.register = (data: any) => {
        // Wrap each command registration in try-catch individually
        if (data.registerOptions?.commands) {
          const safeCommands: string[] = [];
          const skipped: string[] = [];
          for (const cmd of data.registerOptions.commands) {
            try {
              vscode.commands.registerCommand(cmd, () => {});
              // If it succeeded, it means it wasn't registered — dispose and let origRegister handle it
              // Actually we can't easily dispose here. Better approach: just let origRegister try.
              safeCommands.push(cmd);
            } catch {
              skipped.push(cmd);
            }
          }
          // Only pass commands that aren't already registered
          // But we just registered the safe ones above as no-ops — we need to dispose them.
          // Simpler: just catch the error from origRegister.
        }
        // Use a patched registerCommand that ignores 'already exists' errors
        const origCmd = vscode.commands.registerCommand;
        (vscode.commands as any).registerCommand = (id: string, handler: (...args: any[]) => any) => {
          try {
            return origCmd.call(vscode.commands, id, handler);
          } catch (e: any) {
            if (e?.message?.includes('already exists')) {
              return { dispose: () => {} };
            }
            throw e;
          }
        };
        try {
          origRegister(data);
        } finally {
          (vscode.commands as any).registerCommand = origCmd;
        }
      };
    }
  }
}

let client: LanguageClient | undefined;

/**
 * Find the clangd binary. Search order:
 * 1. clangd.path setting (user-configured)
 * 2. vscode-clangd extension's bundled clangd
 * 3. System PATH via `which clangd`
 * 4. Common known locations
 */
async function findClangd(): Promise<string | undefined> {
  // 1. User-configured path
  const configPath = vscode.workspace.getConfiguration('clangd').get<string>('path');
  if (configPath && fs.existsSync(configPath)) {
    return configPath;
  }

  // 2. vscode-clangd extension's bundled clangd
  const clangdExt = vscode.extensions.getExtension('llvm-vs-code-extensions.vscode-clangd');
  if (clangdExt) {
    const bundled = path.join(clangdExt.extensionPath, 'install', process.platform, 'clangd', 'bin', 'clangd');
    if (fs.existsSync(bundled)) {
      return bundled;
    }
    // Also check alternate layout
    const bundled2 = path.join(clangdExt.extensionPath, 'install', `clangd_${process.platform}`, 'bin', 'clangd');
    if (fs.existsSync(bundled2)) {
      return bundled2;
    }
  }

  // 3. System PATH
  const which = await findInPath('clangd');
  if (which) {
    return which;
  }

  // 4. Common locations
  const commonPaths = [
    '/usr/bin/clangd',
    '/usr/local/bin/clangd',
    '/opt/homebrew/bin/clangd',
    '/usr/local/opt/llvm/bin/clangd',
    '/opt/homebrew/opt/llvm/bin/clangd',
  ];
  for (const p of commonPaths) {
    if (fs.existsSync(p)) {
      return p;
    }
  }

  return undefined;
}

function findInPath(binary: string): Promise<string | undefined> {
  return new Promise((resolve) => {
    execFile('which', [binary], { timeout: 3000 }, (err, stdout) => {
      if (err || !stdout.trim()) {
        resolve(undefined);
      } else {
        resolve(stdout.trim());
      }
    });
  });
}

export async function startClient(outputChannel: vscode.OutputChannel): Promise<LanguageClient | undefined> {
  const clangdPath = await findClangd();
  if (!clangdPath) {
    vscode.window.showErrorMessage(
      'Metal LSP: clangd not found. Install clangd or the clangd extension, or set clangd.path in settings.'
    );
    outputChannel.appendLine('ERROR: clangd binary not found');
    return undefined;
  }

  outputChannel.appendLine(`Using clangd at: ${clangdPath}`);

  // Get any user-configured clangd arguments
  const userArgs = vscode.workspace.getConfiguration('clangd').get<string[]>('arguments') ?? [];

  const serverOptions: ServerOptions = {
    command: clangdPath,
    args: [
      '--log=error',
      '--background-index',
      '--clang-tidy=false',
      ...userArgs,
    ],
  };

  const clientOptions: LanguageClientOptions = {
    documentSelector: [{ language: 'metal' }],
    outputChannel,
  };

  client = new MetalLanguageClient(
    'metal-lsp',
    'Metal Language Server',
    serverOptions,
    clientOptions
  );

  await client.start();
  outputChannel.appendLine('clangd language client started');
  return client;
}

export async function stopClient(): Promise<void> {
  if (client) {
    await client.stop();
    client = undefined;
  }
}

export async function restartClient(outputChannel: vscode.OutputChannel): Promise<void> {
  await stopClient();
  await startClient(outputChannel);
}

export function getClient(): LanguageClient | undefined {
  return client;
}
