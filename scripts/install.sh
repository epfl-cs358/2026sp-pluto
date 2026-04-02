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
if command -v sha256sum &>/dev/null; then
    CURRENT_HASH=$(sha256sum "$REQUIREMENTS" | awk '{print $1}')
elif command -v shasum &>/dev/null; then
    CURRENT_HASH=$(shasum -a 256 "$REQUIREMENTS" | awk '{print $1}')
else
    echo "[install] WARNING: No SHA-256 tool found; stamp check skipped."
    CURRENT_HASH=""
fi

if [[ -f "$STAMP" ]]; then
    STORED_HASH=$(cat "$STAMP")
    if [[ -n "$CURRENT_HASH" && "$CURRENT_HASH" == "$STORED_HASH" ]]; then
        echo "[install] Already installed (requirements unchanged). Skipping."
        exit 0
    fi
    echo "[install] requirements.txt has changed. Reinstalling..."
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
echo "$CURRENT_HASH" > "$STAMP"
echo ""
echo "[install] Installation complete."