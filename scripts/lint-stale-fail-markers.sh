#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

matches=$(git -C "$PROJECT_DIR" grep -nE \
    'Expected to (FAIL|fail)|XFAIL|XFail' \
    -- 'tests/**' 2>/dev/null | grep -v 'QEXPECT_FAIL' || true)

if [ -n "$matches" ]; then
    echo "Stale expected-to-fail markers found (use QEXPECT_FAIL macro):"
    echo "$matches"
    exit 1
fi
