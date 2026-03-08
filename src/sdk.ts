import { execFile } from 'child_process';
import { existsSync } from 'fs';
import * as path from 'path';

export interface SdkInfo {
  /** Whether real SDK headers are available */
  useRealHeaders: boolean;
  /** Path to Metal headers directory (-isystem target) */
  headersPath: string;
  /** Path to the compat header (-include target) */
  compatHeaderPath: string;
  /** SDK path from xcrun (null if stubs) */
  sdkPath: string | null;
}

function runXcrun(): Promise<string> {
  // Use absolute path — Cursor/VSCode subprocess may not inherit shell PATH
  const xcrunPaths = ['/usr/bin/xcrun', 'xcrun'];
  return new Promise((resolve, reject) => {
    function tryNext(i: number): void {
      if (i >= xcrunPaths.length) {
        reject(new Error('xcrun not found'));
        return;
      }
      execFile(xcrunPaths[i], ['--show-sdk-path'], { timeout: 3000 }, (error, stdout) => {
        if (error) {
          tryNext(i + 1);
          return;
        }
        resolve(stdout.trim());
      });
    }
    tryNext(0);
  });
}

export async function detectSdk(extensionPath: string): Promise<SdkInfo> {
  const compatHeaderPath = path.join(extensionPath, 'headers', 'metal_compat.h');
  const stubsPath = path.join(extensionPath, 'stubs');

  // Non-macOS: go straight to stubs
  if (process.platform !== 'darwin') {
    return {
      useRealHeaders: false,
      headersPath: stubsPath,
      compatHeaderPath,
      sdkPath: null,
    };
  }

  try {
    const sdkPath = await runXcrun();
    const metalIncludeDir = path.join(sdkPath, 'usr', 'include', 'metal');

    if (existsSync(metalIncludeDir)) {
      return {
        useRealHeaders: true,
        headersPath: metalIncludeDir,
        compatHeaderPath,
        sdkPath,
      };
    }
  } catch {
    // xcrun failed or timed out — fall through to stubs
  }

  return {
    useRealHeaders: false,
    headersPath: stubsPath,
    compatHeaderPath,
    sdkPath: null,
  };
}
