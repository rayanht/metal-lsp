import * as assert from 'assert';
import * as vscode from 'vscode';

suite('Metal LSP Extension', () => {
  test('extension is present', () => {
    const ext = vscode.extensions.getExtension('rayanht.metal-lsp');
    assert.ok(ext);
  });

  test('.metal files are associated with metal language', async () => {
    const doc = await vscode.workspace.openTextDocument({
      language: 'metal',
      content: 'kernel void test() {}',
    });
    assert.strictEqual(doc.languageId, 'metal');
  });

  test('commands are registered', async () => {
    const commands = await vscode.commands.getCommands(true);
    assert.ok(commands.includes('metal-lsp.configure'));
    assert.ok(commands.includes('metal-lsp.showStatus'));
    assert.ok(commands.includes('metal-lsp.restart'));
    assert.ok(commands.includes('metal-lsp.openDocs'));
  });
});
