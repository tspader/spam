#!/usr/bin/env bash
# Regenerate gen/ from xcbproto. libxcb's c_client.py is python (xcbgen), which
# can't run inside an spn build script, so the outputs are vendored and this
# script is run by hand when bumping the pin. Keep the commits in sync with
# [package].version in spn.toml.
set -euo pipefail

LIBXCB_URL="https://gitlab.freedesktop.org/xorg/lib/libxcb.git"
LIBXCB_COMMIT="622152ee42a310876f10602601206954b8d0613e" # libxcb-1.17.0
XCBPROTO_URL="https://gitlab.freedesktop.org/xorg/proto/xcbproto.git"
XCBPROTO_COMMIT="77d7fc04da729ddc5ed4aacf30253726fac24dca" # xcb-proto-1.17.0
# The core protocol plus every extension libxcb's configure enables by default;
# ge, xevie, xprint and xselinux are off upstream.
PROTOCOLS="xproto bigreq xc_misc composite damage dbe dpms dri2 dri3 glx present randr record render res screensaver shape shm sync xf86dri xfixes xinerama xinput xkb xtest xv xvmc"

ROOT="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

fetch() {
  git init -q "$2"
  git -C "$2" fetch -q --depth 1 "$1" "$3"
  git -C "$2" checkout -q FETCH_HEAD
}

fetch "$LIBXCB_URL" "$WORK/libxcb" "$LIBXCB_COMMIT"
fetch "$XCBPROTO_URL" "$WORK/xcbproto" "$XCBPROTO_COMMIT"

mkdir -p "$WORK/gen"
cd "$WORK/gen"
for protocol in $PROTOCOLS; do
  python3 "$WORK/libxcb/src/c_client.py" -c "libxcb 1.17.0" -l "X Version 11" -s 3 \
    -p "$WORK/xcbproto" "$WORK/xcbproto/src/$protocol.xml"
done

mkdir -p "$ROOT/gen"
rm -f "$ROOT/gen/"*.h
cp "$WORK/libxcb/src/xcb.h" "$WORK/libxcb/src/xcbext.h" "$WORK/gen/"*.h "$ROOT/gen/"
echo "regenerated $(ls "$ROOT/gen" | wc -l) files in $ROOT/gen"
