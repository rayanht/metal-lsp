import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { execFile } from 'child_process';
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
} from 'vscode-languageclient/node';

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

  client = new LanguageClient(
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
