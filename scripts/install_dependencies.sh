#!/usr/bin/env bash
set -euo pipefail

run_with_sudo() {
  if command -v sudo >/dev/null 2>&1 && [ "${EUID}" -ne 0 ]; then
    sudo "$@"
  else
    "$@"
  fi
}

run_with_sudo apt-get update
run_with_sudo apt-get install -y \
  build-essential \
  mesa-common-dev \
  libglu1-mesa-dev \
  libgl1-mesa-dev \
  freeglut3-dev \
  libglew-dev \
  libdevil-dev \
  cmake \
  clang-tidy \
  clang-format \
  cppcheck \
  iwyu # enforces include hygiene
