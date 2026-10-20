# Sourced by the generate_from_protos scripts.
#
# run_parallel FUNCTION ITEM...
#
# Runs FUNCTION once per ITEM, up to MAVSDK_GENERATE_JOBS (default: number of
# CPUs) at a time. Output of each run is buffered and printed in ITEM order.
# Returns 1 if any run failed.
run_parallel() {
    local func="$1"
    shift

    local max_jobs="${MAVSDK_GENERATE_JOBS:-$(nproc)}"
    local log_dir
    log_dir="$(mktemp -d)"

    local pids=()
    local started=0
    local collected=0
    local failed=0

    # Waits for the oldest run, so output stays in order.
    collect_oldest() {
        if ! wait "${pids[${collected}]}"; then
            failed=1
        fi
        cat "${log_dir}/${collected}.log"
        collected=$((collected + 1))
    }

    local item
    for item in "$@"; do
        "${func}" "${item}" > "${log_dir}/${started}.log" 2>&1 &
        pids+=("$!")
        started=$((started + 1))
        if [ $((started - collected)) -ge "${max_jobs}" ]; then
            collect_oldest
        fi
    done

    while [ "${collected}" -lt "${started}" ]; do
        collect_oldest
    done

    rm -rf "${log_dir}"

    if [ "${failed}" -ne 0 ]; then
        echo "Error: generation failed, see output above." >&2
        return 1
    fi
}
