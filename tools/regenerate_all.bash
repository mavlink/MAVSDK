#!/usr/bin/env bash

# Regenerates everything CI checks for: code for all languages from the protos,
# C++ and Kotlin style, and the C++ API reference docs.
#
# If any required tool is missing locally, it re-runs itself in the CI container
# using docker or podman.

set -euo pipefail

usage() {
    cat << EOF
Usage: tools/regenerate_all.bash [options]

    -h, --help               Show help
    -b, --build-dir DIR      C++ build dir holding protoc/grpc_cpp_plugin in third_party/install
                             (default: cpp/build/default, configured if protoc is missing)
        --skip-docs          Don't regenerate the C++ API docs
        --docs-only          Only regenerate the C++ API docs
        --container          Always run in the container
        --no-container       Never use the container, fail if tools are missing
        --no-pull            Don't pull the container image first

Environment:
    DOCKER_IMAGE             Container image (default: ${default_image})
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"

default_image="docker.io/mavsdk/mavsdk-ubuntu-26.04:latest"
image="${DOCKER_IMAGE:-${default_image}}"
in_container="${MAVSDK_REGENERATE_IN_CONTAINER:-0}"

clang_format_major=21
java_min_major=21
doxygen_version="1.15.0"

do_code=true
do_docs=true
container_mode="auto"
pull=true
build_dir=""
build_dir_arg_given=""
forward_args=()

while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -b|--build-dir)
            [ $# -ge 2 ] || { echo "Error: $1 needs an argument" >&2; exit 1; }
            build_dir="$(realpath -m "$2")"
            build_dir_arg_given=1
            shift
            ;;
        --skip-docs)
            do_docs=false
            forward_args+=("$1")
            ;;
        --docs-only)
            do_code=false
            forward_args+=("$1")
            ;;
        --container)
            container_mode="always"
            ;;
        --no-container)
            container_mode="never"
            ;;
        --no-pull)
            pull=false
            ;;
        *)
            echo "Error: unknown option: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
    shift
done

if [ "${do_code}" = false ] && [ "${do_docs}" = false ]; then
    echo "Error: --skip-docs and --docs-only together leave nothing to do" >&2
    exit 1
fi

# Container builds go into their own dirs: binaries built there don't run on the
# host, and CMake caches don't survive a change of compiler.
if [ "${in_container}" = "1" ]; then
    build_dir="${repo_root}/cpp/build/container-default"
    docs_build_dir="${repo_root}/cpp/build/container-docs"
    venv_dir="/cache/venv"
else
    build_dir="${build_dir:-${repo_root}/cpp/build/default}"
    docs_build_dir="${repo_root}/cpp/build-docs"
    venv_dir="${repo_root}/venv"
fi

protoc_binary="${build_dir}/third_party/install/bin/protoc"
grpc_plugin_binary="${build_dir}/third_party/install/bin/grpc_cpp_plugin"

have_protoc() {
    [ -x "${protoc_binary}" ] && [ -x "${grpc_plugin_binary}" ]
}

missing=()

need_command() {
    command -v "$1" > /dev/null 2>&1 || missing+=("$1")
}

find_clang_format() {
    local candidate
    for candidate in "clang-format-${clang_format_major}" clang-format; do
        if command -v "${candidate}" > /dev/null 2>&1 \
            && "${candidate}" --version | grep -qE "version ${clang_format_major}\."; then
            return 0
        fi
    done
    return 1
}

java_major() {
    # e.g. 'openjdk version "21.0.5" 2024-10-15' -> 21
    java -version 2>&1 | head -n 1 | sed -E 's/.*version "([0-9]+).*/\1/'
}

check_tools() {
    need_command git
    need_command python3

    if { [ "${do_code}" = true ] && ! have_protoc; } || [ "${do_docs}" = true ]; then
        need_command cmake
        need_command make
        need_command c++
    fi

    if [ "${do_code}" = true ]; then
        if ! python3 -c 'import ensurepip, venv' > /dev/null 2>&1; then
            missing+=("python3-venv")
        fi
        if ! find_clang_format; then
            missing+=("clang-format ${clang_format_major}")
        fi
        if ! command -v java > /dev/null 2>&1 || [ "$(java_major)" -lt "${java_min_major}" ] 2> /dev/null; then
            missing+=("JDK >= ${java_min_major}")
        fi
    fi

    if [ "${do_docs}" = true ]; then
        if ! command -v doxygen > /dev/null 2>&1 || [ "$(doxygen --version)" != "${doxygen_version}" ]; then
            missing+=("doxygen ${doxygen_version}")
        fi
    fi
}

warn_if_dockerfile_changed() {
    # Only latest gets pulled, which is built from the Dockerfile on main.
    [ "${image}" = "${default_image}" ] || return 0
    git -C "${repo_root}" rev-parse --verify -q origin/main > /dev/null || return 0

    local base
    base="$(git -C "${repo_root}" merge-base HEAD origin/main)" || return 0
    if ! git -C "${repo_root}" diff --quiet "${base}" -- docker/Dockerfile-ubuntu-26.04; then
        echo "Warning: docker/Dockerfile-ubuntu-26.04 differs from origin/main," >&2
        echo "         but ${image} is built from main. Results may differ from CI." >&2
    fi
}

run_in_container() {
    local runtime
    runtime="$(command -v docker || command -v podman || true)"
    if [ -z "${runtime}" ]; then
        echo "Error: neither docker nor podman found to use as a fallback." >&2
        exit 1
    fi

    if [ "${pull}" = true ]; then
        echo "Pulling ${image}..."
        if ! "${runtime}" pull "${image}"; then
            if "${runtime}" image inspect "${image}" > /dev/null 2>&1; then
                echo "Warning: pull failed, using the local copy of ${image}." >&2
            else
                echo "Error: could not pull ${image}." >&2
                exit 1
            fi
        fi
    fi

    warn_if_dockerfile_changed

    if [ -n "${build_dir_arg_given}" ]; then
        echo "Note: --build-dir is ignored in the container." >&2
    fi

    # Holds HOME, the venv and the Gradle cache across runs.
    local cache_dir="${XDG_CACHE_HOME:-${HOME}/.cache}/mavsdk-regenerate"
    mkdir -p "${cache_dir}/home" "${cache_dir}/gradle"

    # Mounted at the same path as on the host, so paths in output match.
    local args=(run --rm --init
        -v "${repo_root}:${repo_root}:z"
        -v "${cache_dir}:/cache:z"
        -w "${repo_root}"
        -e MAVSDK_REGENERATE_IN_CONTAINER=1
        -e HOME=/cache/home
        -e GRADLE_USER_HOME=/cache/gradle)

    # In a worktree or with a separate git dir, the repo's .git lives elsewhere.
    local git_common_dir
    git_common_dir="$(cd "${repo_root}" && realpath "$(git rev-parse --git-common-dir)")"
    case "${git_common_dir}" in
        "${repo_root}"/*) ;;
        *) args+=(-v "${git_common_dir}:${git_common_dir}:z") ;;
    esac

    if [ "$(basename "${runtime}")" = "podman" ]; then
        args+=(--userns=keep-id)
    else
        args+=(-u "$(id -u):$(id -g)")
    fi

    if [ -t 0 ] && [ -t 1 ]; then
        args+=(-it)
    fi

    echo "Running in container using $(basename "${runtime}")..."
    exec "${runtime}" "${args[@]}" "${image}" tools/regenerate_all.bash "${forward_args[@]}"
}

if [ "${container_mode}" = "always" ] && [ "${in_container}" != "1" ]; then
    run_in_container
fi

check_tools

if [ ${#missing[@]} -gt 0 ]; then
    echo "Missing tools:"
    printf '    - %s\n' "${missing[@]}"
    echo

    if [ "${in_container}" = "1" ]; then
        echo "Error: the container image ${image} is missing tools." >&2
        exit 1
    fi
    if [ "${container_mode}" = "never" ]; then
        echo "Error: install them, or drop --no-container to use docker/podman." >&2
        exit 1
    fi
    run_in_container
fi

cd "${repo_root}"

if [ "${do_code}" = true ]; then
    if [ ! -x "${venv_dir}/bin/python3" ] || ! "${venv_dir}/bin/python3" -c '' 2> /dev/null; then
        echo "Creating virtual environment in ${venv_dir}..."
        rm -rf "${venv_dir}"
        python3 -m venv "${venv_dir}"
    fi
    # shellcheck disable=SC1091
    source "${venv_dir}/bin/activate"

    if ! have_protoc; then
        echo "--------------------------------------"
        echo "Building protoc and grpc_cpp_plugin in ${build_dir}"
        echo "--------------------------------------"
        cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_MAVSDK_SERVER=ON -B"${build_dir}" -S cpp
    fi

    echo "--------------------------------------"
    echo "Generating code from protos"
    echo "--------------------------------------"
    PATH="${build_dir}/third_party/install/bin:${PATH}" \
        tools/generate_from_protos.bash --build-dir "${build_dir}"

    echo "--------------------------------------"
    echo "Fixing C++ style"
    echo "--------------------------------------"
    # Exits 1 when it changed files, which is expected here.
    (cd cpp && python3 tools/fix_style.py --quiet .) || true

    echo "--------------------------------------"
    echo "Fixing Kotlin style"
    echo "--------------------------------------"
    gradle_args=(ktfmtFormat --console=plain)
    if [ "${in_container}" = "1" ]; then
        gradle_args+=(--no-daemon)
    fi
    (cd kt/mavsdk-kotlin && ./gradlew "${gradle_args[@]}")
fi

if [ "${do_docs}" = true ]; then
    echo "--------------------------------------"
    echo "Generating C++ API docs"
    echo "--------------------------------------"
    (cd cpp && tools/generate_docs.sh --overwrite --build-dir "${docs_build_dir}")
fi

echo
echo "Done. Check the result with 'git status'."
