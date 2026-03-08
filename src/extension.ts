import * as vscode from 'vscode';
import { detectSdk, SdkInfo } from './sdk';
import { updateClangdConfig, MARKER_BEGIN } from './config';
import { startClient, stopClient, restartClient } from './client';
import * as fs from 'fs';
import * as path from 'path';

let outputChannel: vscode.OutputChannel;
let cachedSdkInfo: SdkInfo | undefined;

// Track which workspace folders have been configured
const configuredFolders = new Set<string>();

function log(msg: string): void {
  outputChannel.appendLine(`[${new Date().toISOString()}] ${msg}`);
}

/**
 * Scan a workspace folder for .metal files and configure if found.
 */
async function configureFolder(
  folder: vscode.WorkspaceFolder,
  sdkInfo: SdkInfo
): Promise<void> {
  const pattern = new vscode.RelativePattern(folder, '**/*.metal');
  const metalFiles = await vscode.workspace.findFiles(pattern, null, 1);

  if (metalFiles.length === 0) {
    return;
  }

  const folderPath = folder.uri.fsPath;
  const result = await updateClangdConfig(folderPath, sdkInfo);
  configuredFolders.add(folderPath);

  switch (result) {
    case 'created':
      log(`Created .clangd config in ${folderPath}`);
      break;
    case 'updated':
      log(`Updated .clangd config in ${folderPath}`);
      break;
    case 'skipped':
      log(`Skipped ${folderPath} — user has own Metal config`);
      break;
  }
}

/**
 * Configure all current workspace folders.
 */
async function configureAllFolders(sdkInfo: SdkInfo): Promise<void> {
  const folders = vscode.workspace.workspaceFolders;
  if (!folders) {
    return;
  }

  for (const folder of folders) {
    try {
      await configureFolder(folder, sdkInfo);
    } catch (err) {
      log(`Error configuring ${folder.uri.fsPath}: ${err}`);
    }
  }
}

/**
 * Watch .clangd files in configured folders for external changes.
 */
function watchClangdFiles(context: vscode.ExtensionContext): void {
  const watcher = vscode.workspace.createFileSystemWatcher('**/.clangd');

  watcher.onDidChange(async (uri) => {
    const folderPath = path.dirname(uri.fsPath);
    if (!configuredFolders.has(folderPath)) {
      return;
    }

    try {
      const content = await fs.promises.readFile(uri.fsPath, 'utf-8');
      if (!content.includes(MARKER_BEGIN)) {
        configuredFolders.delete(folderPath);
        vscode.window.showInformationMessage(
          'Metal LSP config was removed from .clangd \u2014 Metal language features may be limited. ' +
          'Run "Metal LSP: Configure" to re-enable.'
        );
        log(`Markers removed from .clangd in ${folderPath} — not re-adding`);
      }
    } catch {
      // File read failed, ignore
    }
  });

  watcher.onDidDelete((uri) => {
    const folderPath = path.dirname(uri.fsPath);
    if (configuredFolders.has(folderPath)) {
      configuredFolders.delete(folderPath);
      vscode.window.showInformationMessage(
        'Metal LSP config was removed from .clangd \u2014 Metal language features may be limited. ' +
        'Run "Metal LSP: Configure" to re-enable.'
      );
      log(`.clangd deleted in ${folderPath}`);
    }
  });

  context.subscriptions.push(watcher);
}

/**
 * Register all extension commands.
 */
function registerCommands(context: vscode.ExtensionContext): void {
  context.subscriptions.push(
    vscode.commands.registerCommand('metal-lsp.configure', async () => {
      if (!cachedSdkInfo) {
        vscode.window.showErrorMessage('Metal LSP: SDK info not available');
        return;
      }
      configuredFolders.clear();
      await configureAllFolders(cachedSdkInfo);
      await restartClient(outputChannel);
      vscode.window.showInformationMessage('Metal LSP: Configuration updated');
    })
  );

  context.subscriptions.push(
    vscode.commands.registerCommand('metal-lsp.showStatus', () => {
      outputChannel.show(true);
      outputChannel.appendLine('--- Metal LSP Status ---');
      if (cachedSdkInfo) {
        outputChannel.appendLine(`Header source: ${cachedSdkInfo.useRealHeaders ? 'SDK' : 'Stubs'}`);
        outputChannel.appendLine(`SDK path: ${cachedSdkInfo.sdkPath ?? 'N/A'}`);
        outputChannel.appendLine(`Headers path: ${cachedSdkInfo.headersPath}`);
        outputChannel.appendLine(`Compat header: ${cachedSdkInfo.compatHeaderPath}`);
      } else {
        outputChannel.appendLine('SDK info: not yet detected');
      }
      outputChannel.appendLine(`Configured folders: ${configuredFolders.size > 0 ? [...configuredFolders].join(', ') : 'none'}`);

      const compileDiag = vscode.workspace.getConfiguration('metal-lsp').get<string>('compileDiagnostics', 'off');
      outputChannel.appendLine(`Compile diagnostics: ${compileDiag}`);
      outputChannel.appendLine('------------------------');
    })
  );

  context.subscriptions.push(
    vscode.commands.registerCommand('metal-lsp.restart', async () => {
      try {
        await restartClient(outputChannel);
        vscode.window.showInformationMessage('Metal LSP: Language server restarted');
      } catch (err) {
        vscode.window.showErrorMessage(`Metal LSP: Failed to restart — ${err}`);
      }
    })
  );

  context.subscriptions.push(
    vscode.commands.registerCommand('metal-lsp.openDocs', () => {
      vscode.env.openExternal(
        vscode.Uri.parse('https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf')
      );
    })
  );
}

export async function activate(context: vscode.ExtensionContext): Promise<void> {
  outputChannel = vscode.window.createOutputChannel('Metal LSP');
  context.subscriptions.push(outputChannel);
  log('Activating Metal LSP extension');

  // 1. Detect SDK
  try {
    cachedSdkInfo = await detectSdk(context.extensionPath);
    log(`SDK detection: ${cachedSdkInfo.useRealHeaders ? 'real SDK' : 'stubs'} — headers at ${cachedSdkInfo.headersPath}`);
  } catch (err) {
    log(`SDK detection failed: ${err}`);
    return;
  }

  // 2. Configure all workspace folders
  await configureAllFolders(cachedSdkInfo);

  // 3. Start our own clangd language client for .metal files
  try {
    await startClient(outputChannel);
  } catch (err) {
    log(`Failed to start clangd client: ${err}`);
  }

  // 4. Watch for new .metal file creation
  const metalWatcher = vscode.workspace.createFileSystemWatcher('**/*.metal');
  metalWatcher.onDidCreate(async (uri) => {
    const folder = vscode.workspace.getWorkspaceFolder(uri);
    if (!folder) {
      return;
    }
    if (configuredFolders.has(folder.uri.fsPath)) {
      return;
    }
    try {
      await configureFolder(folder, cachedSdkInfo!);
    } catch (err) {
      log(`Error configuring folder for new .metal file: ${err}`);
    }
  });
  context.subscriptions.push(metalWatcher);

  // 5. Watch for workspace folder additions
  context.subscriptions.push(
    vscode.workspace.onDidChangeWorkspaceFolders(async (event) => {
      for (const folder of event.added) {
        try {
          await configureFolder(folder, cachedSdkInfo!);
        } catch (err) {
          log(`Error configuring added folder ${folder.uri.fsPath}: ${err}`);
        }
      }
    })
  );

  // 6. Register commands
  registerCommands(context);

  // 7. Watch .clangd files for external modifications
  watchClangdFiles(context);

  log('Metal LSP extension activated');
}

export async function deactivate(): Promise<void> {
  await stopClient();
}
