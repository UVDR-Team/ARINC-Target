# Target dependency report

## Final architecture

```text
Workbench managed VxWorks DKM
  ARINC 615A THA and C entry points
    ARINC 615A protocol and FIND
      target-only TFTP client/server
      required ARINC 665 binary file formats
      local checksum/hash and utility sources
      Boost 1.88 headers
        Asio, Hash2, Property Tree, CRC, Exception and header utilities
    VxWorks C++ runtime, POSIX facilities, IPv4 UDP and mounted filesystem
```

The repository produces four archives in the optional CMake route because the
actual link graph contains four independently useful source libraries:
`arinc_615a_tha_target`, `arinc_615a`, `arinc_665`, and `tftp`. The Workbench
handoff compiles the same 128 production translation units directly into one
managed DKM project. The library count was derived from the dependency graph,
not treated as a requirement.

## Retained and why

- ARINC 665 binary core: upload load-header parsing, file metadata, length and
  integrity data. Desktop XML/media-manager code is not included.
- TFTP: ARINC 615A protocol and data-file transport over UDP.
- Local `arinc_support`: byte encoding, descriptions, exceptions and a small
  synchronous logger; replaces the external Helper package.
- Local `arinc_checksum`: CRC and hash implementation required by ARINC 615A/665.
  Historical `Arinc645*` type names remain for algorithm compatibility, but
  there is no external ARINC 645 library or runtime module.
- Boost 1.88 headers: Asio networking, Property Tree JSON, Hash2 hashes, CRC,
  Exception and utility headers are used throughout the production graph.
  Removing them would require a high-risk protocol rewrite. No compiled Boost
  library is linked and Program_options is disabled.
- VxWorks platform facilities: compiler/C++ runtime, DKM constructor support,
  pthread/semaphore/time/select APIs, IPv4 UDP, and the configured filesystem.
  These come from the matching office SDK/VSB/VIP and target image.

## Removed or isolated from the target

- Qt and GUI code: only desktop applications used them; zero target source,
  include, link or runtime dependency.
- Git/GitHub/libgit: source-control metadata only; target configuration and
  build do not execute Git or access a repository/network.
- External Helper: target-used primitives were reduced to local source headers.
- ARINC 649 and ARINC 429: no required ARINC 615A/665/TFTP production path.
- External ARINC 645: only required algorithms are local source; no library.
- Boost.Program_options and Commands: desktop CLI only, disabled for target.
- spdlog and fmt: replaced by the local synchronous C++ logger.
- libxml++ and ARINC 665 XML/media-manager features: desktop-only and outside
  the target's binary load-file requirement.
- Host tests, Boost.Test, Python runner and CMake tools: excluded from native
  Workbench production sources. They remain in the handoff only as evidence and
  optional host verification inputs.

`cmake/AuditTarget.cmake` fails the host/CMake target audit if forbidden source,
headers or symbols re-enter the production build. `SOURCES.txt` in the handoff
is the authoritative Workbench production file list.
