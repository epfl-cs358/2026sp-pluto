#!/usr/bin/env bash
# =============================================================================
# run.sh — Install dependencies (if needed) and launch the robot controller.
#
# Usage:
#   ./run.sh [OPTIONS]
#
# Options:
#   --reinstall   Delete the install stamp and re-run the full installation
#   --help        Show this help message
#   --port <PORT> Choose the port to use for running the application
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STAMP="$SCRIPT_DIR/scripts/.installed"

# Add new flags in the case block below. Each flag should set a variable and
# shift, then the corresponding logic lives in the "Apply parsed options"
# section further down.

OPT_REINSTALL=0
OPT_PORT=8090

usage() {
    echo "Usage: $(basename "$0") [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --reinstall     Remove the install stamp and reinstall all dependencies"
    echo "  --port <port>   Port to run the web UI on (default: 8080)"
    echo "  --help          Show this help message"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --reinstall)
            OPT_REINSTALL=1
            shift
            ;;
        --port)
            if [[ -z "${2-}" ]]; then
                echo "[run] --port requires a value" >&2
                exit 1
            fi
            OPT_PORT="$2"
            shift 2
            ;;
        --help | -h)
            usage
            exit 0
            ;;
        # Add new options here:
        *)
            echo "[run] Unknown argument: $1" >&2
            echo ""
            usage
            exit 1
            ;;
    esac
done

if [[ "$OPT_REINSTALL" -eq 1 ]]; then
    if [[ -f "$STAMP" ]]; then
        echo "[run] --reinstall: removing install stamp..."
        rm -f "$STAMP"
    else
        echo "[run] --reinstall: stamp not found, will run fresh install."
    fi
fi

bash "$SCRIPT_DIR/scripts/install.sh"

echo "[run] Activating virtual environment..."
source "$SCRIPT_DIR/.venv/bin/activate"

echo "[run] Starting robot controller on port $OPT_PORT..."
python "$SCRIPT_DIR/src/control/main.py" --port "$OPT_PORT"