#!/usr/bin/env python3
"""Validate first-party source documentation coverage and basic wording quality."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = [
    ROOT / "src/bootloader",
    ROOT / "src/sequencer",
    ROOT / "src/shared",
    ROOT / "src/simulator",
    ROOT / "src/hwconfig",
    ROOT / "src/tester",
    ROOT / "src/tools",
]
SUFFIXES = {".h", ".hpp", ".c", ".cc", ".cpp", ".cxx"}
EXCLUDED_FILES = {"FreeRTOSConfig.h"}
EXCLUDED_PARTS = {"test", "tests"}
# Generated bitmap payloads are data, not APIs.
EXCLUDED_DATA_HEADERS = {"ati8x8.h", "commodore.h", "tiny5x5.h"}

BANNED = [
    re.compile(p, re.I) for p in [
        r"@param(?:\[[^]]+\])?\s+value\s+Value supplied by the caller\.",
        r"item to access or modify",
        r"Result computed by",
        r"object used to represent",
        r"Persistent value representing",
        r"Runtime .* value used by the owning component",
        r"Provides .* behavior and state",
        r"Groups the fields that make up",
        r"Number of\s+items currently tracked or supported",
        r"New set to store or apply",
        r"represented by the current object state",
        r"Returns the get\b",
        r"Items, in milliseconds",
        r"Radius, in microseconds",
        r"MIDI status.*microseconds",
    ]
]


def strip_comments(s: str) -> str:
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    return re.sub(r"//.*", " ", s)


def split_params(s: str):
    s = s.strip()
    if not s or s == "void":
        return []
    parts, cur, stack = [], "", []
    pairs = {">": "<", ")": "(", "]": "[", "}": "{"}
    for ch in s:
        if ch == "," and not stack:
            parts.append(cur.strip()); cur = ""; continue
        cur += ch
        if ch in "<([{": stack.append(ch)
        elif ch in ">)]}" and stack and stack[-1] == pairs[ch]: stack.pop()
    if cur.strip(): parts.append(cur.strip())
    out = []
    for param in parts:
        # Remove a simple top-level default expression.
        param = re.sub(r"\s*=\s*[^,]+$", "", param).strip()
        if param == "...": out.append(("args", "...")); continue
        fm = re.search(r"\(\s*[*&]\s*([A-Za-z_]\w*)\s*\)", param)
        if fm:
            out.append((fm.group(1), param)); continue
        m = re.search(r"([A-Za-z_]\w*)\s*(?:\[[^]]*\])?\s*$", param)
        if not m:
            out.append(("", param)); continue
        name = m.group(1); typ = param[:m.start(1)].strip()
        out.append((name if typ else "", param if not typ else typ))
    return out


def find_matching_paren(s: str, start: int):
    depth = 0
    for i in range(start, len(s)):
        if s[i] == "(": depth += 1
        elif s[i] == ")":
            depth -= 1
            if depth == 0: return i
    return None


def parse_callable(stmt: str, cls: str = ""):
    clean = " ".join(strip_comments(stmt).split())
    if not clean or clean.startswith(("if ", "for ", "while ", "switch ", "return ", "static_assert")):
        return None
    op = re.search(r"operator\s*(\(\)|\[\]|[^\s(]+)\s*\(", clean)
    if op:
        name = "operator" + op.group(1); p = op.end() - 1; name_start = op.start()
    else:
        m = re.search(r"(~?[A-Za-z_]\w*)\s*\(", clean)
        if not m: return None
        name = m.group(1); p = m.end() - 1; name_start = m.start()
        if name in {"if", "for", "while", "switch", "sizeof", "decltype", "alignof", "ASSERT"}: return None
    end = find_matching_paren(clean, p)
    if end is None: return None
    before = clean[:name_start].strip()
    if before.count("<") > before.count(">"): return None
    if not before and name != cls and name != "~" + cls: return None
    if not before and name.isupper(): return None
    specs = {"virtual", "static", "inline", "constexpr", "consteval", "explicit", "friend", "extern", "mutable"}
    ret = " ".join(t for t in before.split() if t not in specs and not t.startswith("[["))
    if name in {cls, "~" + cls}: ret = ""
    return name, ret, split_params(clean[p + 1:end])


def brace_delta(line: str) -> int:
    s = re.sub(r'"(?:\\.|[^"\\])*"', '""', line)
    s = re.sub(r"'(?:\\.|[^'\\])*'", "''", s)
    s = s.split("//", 1)[0]
    return s.count("{") - s.count("}")


def leading_doc(lines, idx):
    j = idx - 1
    while j >= 0 and not lines[j].strip(): j -= 1
    # Doxygen documentation conventionally precedes a template declaration.
    if j >= 0 and lines[j].strip().startswith("template"):
        j -= 1
        while j >= 0 and not lines[j].strip(): j -= 1
    if j < 0: return ""
    if lines[j].lstrip().startswith(("///", "//!")):
        arr = []
        while j >= 0 and lines[j].lstrip().startswith(("///", "//!")):
            arr.append(lines[j]); j -= 1
        return "\n".join(reversed(arr))
    if "*/" in lines[j]:
        end = j
        while j >= 0 and "/*" not in lines[j]: j -= 1
        if j >= 0 and ("/**" in lines[j] or "/*!" in lines[j]):
            return "\n".join(lines[j:end + 1])
    return ""


def excluded(path: Path) -> bool:
    rel = path.relative_to(ROOT)
    return path.name in EXCLUDED_FILES | EXCLUDED_DATA_HEADERS or any(p in EXCLUDED_PARTS for p in rel.parts)


def top_eq_before_paren(stmt: str) -> bool:
    p, e = stmt.find("("), stmt.find("=")
    return e >= 0 and (p < 0 or e < p)

issues = []
counts = {"files": 0, "types": 0, "methods": 0, "members": 0, "enum_values": 0}

for source_root in SOURCE_ROOTS:
    if not source_root.exists(): continue
    for path in source_root.rglob("*"):
        if path.suffix.lower() not in SUFFIXES or excluded(path): continue
        rel = path.relative_to(ROOT)
        text = path.read_text(encoding="utf-8", errors="ignore")
        counts["files"] += 1
        head = text[:2200]
        if "@file" not in head: issues.append((rel, 1, "missing @file documentation"))
        if "@brief" not in head: issues.append((rel, 1, "missing file @brief"))
        if "SPDX-License-Identifier:" not in head: issues.append((rel, 1, "missing SPDX license identifier"))
        for pattern in BANNED:
            for m in pattern.finditer(text):
                line = text.count("\n", 0, m.start()) + 1
                issues.append((rel, line, f"low-quality documentation phrase: {m.group(0)!r}"))

        if path.suffix.lower() not in {".h", ".hpp"}: continue
        lines = text.splitlines()
        depth = 0
        scopes = []
        occupied_until = -1
        enum_depths = []
        for i, line in enumerate(lines):
            while scopes and depth < scopes[-1]["depth"]: scopes.pop()
            while enum_depths and depth < enum_depths[-1]: enum_depths.pop()
            class_match = re.match(r"\s*(class|struct|union)\s+([A-Za-z_]\w*)[^;]*\{", line)
            enum_match = re.match(r"\s*enum(?:\s+class)?\s*([A-Za-z_]\w*)?[^;]*\{", line)
            if class_match:
                counts["types"] += 1
                if not leading_doc(lines, i): issues.append((rel, i + 1, f"undocumented {class_match.group(1)} {class_match.group(2)}"))
                scopes.append({"kind": class_match.group(1), "name": class_match.group(2), "depth": depth + 1})
            elif enum_match:
                counts["types"] += 1
                if not leading_doc(lines, i): issues.append((rel, i + 1, "undocumented enum"))
                enum_depths.append(depth + 1)

            # Enumerator coverage for simple one-value-per-line declarations.
            if enum_depths and depth == enum_depths[-1]:
                s = line.strip()
                if re.match(r"[A-Za-z_]\w*\s*(?:=\s*[^,]+)?\s*,?\s*(?://.*)?$", s) and not s.startswith(("enum", "//")):
                    name = re.match(r"([A-Za-z_]\w*)", s).group(1)
                    if name not in {"public", "private", "protected"}:
                        counts["enum_values"] += 1
                        if not (leading_doc(lines, i) or "///<" in line or "//!<" in line):
                            issues.append((rel, i + 1, f"undocumented enumerator {name}"))

            active = next((s for s in reversed(scopes) if depth == s["depth"]), None)
            s = line.strip()
            if i > occupied_until and active and active["kind"] in {"class", "struct", "union"} and s and not s.startswith(("/", "*", "#")):
                if s in {"public:", "private:", "protected:", "};", "}"} or s.startswith("};") or class_match or enum_match or s == "{":
                    pass
                elif re.match(r"(using|typedef|static_assert|friend\s+(?:class|struct)|template\s*<)", s):
                    pass
                elif s.endswith(","):
                    # Usually a parameter/initializer line of a multiline declaration.
                    pass
                else:
                    stmt, j = [], i
                    paren_balance = 0
                    while j < len(lines) and j - i < 40:
                        part = lines[j].strip(); stmt.append(part)
                        clean_part = strip_comments(part)
                        paren_balance += clean_part.count("(") - clean_part.count(")")
                        joined = " ".join(stmt)
                        if paren_balance <= 0 and (";" in clean_part or "{" in clean_part): break
                        j += 1
                    joined = " ".join(stmt)
                    call = parse_callable(joined, active["name"])
                    if call:
                        counts["methods"] += 1
                        name, ret, params = call
                        doc = leading_doc(lines, i)
                        if not doc:
                            issues.append((rel, i + 1, f"undocumented method {name}"))
                        else:
                            if "@brief" not in doc:
                                issues.append((rel, i + 1, f"method {name} has no @brief"))
                            for param_name, _ in params:
                                if param_name and not re.search(r"@param(?:\[[^]]+\])?\s+" + re.escape(param_name) + r"\b", doc):
                                    issues.append((rel, i + 1, f"method {name}: undocumented parameter {param_name}"))
                            if ret and ret != "void" and not re.search(r"@return\b", doc):
                                issues.append((rel, i + 1, f"method {name}: missing @return"))
                        occupied_until = j
                    elif (";" in joined and "(" not in joined and not joined.startswith("return ")):
                        if s.endswith(");") and "," in s:
                            depth += brace_delta(line)
                            continue
                        counts["members"] += 1
                        if not (leading_doc(lines, i) or "///<" in line or "//!<" in line):
                            issues.append((rel, i + 1, f"undocumented member: {s}"))
            depth += brace_delta(line)

if issues:
    print(f"Source documentation check FAILED ({len(issues)} issue(s))")
    for rel, line, msg in issues[:300]: print(f"{rel}:{line}: {msg}")
    if len(issues) > 300: print(f"... {len(issues) - 300} additional issue(s)")
    sys.exit(1)

print(
    "Source documentation check PASS: "
    f"{counts['files']} files, {counts['types']} types, {counts['methods']} methods, "
    f"{counts['members']} members, {counts['enum_values']} enumerators"
)
