import * as assert from 'assert';
import {
  generateMetalBlock,
  hasUserMetalConfig,
  MARKER_BEGIN,
  MARKER_END,
} from '../../config';
import { SdkInfo } from '../../sdk';

const stubSdkInfo: SdkInfo = {
  useRealHeaders: false,
  headersPath: '/fake/extension/stubs',
  compatHeaderPath: '/fake/extension/headers/metal_compat.h',
  sdkPath: null,
};

suite('Config Generator', () => {
  test('generates valid YAML block with markers', () => {
    const block = generateMetalBlock(stubSdkInfo);
    assert.ok(block.startsWith(MARKER_BEGIN));
    assert.ok(block.endsWith(MARKER_END));
    assert.ok(block.includes('PathMatch: .*\\.metal'));
    assert.ok(block.includes('-xc++'));
    assert.ok(block.includes('-std=c++17'));
    assert.ok(block.includes(stubSdkInfo.compatHeaderPath));
    assert.ok(block.includes(stubSdkInfo.headersPath));
  });

  test('includes -include and -isystem flags in correct order', () => {
    const block = generateMetalBlock(stubSdkInfo);
    const includeIdx = block.indexOf('-include');
    const isystemIdx = block.indexOf('-isystem');
    assert.ok(includeIdx < isystemIdx, '-include should come before -isystem');
  });

  test('detects user metal config outside markers', () => {
    const content = [
      'If:',
      '  PathMatch: .*\\.metal',
      '',
      'CompileFlags:',
      '  Add:',
      '    - -xc++',
    ].join('\n');

    assert.strictEqual(hasUserMetalConfig(content), true);
  });

  test('does not false-positive on marker block', () => {
    const block = generateMetalBlock(stubSdkInfo);
    assert.strictEqual(hasUserMetalConfig(block), false);
  });

  test('does not false-positive when marker block and other content', () => {
    const block = generateMetalBlock(stubSdkInfo);
    const content = [
      'CompileFlags:',
      '  Add:',
      '    - -Wall',
      '',
      block,
    ].join('\n');

    assert.strictEqual(hasUserMetalConfig(content), false);
  });

  test('detects user config even when marker block also present', () => {
    const block = generateMetalBlock(stubSdkInfo);
    const content = [
      'If:',
      '  PathMatch: .*\\.metal',
      '',
      'CompileFlags:',
      '  Add:',
      '    - -xc++',
      '',
      block,
    ].join('\n');

    assert.strictEqual(hasUserMetalConfig(content), true);
  });

  test('includes workspace root as -I when provided', () => {
    const block = generateMetalBlock(stubSdkInfo, {
      workspaceRoot: '/my/project',
    });
    assert.ok(block.includes('- -I'));
    assert.ok(block.includes('- /my/project'));
  });

  test('defaults workspace root to . when not provided', () => {
    const block = generateMetalBlock(stubSdkInfo);
    assert.ok(block.includes('- -I'));
    assert.ok(block.includes('- .'));
  });

  test('includes extra includePaths as -isystem', () => {
    const block = generateMetalBlock(stubSdkInfo, {
      workspaceRoot: '/proj',
      includePaths: ['/extra/headers', '/vendor/lib'],
    });
    // Should have the SDK -isystem plus two extra -isystem entries
    const isystemMatches = block.match(/- -isystem/g);
    assert.strictEqual(isystemMatches?.length, 3); // 1 SDK + 2 extra
    assert.ok(block.includes('- /extra/headers'));
    assert.ok(block.includes('- /vendor/lib'));
  });

  test('includes extra compileFlags', () => {
    const block = generateMetalBlock(stubSdkInfo, {
      workspaceRoot: '/proj',
      compileFlags: ['-DDEBUG=1', '-Werror'],
    });
    assert.ok(block.includes('- -DDEBUG=1'));
    assert.ok(block.includes('- -Werror'));
  });

  test('-I workspace root comes after -isystem SDK headers', () => {
    const block = generateMetalBlock(stubSdkInfo, {
      workspaceRoot: '/proj',
    });
    const isystemIdx = block.indexOf(`- ${stubSdkInfo.headersPath}`);
    const dashIIdx = block.indexOf('- -I');
    assert.ok(isystemIdx < dashIIdx, '-isystem SDK should come before -I workspace root');
  });
});
