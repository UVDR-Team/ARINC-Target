#!/usr/bin/env bash
# SPDX-License-Identifier: MPL-2.0
set -euo pipefail
project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
sdk_dir="${WIND_SDK_HOME:-}"
toolchain="${ARINC615A_TOOLCHAIN:-${sdk_dir}/vxsdk/sysroot/mk/dkm.toolchain.cmake}"
boost_dir="${ARINC615A_BOOST_INCLUDE_DIR:-}"
build_dir="${ARINC615A_BUILD_DIR:-${project_dir}/build-vxworks}"

if [[ ! -f "$toolchain" ]]; then
  printf '%s\n' 'Set ARINC615A_TOOLCHAIN to your selected SDK DKM .cmake toolchain, then rerun.' >&2
  exit 2
fi
if [[ -z "$boost_dir" && -d "$sdk_dir" ]]; then
  candidates=()
  while IFS= read -r header; do candidates+=("${header%/boost/version.hpp}"); done < <(find "$sdk_dir" -type f -path '*/boost/version.hpp' -print)
  if [[ ${#candidates[@]} -eq 1 ]]; then
    boost_dir="${candidates[0]}"
  else
    printf '%s\n' 'Set ARINC615A_BOOST_INCLUDE_DIR to the SDK directory containing boost/version.hpp.' >&2
    if [[ ${#candidates[@]} -gt 0 ]]; then printf 'Candidate: %s\n' "${candidates[@]}" >&2; fi
    exit 2
  fi
fi
if [[ ! -f "$boost_dir/boost/version.hpp" ]]; then
  printf '%s\n' 'ARINC615A_BOOST_INCLUDE_DIR must contain boost/version.hpp.' >&2
  exit 2
fi

cmake --version
cmake -S "$project_dir" -B "$build_dir" \
  "-DCMAKE_TOOLCHAIN_FILE=$toolchain" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  "-DARINC615A_BOOST_INCLUDE_DIR=$boost_dir" \
  -DARINC_615A_VXWORKS=ON -DARINC615A_TARGET_ONLY=ON \
  -DARINC615A_BACKGROUND_THREAD=OFF -DARINC615A_BUILD_TESTS=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build "$build_dir" --parallel "${ARINC615A_BUILD_JOBS:-4}" 2>&1 | tee "$build_dir/build.log"
cmake --build "$build_dir" --target arinc615a_audit 2>&1 | tee "$build_dir/audit.log"
cmake --install "$build_dir" --prefix "$build_dir/stage"
printf '\nTarget libraries and C header: %s/stage\nBuild information: %s/BUILD_INFO.txt\n' "$build_dir" "$build_dir"
