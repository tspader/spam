#!/usr/bin/env bash
set -euo pipefail

LUAJIT_URL="https://github.com/LuaJIT/LuaJIT.git"
LUAJIT_COMMIT="b925b3e3fc6771171602323b45fbe9fb8fc90369"

ROOT="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

git init -q "$WORK/luajit"
git -C "$WORK/luajit" fetch -q "$LUAJIT_URL" "$LUAJIT_COMMIT"
git -C "$WORK/luajit" checkout -q FETCH_HEAD

export MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-11.0}"
make -C "$WORK/luajit" -j"$(getconf _NPROCESSORS_ONLN)" CCDEBUG=

arch="$(uname -m)"; [ "$arch" = "arm64" ] && arch="aarch64"
os="$(uname -s | tr '[:upper:]' '[:lower:]')"; [ "$os" = "darwin" ] && os="macos"
target="$arch-$os"
mkdir -p "$ROOT/gen/$target"
cp "$WORK/luajit/src/luajit.h" "$ROOT/gen/luajit.h"
for f in lj_bcdef.h lj_ffdef.h lj_libdef.h lj_recdef.h lj_folddef.h lj_vm.S; do
  cp "$WORK/luajit/src/$f" "$ROOT/gen/$target/$f"
done
