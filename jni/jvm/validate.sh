#!/bin/bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
library="${1:-"$script_dir/../build/libmavsdk_jni.dylib"}"

"$script_dir/compile.sh"
python3 "$script_dir/validate_generated.py" --library "$library"
