#!/usr/bin/env bash
# Render a Markdown file to PDF, with Mermaid diagrams drawn properly.
#
#   tools/md-to-pdf.sh <file.md> [output.pdf] [paper]
#
# paper defaults to A4. Use a larger size for diagram-heavy documents: a
# Mermaid chart is scaled to the page width, so a big flowchart on A4 comes out
# correct but unreadably small. A3 or A2 fixes that without touching the
# diagram.
#
#   tools/md-to-pdf.sh docs/visual-representation.md "" A2
#
# Uses marked for Markdown and headless Chrome for the PDF. Mermaid renders in
# the page, so Chrome is given a virtual time budget to let it finish before
# printing -- without that you get blank boxes where the diagrams should be.
set -euo pipefail

SRC="${1:?usage: md-to-pdf.sh <file.md> [out.pdf]}"
OUT="${2:-}"; OUT="${OUT:-${SRC%.md}.pdf}"
PAPER="${3:-A4}"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

CHROME="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
[ -x "$CHROME" ] || { echo "Google Chrome not found at $CHROME" >&2; exit 1; }

cat > "$WORK/render.mjs" <<'JS'
import { marked } from 'marked';
import { readFileSync, writeFileSync } from 'node:fs';
import { basename } from 'node:path';

const src = process.argv[2], out = process.argv[3];

// CSS Paged Media only defines A5..A3, B5, B4, letter, legal and ledger as
// keywords. Anything bigger must be given as explicit dimensions, or Chrome
// silently falls back to Letter -- which looks like the setting was ignored.
const PAPERS = {
  A2: '420mm 594mm', A1: '594mm 841mm', A0: '841mm 1189mm',
  'A2 landscape': '594mm 420mm', 'A1 landscape': '841mm 594mm',
};
const arg = process.argv[4] || 'A4';
const paper = PAPERS[arg] || arg;
let body = marked.parse(readFileSync(src, 'utf8'));

// marked turns ```mermaid into <pre><code class="language-mermaid">. Mermaid
// needs a plain <div class="mermaid"> holding unescaped text.
const unescape = s => s.replace(/&lt;/g,'<').replace(/&gt;/g,'>')
                       .replace(/&quot;/g,'"').replace(/&#39;/g,"'")
                       .replace(/&amp;/g,'&');
body = body.replace(
  /<pre><code class="language-mermaid">([\s\S]*?)<\/code><\/pre>/g,
  (_, code) => `<div class="mermaid">${unescape(code)}</div>`);

writeFileSync(out, `<!doctype html><html><head><meta charset="utf-8">
<title>${basename(src)}</title>
<script src="https://cdn.jsdelivr.net/npm/mermaid@11.4.1/dist/mermaid.min.js"><\/script>
<style>
  @page { size: ${paper}; margin: 18mm 16mm; }
  body { font: 11pt/1.55 -apple-system, system-ui, sans-serif; color: #111; max-width: none; }
  h1 { font-size: 20pt; border-bottom: 2px solid #111; padding-bottom: .3em; }
  h2 { font-size: 15pt; margin-top: 1.6em; border-bottom: 1px solid #ccc;
       padding-bottom: .2em; page-break-after: avoid; }
  h3 { font-size: 12pt; page-break-after: avoid; }
  p, li { orphans: 3; widows: 3; }
  code { font-family: ui-monospace, Menlo, monospace; font-size: .88em;
         background: #f4f4f4; padding: .12em .3em; border-radius: 3px; }
  pre { background: #f6f6f6; padding: .8em 1em; border-radius: 6px;
        border: 1px solid #e3e3e3; overflow-x: auto; page-break-inside: avoid; }
  pre code { background: none; padding: 0; }
  blockquote { border-left: 3px solid #888; margin-left: 0; padding-left: 1em;
               color: #444; background: #fafafa; page-break-inside: avoid; }
  table { border-collapse: collapse; width: 100%; font-size: .92em;
          page-break-inside: avoid; }
  th, td { border: 1px solid #ccc; padding: .45em .6em; text-align: left; vertical-align: top; }
  th { background: #f0f0f0; }
  .mermaid { text-align: center; margin: 1.4em 0; page-break-inside: avoid; }
  a { color: #0645ad; text-decoration: none; }
  hr { border: none; border-top: 1px solid #ddd; margin: 2em 0; }
  strong { color: #000; }
</style></head><body>
${body}
<script>mermaid.initialize({startOnLoad:true,theme:'neutral',
  // htmlLabels:false is essential for printing: HTML labels render inside
  // <foreignObject>, which Chrome's print-to-PDF silently drops, leaving a
  // blank page whose text still extracts.
  htmlLabels:false,
  flowchart:{useMaxWidth:true,htmlLabels:false},
  securityLevel:'loose'});<\/script>
</body></html>`);
JS

echo "rendering $SRC ..."
(cd "$WORK" && npm install --silent --no-fund --no-audit marked >/dev/null 2>&1)
node "$WORK/render.mjs" "$PWD/$SRC" "$WORK/page.html" "$PAPER"

"$CHROME" --headless=new --disable-gpu --no-sandbox \
    --virtual-time-budget=30000 --no-pdf-header-footer \
    --print-to-pdf="$PWD/$OUT" "file://$WORK/page.html" >/dev/null 2>&1

echo "wrote $OUT"
