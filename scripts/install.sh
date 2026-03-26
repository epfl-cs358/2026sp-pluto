#!/usr/bin/env bash
# =============================================================================
# install.sh — Set up the Python virtual environment and install dependencies.
#
# Behaviour:
#   - Checks for Python 3.13 (opens download page if missing)
#   - Creates .venv at the project root
#   - Installs packages from requirements.txt
#   - Writes a stamp file (.installed) so subsequent runs skip heavy work
# =============================================================================

set -euo pipefail

# Paths:
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
STAMP="$SCRIPT_DIR/.installed"
VENV="$PROJECT_ROOT/.venv"
REQUIREMENTS="$PROJECT_ROOT/requirements.txt"

# Stamp check:
if [[ -f "$STAMP" ]]; then
    echo "[install] Already installed (stamp found). Skipping."
    exit 0
fi

echo "[install] Starting installation..."

# Python 3.13 detection
PYTHON=""
DOWNLOAD_URL="https://www.python.org/downloads/"

for candidate in python3.13 python3 python; do
    if command -v "$candidate" &>/dev/null; then
        raw_ver=$("$candidate" --version 2>&1 | grep -oE '[0-9]+\.[0-9]+' | head -1)
        if [[ "$raw_ver" == "3.13" ]]; then
            PYTHON="$candidate"
            break
        fi
    fi
done

if [[ -z "$PYTHON" ]]; then
    echo ""
    echo "ERROR:"
    echo "    Python 3.13 was not found on this system."
    echo "    Please download and install it from:"
    echo "    $DOWNLOAD_URL"
    echo ""
    if command -v open &>/dev/null; then
        open "$DOWNLOAD_URL" 2>/dev/null || true
    elif command -v xdg-open &>/dev/null; then
        xdg-open "$DOWNLOAD_URL" 2>/dev/null &
    fi
    exit 1
fi

echo "[install] Found: $($PYTHON --version)"

# Virtual environment:
if [[ -d "$VENV" ]]; then
    echo "[install] Removing existing virtual environment..."
    rm -rf "$VENV"
fi

echo "[install] Creating virtual environment at: $VENV"
"$PYTHON" -m venv "$VENV"

# Dependencies:
echo "[install] Upgrading pip..."
"$VENV/bin/pip" install --upgrade pip --quiet

echo "[install] Installing requirements from: $REQUIREMENTS"
"$VENV/bin/pip" install -r "$REQUIREMENTS"

# Stamp:
touch "$STAMP"
echo ""
echo "[install] Installation complete."