#!/usr/bin/env bash

# Generate Python API reference markdown for mavsdk and aiomavsdk.
#
# Usage: py/tools/generate_docs.sh [--overwrite]
#
# Without --overwrite, docs are generated into a temp directory for inspection.
# With --overwrite, they are written directly into docs/en/python/api_reference/.

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="${SCRIPT_DIR}/../.."
PY_ROOT="${REPO_ROOT}/py"
DOCS_API_REF="${REPO_ROOT}/docs/en/python/api_reference"

overwrite=false
for arg in "$@"; do
    case "$arg" in
        --overwrite) overwrite=true ;;
    esac
done

if [ "$overwrite" = true ]; then
    out_dir="${DOCS_API_REF}"
else
    out_dir="$(mktemp -d)/python_api_reference"
    echo "Dry run — writing to ${out_dir}"
fi

python3 "${SCRIPT_DIR}/generate_markdown.py" \
    --py-root "${PY_ROOT}" \
    --out-root "${out_dir}"

echo "Python API reference written to: ${out_dir}"
