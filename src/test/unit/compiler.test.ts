import * as assert from 'assert';
import { parseDiagnosticLine } from '../../diagnosticParser';

suite('Compiler Diagnostics Parser', () => {
  test('parses error line', () => {
    const result = parseDiagnosticLine(
      '/path/to/shader.metal:10:5: error: use of undeclared identifier'
    );
    assert.ok(result);
    assert.strictEqual(result.file, '/path/to/shader.metal');
    assert.strictEqual(result.line, 10);
    assert.strictEqual(result.col, 5);
    assert.strictEqual(result.severity, 'error');
    assert.strictEqual(result.message, 'use of undeclared identifier');
  });

  test('parses warning line', () => {
    const result = parseDiagnosticLine(
      '/path/to/shader.metal:42:12: warning: unused variable'
    );
    assert.ok(result);
    assert.strictEqual(result.severity, 'warning');
    assert.strictEqual(result.line, 42);
    assert.strictEqual(result.col, 12);
    assert.strictEqual(result.message, 'unused variable');
  });

  test('parses note line', () => {
    const result = parseDiagnosticLine(
      '/path/to/shader.metal:10:5: note: previous declaration is here'
    );
    assert.ok(result);
    assert.strictEqual(result.severity, 'note');
    assert.strictEqual(result.message, 'previous declaration is here');
  });

  test('returns null for non-diagnostic line', () => {
    assert.strictEqual(parseDiagnosticLine('1 error generated.'), null);
    assert.strictEqual(parseDiagnosticLine(''), null);
    assert.strictEqual(
      parseDiagnosticLine('In file included from /path/to/file.h:1:'),
      null
    );
  });

  test('handles paths with spaces', () => {
    const result = parseDiagnosticLine(
      '/path/to/my shader.metal:5:1: error: expected expression'
    );
    assert.ok(result);
    assert.strictEqual(result.file, '/path/to/my shader.metal');
    assert.strictEqual(result.line, 5);
    assert.strictEqual(result.col, 1);
  });

  test('handles Windows-style paths', () => {
    const result = parseDiagnosticLine(
      'C:\\Users\\dev\\shader.metal:5:1: error: expected expression'
    );
    assert.ok(result);
    assert.strictEqual(result.file, 'C:\\Users\\dev\\shader.metal');
    assert.strictEqual(result.line, 5);
  });

  test('handles messages with colons', () => {
    const result = parseDiagnosticLine(
      '/path/shader.metal:1:1: error: expected type: found int'
    );
    assert.ok(result);
    assert.strictEqual(result.message, 'expected type: found int');
  });
});
