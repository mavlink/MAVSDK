#!/usr/bin/env bash

# Generate API reference docs for all languages and copy them into docs/.
#
# Usage:
#   tools/generate_docs.sh [--overwrite] [--skip-cpp] [--skip-python]
#
# Options:
#   --overwrite    Copy generated docs into docs/en/*/api_reference/ (default: dry run only)
#   --skip-cpp     Skip C++ doc generation
#   --skip-python  Skip Python doc generation

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="${SCRIPT_DIR}/.."

overwrite=false
skip_cpp=false
skip_python=false

for arg in "$@"; do
    case "$arg" in
        --overwrite)    overwrite=true ;;
        --skip-cpp)     skip_cpp=true ;;
        --skip-python)  skip_python=true ;;
    esac
done

# ── C++ ──────────────────────────────────────────────────────────────────────
if [ "$skip_cpp" = false ]; then
    echo "==> Generating C++ API docs"
    cpp_args=()
    [ "$overwrite" = true ] && cpp_args+=(--overwrite)
    bash "${REPO_ROOT}/cpp/tools/generate_docs.sh" "${cpp_args[@]}"
fi

# ── Python (mavsdk / aiomavsdk) ──────────────────────────────────────────────
if [ "$skip_python" = false ]; then
    echo "==> Generating Python API docs"
    python_args=()
    [ "$overwrite" = true ] && python_args+=(--overwrite)
    bash "${REPO_ROOT}/py/tools/generate_docs.sh" "${python_args[@]}"
fi

echo "Done."
