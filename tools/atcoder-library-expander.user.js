// ==UserScript==
// @name         AtCoder Library Expander
// @namespace    https://github.com/breakerso6/kyopro_libraries
// @version      1.0.1
// @description  GitHub の C++ ライブラリを提出欄で再帰展開します。
// @match        https://atcoder.jp/contests/*/submit*
// @match        https://atcoder.jp/contests/*/tasks/*
// @run-at       document-idle
// @grant        unsafeWindow
// @grant        GM_xmlhttpRequest
// @grant        GM_getValue
// @grant        GM_setValue
// @connect      raw.githubusercontent.com
// @noframes
// @license      CC0-1.0
// ==/UserScript==

(function () {
    "use strict";

    const DEFAULTS = {
        owner: "breakerso6", repository: "kyopro_libraries",
        ref: "feature/icpc-extra-libraries", expandAcl: false,
    };
    const CATEGORIES = /^(algorithm|data_structure|geometry|graph|math|string|tree|utility)\//;
    const INCLUDE = /^\s*#\s*include\s*([<"])([^>"\r\n]+)[>"]\s*(?:\/\/.*)?$/;

    function normalize(path) {
        const parts = [];
        if (/^[\/\\]|^[A-Za-z]:|[\\?#\x00-\x1f]/.test(path)) {
            throw new Error(`GitHub 上の相対パスで指定してください: ${path}`);
        }
        for (const part of path.split("/")) {
            if (part === "..") {
                if (!parts.length) throw new Error(`リポジトリ外のパスです: ${path}`);
                parts.pop();
            } else if (part && part !== ".") parts.push(part);
        }
        return parts.join("/");
    }

    function rawUrl(repo, path) {
        const encodePath = (value) => value.split("/").map(encodeURIComponent).join("/");
        return `https://raw.githubusercontent.com/${encodeURIComponent(repo.owner)}/${encodeURIComponent(repo.repository)}/${encodePath(repo.ref)}/${encodePath(path)}`;
    }

    // Mask comments and raw strings while keeping line numbers and include strings.
    function directiveView(source) {
        let result = "", block = false, rawEnd = "", quote = "";
        for (let i = 0; i < source.length;) {
            const c = source[i], next = source[i + 1];
            if (block || rawEnd) {
                const end = block ? "*/" : rawEnd;
                if (source.startsWith(end, i)) {
                    result += " ".repeat(end.length);
                    i += end.length;
                    block = false;
                    rawEnd = "";
                } else { result += c === "\n" ? "\n" : " "; i++; }
            } else if (quote) {
                result += c;
                i++;
                if (c === "\\" && i < source.length) { result += source[i++]; }
                else if (c === quote || c === "\n") quote = "";
            } else if (c === "/" && next === "*") {
                block = true; result += "  "; i += 2;
            } else if (c === "/" && next === "/") {
                while (i < source.length && source[i] !== "\n") { result += " "; i++; }
            } else {
                const raw = c === "R" && next === '"' && source.slice(i, i + 20).match(/^R"([^ ()\\\t\r\n]{0,16})\(/);
                if (raw) {
                    rawEnd = `)${raw[1]}"`;
                    result += " ".repeat(raw[0].length);
                    i += raw[0].length;
                } else {
                    if (c === '"' || c === "'") quote = c;
                    result += c; i++;
                }
            }
        }
        return result.split("\n");
    }

    function headerLines(source) {
        const lines = source.replace(/\r\n?/g, "\n").split("\n");
        const directives = directiveView(lines.join("\n"));
        const skip = new Set();
        const meaningful = [];
        directives.forEach((line, i) => {
            if (/^\s*#\s*pragma\s+once\s*$/.test(line)) skip.add(i);
            else if (line.trim()) meaningful.push(i);
        });
        const [first, second] = meaningful;
        const guard = directives[first]?.match(/^\s*#\s*ifndef\s+([A-Z_][A-Z0-9_]*)\s*$/);
        if (guard && directives[second]?.trim() === `#define ${guard[1]}`) {
            let depth = 0;
            for (const i of meaningful) {
                if (/^\s*#\s*(if|ifdef|ifndef)\b/.test(directives[i])) depth++;
                if (/^\s*#\s*endif\b/.test(directives[i]) && --depth === 0) {
                    if (i === meaningful.at(-1)) [first, second, i].forEach((j) => skip.add(j));
                    break;
                }
            }
        }
        return { lines, directives, skip };
    }

    class Expander {
        constructor(settings, request, progress = () => {}) {
            this.settings = { ...DEFAULTS, ...settings };
            this.request = request;
            this.progress = progress;
            this.cache = new Map();
            this.included = new Set();
            this.unconditional = new Set();
            this.active = new Set();
        }

        async resolve(name, delimiter, current) {
            const acl = name.startsWith("atcoder/");
            if (acl && !this.settings.expandAcl) return null;
            const prefix = `${this.settings.repository}/`;
            if (name.startsWith(prefix)) name = name.slice(prefix.length);
            const local = name.startsWith("libraries/") || CATEGORIES.test(name);
            if (!acl && delimiter === "<" && !local) return null;
            const repo = acl ? { owner: "atcoder", repository: "ac-library", ref: "master" } : this.settings;
            const paths = [];
            if (!acl && current && !local) paths.push(`${current.slice(0, current.lastIndexOf("/") + 1)}${name}`);
            if (!current || !/^\.{1,2}\//.test(name)) {
                paths.push(name);
                if (!acl && !name.startsWith("libraries/")) paths.push(`libraries/${name}`);
            }
            for (const unresolved of new Set(paths)) {
                const path = normalize(unresolved);
                for (const candidate of path.endsWith(".hpp") ? [path] : [path, `${path}.hpp`]) {
                    const url = rawUrl(repo, candidate);
                    if (!this.cache.has(url)) {
                        this.progress(candidate);
                        this.cache.set(url, this.request(url));
                    }
                    const text = await this.cache.get(url);
                    if (text !== null) return { path: candidate, url, text };
                }
            }
            throw new Error(`include が見つかりません: ${name} (${repo.owner}/${repo.repository}@${repo.ref})`);
        }

        async expand(source) {
            this.included.clear();
            this.unconditional.clear();
            this.active.clear();
            return this.expandText(source, null, 0, false);
        }

        async expandText(source, current, depth, conditional) {
            if (depth > 128) throw new Error("include のネストが深すぎます。");
            const { lines, directives, skip } = headerLines(source);
            const result = [];
            let conditionDepth = 0;
            for (let i = 0; i < lines.length; i++) {
                if (current && skip.has(i)) continue;
                if (/^\s*#\s*(if|ifdef|ifndef)\b/.test(directives[i])) conditionDepth++;
                if (/^\s*#\s*endif\b/.test(directives[i])) conditionDepth--;
                const match = directives[i].match(INCLUDE);
                if (match) {
                    const header = await this.resolve(match[2], match[1], current);
                    if (header) {
                        // Keep comment delimiters surrounding a directive intact.
                        const hash = directives[i].indexOf("#");
                        const directive = directives[i].slice(hash).match(/^#\s*include\s*[<"][^>"\r\n]+[>"]/)[0];
                        const prefix = lines[i].slice(0, hash);
                        const suffix = lines[i].slice(hash + directive.length);
                        if (prefix.trim()) result.push(prefix);
                        if (!this.unconditional.has(header.url) && !this.active.has(header.url)) {
                            this.included.add(header.url);
                            const guarded = conditional || conditionDepth > 0;
                            if (!guarded) this.unconditional.add(header.url);
                            this.active.add(header.url);
                            // URL character codes form a collision-free C++ macro identifier.
                            const guard = `GITHUB_LIBRARY_${Array.from(header.url, (c) => c.charCodeAt(0).toString(16).padStart(2, "0")).join("")}_HPP`;
                            result.push(`// begin include: ${match[2]}\n// ${header.url}`);
                            result.push(`#ifndef ${guard}\n#define ${guard}`);
                            result.push(await this.expandText(header.text, header.path, depth + 1, guarded));
                            result.push(`#endif // ${guard}\n// end include: ${match[2]}`);
                            this.active.delete(header.url);
                        }
                        if (suffix.trim()) result.push(suffix);
                        continue;
                    }
                }
                result.push(lines[i]);
            }
            return result.join("\n");
        }
    }

    function editorAccess(page, doc) {
        const plain = doc.querySelector('#plain-textarea, .plain-textarea, textarea[name="sourceCode"], textarea[name="data.SourceCode"], textarea#sourceCode');
        const node = doc.querySelector("#editor");
        let editor = node?.env?.editor;
        if (!editor && node && page.ace?.edit) editor = page.ace.edit(node);
        if (!editor && page.jQuery) {
            const legacy = page.jQuery(".editor").data("editor");
            editor = legacy?.doc || legacy;
        }
        if (!editor) editor = doc.querySelector(".CodeMirror")?.CodeMirror;
        if (!editor && !plain) throw new Error("提出用エディタが見つかりません。");
        return {
            read() {
                if (typeof page.getSourceCode === "function") return page.getSourceCode();
                if (plain && plain.getClientRects().length) return plain.value;
                if (editor?.getValue) return editor.getValue();
                return plain.value;
            },
            write(value) {
                if (editor?.setValue) editor.setValue(value, -1);
                if (plain) {
                    plain.value = value;
                    for (const type of ["input", "change"]) plain.dispatchEvent(new page.Event(type, { bubbles: true }));
                }
            },
        };
    }

    function requestHeader(url) {
        return new Promise((resolve, reject) => {
            GM_xmlhttpRequest({
                method: "GET", url, timeout: 20000, anonymous: true,
                onload(response) {
                    if (response.status === 404) resolve(null);
                    else if (response.status === 200) resolve(response.responseText);
                    else reject(new Error(`GitHub からの取得に失敗しました (HTTP ${response.status}): ${url}`));
                },
                onerror: () => reject(new Error(`GitHub に接続できません: ${url}`)),
                ontimeout: () => reject(new Error(`取得がタイムアウトしました: ${url}`)),
            });
        });
    }

    function install(page, doc, storage, request = requestHeader) {
        // #sourceCode can be the whole Bootstrap form row, including its label
        // and sidebar. Mount inside the editor column instead of before that row.
        const editorNode = doc.querySelector("#editor, #plain-textarea, .plain-textarea, textarea[name='data.SourceCode'], textarea[name='sourceCode'], .editor");
        const source = doc.getElementById("sourceCode");
        const column = editorNode?.closest(".col-sm-10, .col-md-10, .col-lg-10, .col-xs-10");
        const anchor = column ? null : editorNode || source;
        const container = column || anchor?.parentNode;
        if (!container || doc.getElementById("github-library-expander")) return false;
        const panel = doc.createElement("div");
        panel.id = "github-library-expander";
        panel.style.cssText = "display:flow-root;clear:both;box-sizing:border-box;width:100%;min-width:0;max-width:100%;margin:0 0 10px;padding:10px;border:1px solid #ddd;border-radius:4px;background:#fafafa;text-align:left;";
        // All markup is static; remote source is only written into the code editor.
        panel.innerHTML = `
            <style>
                #github-library-expander .ghle-actions { display:flex;flex-wrap:wrap;align-items:center;gap:6px; }
                #github-library-expander [role="status"] { flex:1 1 200px;min-width:0;white-space:pre-wrap;overflow-wrap:anywhere; }
                #github-library-expander details { margin-top:8px; }
                #github-library-expander summary { cursor:pointer;text-align:left; }
                #github-library-expander .ghle-settings { display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,180px),1fr));gap:10px;margin-top:10px; }
                #github-library-expander label { display:flex;flex-direction:column;gap:4px;float:none;min-width:0;width:auto;margin:0;font-weight:normal;text-align:left; }
                #github-library-expander input:not([type="checkbox"]) { box-sizing:border-box;width:100%;min-width:0;max-width:100%;height:30px;padding:4px 6px;border:1px solid #ccc;border-radius:3px; }
                #github-library-expander .ghle-checkbox { grid-column:1/-1;flex-direction:row;align-items:center;gap:6px; }
                #github-library-expander input[type="checkbox"] { position:static;float:none;width:auto;margin:0; }
            </style>
            <div class="ghle-actions">
                <button type="button" class="btn btn-default btn-sm" data-action="expand">ライブラリを展開</button>
                <button type="button" class="btn btn-default btn-sm" data-action="restore" disabled>元に戻す</button>
                <span role="status" aria-live="polite"></span>
            </div>
            <details><summary>GitHub の取得設定</summary>
                <div class="ghle-settings">
                    <label>Owner <input data-setting="owner"></label>
                    <label>Repository <input data-setting="repository"></label>
                    <label>ブランチ・タグ・コミット <input data-setting="ref"></label>
                    <label class="ghle-checkbox"><input type="checkbox" data-setting="expandAcl"> ACL も展開する</label>
                </div>
            </details>`;
        container.insertBefore(panel, column ? column.firstChild : anchor);
        const settings = { ...DEFAULTS, ...storage.get() };
        for (const key of Object.keys(DEFAULTS)) {
            const input = panel.querySelector(`[data-setting="${key}"]`);
            if (input.type === "checkbox") input.checked = settings[key];
            else input.value = settings[key];
        }
        const expandButton = panel.querySelector('[data-action="expand"]');
        const restoreButton = panel.querySelector('[data-action="restore"]');
        const status = panel.querySelector('[role="status"]');
        let backup = null, expandedValue = null;
        const message = (text, error = false) => {
            status.textContent = text;
            status.style.color = error ? "#b00" : "";
        };
        expandButton.addEventListener("click", async () => {
            expandButton.disabled = true;
            restoreButton.disabled = true;
            try {
                const config = {};
                for (const key of Object.keys(DEFAULTS)) {
                    const input = panel.querySelector(`[data-setting="${key}"]`);
                    config[key] = input.type === "checkbox" ? input.checked : input.value.trim();
                }
                if (!/^[\w.-]+$/.test(config.owner) || !/^[\w.-]+$/.test(config.repository) ||
                    !config.ref || /[?#\\\s]|(^|\/)\.\.?($|\/)/.test(config.ref)) {
                    throw new Error("Owner・Repository・ブランチの設定を確認してください。");
                }
                storage.set(config);
                const editor = editorAccess(page, doc);
                const original = editor.read();
                const expander = new Expander(config, request, (path) => message(`取得中: ${path}`));
                const expanded = await expander.expand(original);
                if (editorAccess(page, doc).read() !== original) {
                    throw new Error("取得中にコードが変更されたため、反映を中止しました。もう一度展開してください。");
                }
                if (!expander.included.size) { message("展開対象の include はありません。"); return; }
                backup = original;
                expandedValue = expanded;
                editorAccess(page, doc).write(expanded);
                message(`${expander.included.size} ファイルを展開しました。`);
            } catch (error) { message(error.message, true); }
            finally {
                expandButton.disabled = false;
                restoreButton.disabled = backup === null;
            }
        });
        restoreButton.addEventListener("click", () => {
            try {
                const editor = editorAccess(page, doc);
                if (editor.read() !== expandedValue && !page.confirm("展開後の編集内容を破棄して、展開前のコードに戻しますか？")) return;
                editor.write(backup);
                backup = expandedValue = null;
                restoreButton.disabled = true;
                message("展開前のコードに戻しました。");
            } catch (error) { message(error.message, true); }
        });
        return true;
    }

    if (typeof module === "object" && module.exports) {
        module.exports = { Expander, editorAccess, install, rawUrl };
    } else {
        const page = typeof unsafeWindow === "undefined" ? window : unsafeWindow;
        const storage = {
            get: () => GM_getValue("github-library-expander-settings", {}),
            set: (value) => GM_setValue("github-library-expander-settings", value),
        };
        if (!install(page, document, storage)) {
            const observer = new MutationObserver(() => {
                if (install(page, document, storage)) observer.disconnect();
            });
            observer.observe(document.body, { childList: true, subtree: true });
            setTimeout(() => observer.disconnect(), 30000);
        }
    }
})();
