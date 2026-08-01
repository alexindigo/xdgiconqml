#!/usr/bin/env python3
"""
Check every Q_PROPERTY declaration for compatible NOTIFY signal signature.
Compatible = parameterless OR same underlying type as property.

Works on the flat-header layout used in this repo; does not use libclang.
"""
import re
import sys
from pathlib import Path

Q_PROP = re.compile(
    r"Q_PROPERTY\s*\(\s*(?P<type>[\w:<>]+(?:\s*[*&])?)\s+(?P<name>\w+)"
    r"[^)]*NOTIFY\s+(?P<sig>\w+)[^)]*\)",
    re.DOTALL,
)

SIG_DECL = re.compile(
    r"void\s+(?P<name>\w+)\s*\((?P<params>[^)]*)\)\s*(?:const\s*)?;"
)


def normalize_type(t: str) -> str:
    return re.sub(r"\s+", "", t).replace("const", "").replace("&", "").replace("*", "")


def check_header(path: Path) -> list[str]:
    text = path.read_text()
    errors: list[str] = []
    signals: dict[str, str] = {}
    for m in SIG_DECL.finditer(text):
        signals[m.group("name")] = m.group("params").strip()

    for m in Q_PROP.finditer(text):
        prop_type = normalize_type(m.group("type"))
        prop_name = m.group("name")
        sig_name = m.group("sig")

        if sig_name not in signals:
            errors.append(
                f"{path}: Q_PROPERTY '{prop_name}' declares NOTIFY "
                f"'{sig_name}' but no such signal is declared"
            )
            continue

        params = signals[sig_name]
        if not params:
            continue

        first = params.split(",")[0].strip()
        first_type = re.sub(r"\s+\w+$", "", first).strip()
        sig_type = normalize_type(first_type)

        if sig_type != prop_type:
            errors.append(
                f"{path}: Q_PROPERTY '{prop_name}' has type "
                f"'{m.group('type').strip()}' but NOTIFY signal "
                f"'{sig_name}' declares parameter type '{first_type}' "
                f"(normalized: {sig_type} vs {prop_type})"
            )

    return errors


def main() -> int:
    root = Path(__file__).resolve().parent.parent
    headers = sorted(root.glob("*.h"))
    all_errors: list[str] = []
    for h in headers:
        all_errors.extend(check_header(h))

    for e in all_errors:
        print(f"error: {e}", file=sys.stderr)

    return 1 if all_errors else 0


if __name__ == "__main__":
    sys.exit(main())
