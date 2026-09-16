#!/usr/bin/env python3
"""scatter — regenerate this README from the Square's own stream.

Dada is not the breaking of sentences; that is vandalism. Dada is the
shuffling of things that may honestly be shuffled — the order of household
lists, the order of quotable corollaries — each unit carried whole, so the
sense survives. The new order is decreed by raylift, the two-horse oracle
that reads noise.png, and is therefore canonical, reproducible, and exactly
wrong in the right way.

What is never shuffled: paragraphs, numbered steps, code, tables, headings,
pixel art, footnotes, and the oracles' own words.

Usage:
    ./scatter.py            regenerate README.md from noise.png
"""

import re
import subprocess
import sys

STONE = "noise.png"
README = "README.md"
MAXR = 1000

S = []
K = 0


def draw(m):
    global K
    v = S[K]
    K += 1
    return v % m


def shuffle(items):
    for i in range(len(items) - 1, 0, -1):
        j = draw(i + 1)
        items[i], items[j] = items[j], items[i]
    return items


def load_stream():
    global S
    p = subprocess.run(
        ["bash", "-c", f"./raylift.sh {STONE} | head -n {MAXR}"],
        capture_output=True,
        text=True,
        check=True,
    )
    S = [int(x) for x in p.stdout.split()]
    if not S:
        sys.exit("scatter: the horses returned nothing.")


def is_bullet(s):
    return bool(re.match(r"^[-*] ", s))


def is_numbered(s):
    return bool(re.match(r"^\d+[.)] ", s))


def is_quote(s):
    return s.startswith(">")


def is_special(s):
    return (
        s == ""
        or s == "---"
        or s.startswith("```")
        or re.match(r"^#{1,6} ", s)
        or re.match(r"^\[\^[^]]+\]:", s)
        or s.startswith("|")
        or is_bullet(s)
        or is_numbered(s)
        or is_quote(s)
        or re.match(r"^<p align", s)
    )


def group_items(lines, i, n, marker):
    """Collect a list group, each item = start line + indented continuation."""
    items = []
    while i < n and marker(lines[i].strip()):
        buf = [lines[i]]
        i += 1
        while i < n:
            cont = lines[i]
            if cont == "":
                nxt = lines[i + 1] if i + 1 < n else ""
                if nxt.startswith(("    ", "\t")) or nxt.strip().startswith("```"):
                    buf.append(cont)
                    i += 1
                    continue
                break
            if cont.startswith("```"):
                buf.append(cont)
                i += 1
                while i < n and not lines[i].strip().startswith("```"):
                    buf.append(lines[i])
                    i += 1
                if i < n:
                    buf.append(lines[i])
                    i += 1
                continue
            if cont.startswith(("    ", "\t", "  ")):
                buf.append(cont)
                i += 1
                continue
            break
        items.append("\n".join(buf).rstrip("\n"))
    return items, i


def main():
    load_stream()

    lines = open(README, encoding="utf-8").read().split("\n")
    out = []
    i, n = 0, len(lines)
    first_quote = True

    while i < n:
        line = lines[i]
        s = line.strip()

        if s.startswith("```"):
            out.append(lines[i])
            i += 1
            while i < n and not lines[i].startswith("```"):
                out.append(lines[i])
                i += 1
            if i < n:
                out.append(lines[i])
                i += 1
            continue

        if (
            s in ("", "---")
            or re.match(r"^#{1,6} ", s)
            or re.match(r"^\[\^[^]]+\]:", s)
            or re.match(r"^<p align", s)
        ):
            out.append(line)
            i += 1
            continue

        if s.startswith("|"):
            while i < n and lines[i].strip().startswith("|"):
                out.append(lines[i])
                i += 1
            continue

        if is_numbered(s):
            items, i = group_items(lines, i, n, is_numbered)
            out.extend(items)
            continue

        if is_bullet(s):
            items, i = group_items(lines, i, n, is_bullet)
            if len(items) > 1:
                items = shuffle(items)
            out.extend(items)
            continue

        if is_quote(s):
            qs = []
            while i < n and lines[i].strip().startswith(">"):
                qs.append(lines[i])
                i += 1
            joined = "\n".join(qs)
            fragile = (
                any(re.search(r">\s*\*\*|\>\s*---", q) for q in qs)
                or re.search(r"[≈=]|≫|⊕|<i>|hatch|\bmod\b|seed =", joined)
            )
            units, cur = [], []
            for q in qs:
                if q.strip() == ">":
                    if cur:
                        units.append(cur)
                        cur = []
                else:
                    cur.append(q)
            if cur:
                units.append(cur)
            if first_quote:
                if len(qs) >= 3 and qs[1].strip() == ">" and "fn. 7" in joined:
                    fragile = True  # the epigraph keeps its altar
            first_quote = False
            if (not fragile) and len(units) > 1:
                units = shuffle(units)
            for u in units:
                out.extend(u)
                out.append(">")
            if out and out[-1] == ">":
                out.pop()
            continue

        para_start = i
        while i < n:
            t = lines[i].strip()
            if is_special(t) or t == "":
                break
            i += 1
        out.extend(lines[para_start:i])

    body = "\n".join(out)
    if not body.endswith("\n"):
        body += "\n"
    open(README, "w", encoding="utf-8").write(body)
    print(f"scatter: {K} true answers consumed; README re-scattered.")


if __name__ == "__main__":
    main()
