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
});
