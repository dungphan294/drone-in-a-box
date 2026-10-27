#!/usr/bin/env bash
# Build and unit-test this firmware in Docker, with no ESP-IDF on the host.
#
# Why: the toolchain is a 2 GB install pinned to one IDF version, and "it
# builds on my machine" has already cost this project once. The container is
# the same on a Mac, on a Linux box and in CI, so a green build here means the
# same thing everywhere.
#
#   tools/docker-build.sh build      # idf.py build
#   tools/docker-build.sh test       # host unit tests (command + auth suites)
#   tools/docker-build.sh check      # test, then build -- what CI should run
#   tools/docker-build.sh menuconfig # interactive sdkconfig editor
#   tools/docker-build.sh shell      # a prompt inside the container, IDF ready
#   tools/docker-build.sh clean      # remove build/ and build-host/
#   tools/docker-build.sh size       # idf.py size, after a build
#
# FLASHING IS NOT HERE, and that is not an omission. Docker Desktop on macOS
# and Windows cannot pass a USB serial device into a container, so `idf.py
# flash` and `monitor` have to run on the host. Build here, flash there:
#
#   tools/docker-build.sh build
#   pip install esptool                      # once
#   esptool.py -p /dev/cu.usbmodem* write_flash @build/flash_args
#   # or just:  idf.py -p /dev/cu.usbmodem* flash monitor   (host IDF install)
#
# On Linux, add --device=/dev/ttyUSB0 to DOCKER_RUN_ARGS and flashing works.
set -euo pipefail

# Pinned, not :latest. The README targets ESP-IDF v6.0.2 and the duty
# arithmetic, the LEDC resolution ladder in actuator.c and dependencies.lock
# are all written against it. Override deliberately:
#   IDF_VERSION=v6.0.3 tools/docker-build.sh build
IDF_VERSION=${IDF_VERSION:-v6.0.2}
IMAGE="espressif/idf:${IDF_VERSION}"

PROJECT_DIR=$(cd "$(dirname "$0")/.." && pwd)

# The component manager downloads managed_components (espressif/mdns) on the
# first build. Cache it in a named volume so later runs are offline-fast and a
# flaky network does not fail a rebuild.
CACHE_VOLUME="idf-cache-${IDF_VERSION}"

# Run as the invoking user, or every file the build writes comes back owned by
# root and the next host-side `git status` is a mess. The image's entrypoint
# needs a writable HOME, which an unknown uid does not get, so point it at the
# cache volume.
DOCKER_RUN_ARGS=(
    --rm
    --user "$(id -u):$(id -g)"
    --env HOME=/cache
    --volume "${PROJECT_DIR}:/project"
    --volume "${CACHE_VOLUME}:/cache"
    --workdir /project
)
# An `if`, not `[ -t 0 ] && ...`: that form is the whole statement, so under
# `set -e` a non-tty stdin (any CI runner, any pipe) would exit 1 here before
# doing a scrap of work.
if [ -t 0 ]; then
    DOCKER_RUN_ARGS+=(--interactive --tty)
fi

# A fresh named volume is root-owned, and we run as the invoking user, so HOME
# would be read-only and the component-manager cache could not be written.
# Chown it once, as root, and leave a marker: -R over a populated cache on
# every run would be slow for no reason.
prepare_cache() {
    docker volume create "$CACHE_VOLUME" >/dev/null
    docker run --rm --user 0:0 \
        --volume "${CACHE_VOLUME}:/cache" "$IMAGE" \
        bash -c "test -e /cache/.owned || { chown -R $(id -u):$(id -g) /cache && touch /cache/.owned; }"
}

run_in_container() {
    prepare_cache
    docker run "${DOCKER_RUN_ARGS[@]}" "$IMAGE" bash -lc "$1"
}

# The host test suite is a plain CMake project that borrows Unity from the IDF
# checkout, which is why it needs IDF_PATH even though it compiles no ESP-IDF
# code and runs on the build machine rather than the chip.
HOST_TESTS='
set -e
cmake -S test/host -B build-host >/dev/null
cmake --build build-host -j"$(nproc)"
ctest --test-dir build-host --output-on-failure
'

case "${1:-check}" in
    build)      run_in_container "idf.py build" ;;
    test)       run_in_container "$HOST_TESTS" ;;
    check)      run_in_container "$HOST_TESTS"; run_in_container "idf.py build" ;;
    menuconfig) run_in_container "idf.py menuconfig" ;;
    size)       run_in_container "idf.py size" ;;
    shell)      run_in_container "exec bash" ;;
    clean)      rm -rf "${PROJECT_DIR}/build" "${PROJECT_DIR}/build-host"
                echo "removed build/ and build-host/" ;;
    *)          # The leading comment block is the usage text. Printing a line
                # range here drifts every time the header is edited.
                awk 'NR>1 && /^#/ { sub(/^# ?/, ""); print; next } NR>1 { exit }' "$0"
                exit 2 ;;
esac
