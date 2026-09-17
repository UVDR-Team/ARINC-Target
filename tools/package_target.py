#!/usr/bin/env python3
"""Package the audited source manifest and its local include closure, without desktop projects."""
import argparse
import re
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INCLUDE_ROOTS = [ROOT / name for name in (
    "compat", "helper/lib", "arinc-649/lib", "arinc_665/lib", "tftp/lib", "lib",
    "app/arinc_615a_test_tha")]
INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"\n]+)[>"]', re.MULTILINE)


def collect():
    manifest = ROOT / "cmake/TargetSources.cmake"
    paths = re.findall(r'\$\{PROJECT_SOURCE_DIR\}/([^\s)]+)', manifest.read_text())
    pending = [ROOT / path for path in paths]
    pending += [ROOT / "compat/vxworks_platform.hpp"]
    for module in ("helper/lib/helper", "arinc-649/lib/arinc_649", "arinc_665/lib/arinc_665", "tftp/lib/tftp", "lib/arinc_615a"):
        pending.append(ROOT / module / "Version.hpp.in")
    selected = set()
    while pending:
        path = pending.pop().resolve()
        if path in selected:
            continue
        path.relative_to(ROOT)
        if not path.is_file():
            raise RuntimeError(f"Missing target source: {path}")
        selected.add(path)
        for header in INCLUDE.findall(path.read_text()):
            for base in [path.parent, *INCLUDE_ROOTS]:
                candidate = base / header
                if candidate.is_file():
                    pending.append(candidate)
                    break
            else:
                # SDK/system and CMake-generated headers are intentionally external.
                if header.startswith(("helper/", "arinc_649/", "arinc_665/", "arinc_615a/", "tftp/", "fmt/", "spdlog/")):
                    if not (header.endswith("_export.h") or header.endswith("/Version.hpp")):
                        raise RuntimeError(f"Unresolved project include {header} in {path}")
    for name in ("cmake/TargetSources.cmake", "cmake/TargetBuild.cmake", "cmake/AuditTarget.cmake",
                 "tools/build_vxworks.sh", "README.md", "OFFLINE_VXWORKS_WORKBENCH.md", "VxWorksDKM.md",
                 "VxWorksDKMDependencies.cmake.example",
                 "DEPENDENCY_AUDIT.md", "LICENSE",
                 "app/arinc_615a_test_tha/arinc_615a_test_tha.json.template", "examples/vxworks_entry.c"):
        selected.add(ROOT / name)
    for name in ("helper", "arinc-649", "arinc_665", "tftp"):
        selected.add(ROOT / name / "LICENSE")
    return selected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/arinc615a-target-source.zip")
    args = parser.parse_args()
    files = collect()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    cmake = '''cmake_minimum_required(VERSION 3.24)
project(arinc_615a LANGUAGES C CXX)
option(ARINC615A_TARGET_ONLY "Build only the target source graph" ON)
option(ARINC_615A_VXWORKS "Use VxWorks DKM defaults" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
  set(ARINC_615A_VXWORKS ON)
endif()
if(ARINC615A_BUILD_TESTS)
  message(FATAL_ERROR "Use the full Git checkout for regression tests; this package contains target sources only")
endif()
include(cmake/TargetBuild.cmake)
'''
    with zipfile.ZipFile(args.output, "w", zipfile.ZIP_DEFLATED) as archive:
        archive.writestr("arinc615a-target/CMakeLists.txt", cmake)
        for path in sorted(files):
            archive.write(path, "arinc615a-target/" + path.relative_to(ROOT).as_posix())
    print(f"Packaged {len(files) + 1} files: {args.output}")


if __name__ == "__main__":
    main()
