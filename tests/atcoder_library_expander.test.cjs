const { test } = require("node:test");
const assert = require("node:assert/strict");
const fs = require("node:fs/promises");
const path = require("node:path");
const { Expander, editorAccess, install } = require("../tools/atcoder-library-expander.user.js");

function fixture(files, settings = {}) {
    const calls = [];
    const expander = new Expander(settings, async (url) => {
        calls.push(url);
        const name = url.split("feature/icpc-extra-libraries/")[1];
        return files[name] ?? null;
    });
    return { expander, calls };
}

test("root, repository-prefixed and short paths expand recursive dependencies once", async () => {
    const { expander, calls } = fixture({
        "libraries/geometry/ClosestPair.hpp": '#pragma once\n#include "libraries/geometry/GeometryInt.hpp"',
        "libraries/geometry/GeometryInt.hpp": '#pragma once\n#include "../math/Rational.hpp"',
        "libraries/math/Rational.hpp": "#include <vector>\nstruct Rational {};",
    });
    const output = await expander.expand(
        '#include "kyopro_libraries/libraries/geometry/ClosestPair.hpp"\n' +
        '#include "geometry/ClosestPair.hpp"\n#include "libraries/math/Rational.hpp"',
    );
    assert.equal(output.match(/struct Rational/g).length, 1);
    assert.ok(output.includes("#include <vector>"));
    assert.ok(!output.includes('#include "'));
    assert.ok(!output.includes("#pragma once"));
    assert.equal(calls.length, new Set(calls).size);
    assert.equal(expander.included.size, 3);
});

test("cycles terminate and source pragmas and conditional header code survive", async () => {
    const { expander } = fixture({
        "libraries/graph/A.hpp": '#ifndef A_HPP\n#define A_HPP\n#include "B.hpp"\n#if FLAG\nint a;\n#endif\n#endif // A_HPP',
        "libraries/graph/B.hpp": '#pragma once\n#include "A.hpp"\nint b;',
    });
    const output = await expander.expand('#pragma once\n#include "libraries/graph/A.hpp"');
    assert.equal(output.match(/int b;/g).length, 1);
    assert.ok(output.startsWith("#pragma once\n"));
    assert.ok(output.includes("#if FLAG\nint a;\n#endif"));
    assert.ok(!output.includes("#ifndef A_HPP"));
});

test("comments, raw strings and multiline comment delimiters are preserved", async () => {
    const { expander, calls } = fixture({ "libraries/math/A.hpp": "int a;" });
    const untouched = '/*\n#include "missing.hpp"\n*/\nauto s = R"tag(\n#include "missing.hpp"\n)tag";\n';
    assert.equal(await expander.expand(untouched), untouched);
    assert.equal(calls.length, 0);
    const output = await expander.expand('/* start\n*/ #include "libraries/math/A.hpp" /* tail\nend */');
    assert.ok(output.startsWith("/* start\n*/ \n// begin"));
    assert.ok(output.endsWith(" /* tail\nend */"));
});

test("a conditional first include does not suppress a later unconditional include", async () => {
    const { expander } = fixture({ "libraries/math/A.hpp": "struct A {};" });
    const output = await expander.expand('#if 0\n#include "libraries/math/A.hpp"\n#endif\n#include "libraries/math/A.hpp"');
    assert.equal(output.match(/struct A/g).length, 2);
    const guards = [...output.matchAll(/^#ifndef (GITHUB_LIBRARY_\w+)$/gm)];
    assert.equal(guards.length, 2);
    assert.equal(guards[0][1], guards[1][1]);
});

test("ACL stays untouched by default and expands on request including .hpp fallback", async () => {
    const source = '#include <bits/stdc++.h>\n#include <atcoder/segtree>\n';
    const off = fixture({});
    assert.equal(await off.expander.expand(source), source);
    assert.equal(off.calls.length, 0);
    const on = new Expander({ expandAcl: true }, async (url) => {
        if (url.endsWith("/atcoder/segtree")) return '#include "atcoder/internal_bit"';
        if (url.endsWith("/atcoder/internal_bit.hpp")) return "int acl;";
        return null;
    });
    const output = await on.expand(source);
    assert.ok(output.includes("int acl;"));
    assert.ok(output.includes("#include <bits/stdc++.h>"));
    assert.ok(!output.includes("#include <atcoder/"));
});

test("missing files and network errors reject without generating partial output", async () => {
    const { expander } = fixture({});
    await assert.rejects(expander.expand('#include "libraries/missing.hpp"'), /見つかりません/);
    const offline = new Expander({}, async () => { throw new Error("offline"); });
    await assert.rejects(offline.expand('#include "libraries/math/A.hpp"'), /offline/);
});

test("custom repository refs and paths are encoded, root escapes are rejected", async () => {
    const calls = [];
    const expander = new Expander({ owner: "me", repository: "lib", ref: "release/test" }, async (url) => {
        calls.push(url);
        return "int value;";
    });
    await expander.expand('#include "lib/libraries/math/A B.hpp"');
    assert.equal(calls[0], "https://raw.githubusercontent.com/me/lib/release/test/libraries/math/A%20B.hpp");
    await assert.rejects(expander.expand('#include "../../private.hpp"'), /リポジトリ外/);
});

function editorFixture({ visible = false, legacy = false, textareaOnly = false } = {}) {
    const events = [];
    const plain = { value: "plain", getClientRects: () => visible ? [{}] : [], dispatchEvent: (event) => events.push(event.type) };
    const rich = { value: "rich", getValue() { return this.value; }, setValue(value) { this.value = value; } };
    const page = { Event: class { constructor(type) { this.type = type; } } };
    if (legacy) page.jQuery = () => ({ data: () => ({ doc: rich }) });
    const doc = { querySelector: (selector) => selector === "#editor" ? (legacy || textareaOnly ? null : { env: { editor: rich } }) : selector === ".CodeMirror" ? null : plain };
    return { page, doc, plain, rich, events };
}

test("Ace editor writes also synchronize the textarea and dispatch change events", () => {
    const f = editorFixture();
    const editor = editorAccess(f.page, f.doc);
    assert.equal(editor.read(), "rich");
    editor.write("expanded");
    assert.equal(f.rich.value, "expanded");
    assert.equal(f.plain.value, "expanded");
    assert.deepEqual(f.events, ["input", "change"]);
});

test("plain-text mode, old CodeMirror and the AtCoder getSourceCode helper", () => {
    for (const options of [{ visible: true }, { textareaOnly: true }, { legacy: true }]) {
        const f = editorFixture(options);
        const editor = editorAccess(f.page, f.doc);
        assert.equal(editor.read(), options.legacy ? "rich" : "plain");
        editor.write("new");
        assert.equal(editor.read(), "new");
        f.page.getSourceCode = () => "helper";
        assert.equal(editor.read(), "helper");
    }
});

// Small DOM stand-in: exercise the actual installed click handlers, not copies.
function uiFixture(request, { inColumn = false } = {}) {
    const f = editorFixture({ visible: true });
    const controls = new Map();
    const control = (selector) => {
        if (!controls.has(selector)) controls.set(selector, {
            type: selector.includes("expandAcl") ? "checkbox" : "text",
            addEventListener(type, callback) { this[type] = callback; },
        });
        return controls.get(selector);
    };
    const panel = { style: {}, querySelector: control };
    control('[role="status"]').style = {};
    let placement;
    const parent = { insertBefore(node, before) { placement = { parent: this, node, before }; } };
    const column = { ...parent, firstChild: { sidebar: true } };
    const anchor = { parentNode: parent, closest: () => inColumn ? column : null };
    const baseQuery = f.doc.querySelector;
    f.doc.querySelector = (selector) => selector.startsWith("#editor,") ? anchor : baseQuery(selector);
    f.doc.createElement = () => panel;
    f.doc.getElementById = () => null;
    f.page.confirm = () => false;
    const storage = { get: () => ({}), set() {} };
    assert.equal(install(f.page, f.doc, storage, request), true);
    f.plain.value = '#include "libraries/math/A.hpp"';
    return {
        ...f,
        expand: control('[data-action="expand"]'),
        restore: control('[data-action="restore"]'),
        status: control('[role="status"]'),
        placement, panel, anchor, parent, column,
    };
}

test("UI expands and restores without submitting, and confirms before discarding edits", async () => {
    const f = uiFixture(async () => "struct A {};");
    assert.equal(f.placement.parent, f.parent);
    assert.equal(f.placement.before, f.anchor);
    const original = f.plain.value;
    await f.expand.click();
    assert.ok(f.plain.value.includes("struct A {};"));
    assert.equal(f.restore.disabled, false);
    f.plain.value += "\n// edit";
    f.restore.click();
    assert.ok(f.plain.value.endsWith("// edit"));
    f.page.confirm = () => true;
    f.restore.click();
    assert.equal(f.plain.value, original);
    assert.equal(f.restore.disabled, true);
});

test("UI leaves current code intact on fetch failure or concurrent edits", async () => {
    const failed = uiFixture(async () => { throw new Error("offline"); }, { inColumn: true });
    assert.equal(failed.placement.parent, failed.column);
    assert.equal(failed.placement.before, failed.column.firstChild);
    const original = failed.plain.value;
    await failed.expand.click();
    assert.equal(failed.plain.value, original);
    assert.equal(failed.expand.disabled, false);
    assert.equal(failed.status.textContent, "offline");
    let release;
    const concurrent = uiFixture(() => new Promise((resolve) => { release = resolve; }));
    const pending = concurrent.expand.click();
    concurrent.plain.value = "new code";
    release("struct A {};");
    await pending;
    assert.equal(concurrent.plain.value, "new code");
    assert.match(concurrent.status.textContent, /取得中にコードが変更/);
});

test("real geometry dependencies expand from local files using GitHub URL mapping", async () => {
    const root = path.resolve(__dirname, "..");
    const expander = new Expander({}, async (url) => {
        const relative = url.split("feature/icpc-extra-libraries/")[1];
        try { return await fs.readFile(path.join(root, relative), "utf8"); }
        catch (error) { if (error.code === "ENOENT") return null; throw error; }
    });
    const output = await expander.expand('#include "libraries/geometry/ClosestPair.hpp"');
    assert.ok(output.includes("closest_pair_distance2"));
    assert.ok(output.includes("struct Rational"));
    assert.ok(!/^\s*#\s*include\s*"/m.test(output));
    assert.equal(expander.included.size, 3);
});
