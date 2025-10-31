#!/usr/bin/env bash
set -euo pipefail

# Run clang-tidy and cppcheck with a generated compile_commands.json.
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"

# Configure the build tree and (re)generate compile_commands.json used by the linters.
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Static analysis with clang-tidy; uses the compile database for accurate diagnostics.
clang-tidy "${REPO_ROOT}/main.cpp" -p "${BUILD_DIR}"

# Cross-check the same sources with cppcheck for complementary warnings.
cppcheck --project="${BUILD_DIR}/compile_commands.json" --enable=warning,style,performance --error-exitcode=1
