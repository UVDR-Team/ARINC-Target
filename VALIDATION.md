# Validation record

Date: 23 September 2026 (Asia/Kolkata)

## Result and scope

The target-only source graph builds successfully on the available macOS host
with AppleClang 17 in C++20 mode. The independently implemented loopback peer
exercises real UDP/TFTP packets and all four ARINC 615A target operations. A
relocated copy of the office handoff also builds and passes with AddressSanitizer
and UndefinedBehaviorSanitizer enabled.

This is strong host-side evidence, but it is **not VxWorks 26.03 validation**.
The licensed Wind River compiler, exact VSB/BSP, LS1028ARDB target image and
board were not available. A Workbench compile, DKM link/load, on-board network
test, stop/restart and unload test remain mandatory. No document in this project
should be interpreted as a zero-error guarantee for an untested SDK/image.

## Executed checks

- Clean root Debug configuration using only the bundled ARINC 665 source and
  SHA-verified bundled Boost 1.88 headers; no network or sibling repository.
- Clean optimized Release compilation and executable link passed. A new-libc++
  stream symbol found by this check was removed in favor of portable `ostream::write`.
- All 229 original and added regression cases passed (1,613 assertions).
- C entry/self-test passed (FIND and initialization codecs plus SHA256 vector).
- Independent Python UDP/TFTP integration suite passed:
  FIND; Information and LCL; Upload with ARINC 665 header and a 4,097-byte file;
  Media Defined Download; Operator Defined Download listing/selection; clean stop.
- Lost first TFTP DATA acknowledgement recovered through retransmission.
- Malformed FIND input was ignored and the next valid request succeeded.
- Empty, malformed and path-traversal load headers were rejected; no file was
  written outside the configured upload directory and the server stayed alive.
- Corrupt and truncated uploads were rejected by stored size/CRC validation;
  a subsequent valid upload completed.
- Runtime start/stop/restart was tested for three cycles; a concurrent second
  runtime and unknown/disabled operations were rejected.
- Full test suite repeated ten consecutive times: all 30 CTest executions passed.
- Dependency audit passed across 128 production translation units, compiler
  include dependencies and undefined symbols of all four target archives.
- The relocated handoff compiled all 128 translation units as one target using
  the portable Boost.Asio `select` backend, then passed self/network tests under
  `-fsanitize=address,undefined -fno-sanitize-recover=all`.
- `git diff --check` passed; the final archive's SHA256 and per-file checksums
  are generated during final packaging.

## Important fixes validated by the negative tests

- Upload no longer continues after rejecting an empty ARINC 665 load header.
- Invalid/traversal load filenames are rejected before filesystem access.
- Stored upload length, CRC16 and optional ARINC 665 check value are verified.
- A pending status transmission is no longer replaced while active. The old
  behavior could trigger an assertion and corrupt the operation lifecycle when
  an upload was rejected quickly.
- Finalisation cancels its timer before invoking an owner callback that may
  release the operation object.
- A missing JSON configuration returns an error instead of silently starting
  with no enabled operations.

## Final office acceptance gate

Follow `START_HERE.md` inside the delivered ZIP. Record all of these before
calling the port complete: clean Workbench Debug build; no unresolved DKM
symbols; module load; self-test return 0; FIND; Information; known upload and
both downloads; corrupt/timeout/abort handling; stop/restart; clean unload;
stack and long-run memory checks. Preserve the exact VSB/VIP/BSP identifiers,
compiler output, target image identity and transfer logs with this record.
