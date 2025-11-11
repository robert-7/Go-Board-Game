#!/usr/bin/env bash
set -euo pipefail

# Run clang-tidy and cppcheck with a generated compile_commands.json.
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"

# Configure the build tree and (re)generate compile_commands.json used by the linters.
# Enable comprehensive warnings so clang diagnostics (unused constants, etc.) surface during analysis.
cmake -S "${REPO_ROOT}" \
	-B "${BUILD_DIR}" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic"

# Find all C++ source files, excluding the build directory, and pass them to the linters.
mapfile -t SOURCE_FILES < <(find "${REPO_ROOT}" -path "${BUILD_DIR}" -prune -o -type f -name "*.cpp" -print)

if [[ ${#SOURCE_FILES[@]} -eq 0 ]]; then
	echo "No C++ source files found for linting." >&2
	exit 0
fi

# Style check with clang-format; fails if formatting differs from .clang-format.
echo "Checking formatting with clang-format..."
clang-format --dry-run --Werror "${SOURCE_FILES[@]}"

# Static analysis with clang-tidy; uses the compile database for accurate diagnostics.
echo "Running static analysis with clang-tidy..."
clang-tidy -p "${BUILD_DIR}" "${SOURCE_FILES[@]}"

# Include-What-You-Use to enforce proper includes; uses the compile database via the helper tool.
echo "Checking includes with iwyu_tool..."
# The iwyu_tool script doesn't directly accept multiple files, so we loop.
for file in "${SOURCE_FILES[@]}"; do
    iwyu_tool -p "${BUILD_DIR}" "$file"
done

# Cross-check the same sources with cppcheck for complementary warnings.
# ignore normalCheckLevelMaxBranches to reduce noise
cppcheck --project="${BUILD_DIR}/compile_commands.json" \
	--enable=warning,style,performance \
	--error-exitcode=1 \
	--suppress=normalCheckLevelMaxBranches \
	--suppress="*:${BUILD_DIR}/_deps/*" \
	--suppress="*:${BUILD_DIR}/CMakeFiles/*"
