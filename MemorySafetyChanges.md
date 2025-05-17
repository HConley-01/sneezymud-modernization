# Memory Safety Improvements in SneezyMUD

This document details all changes made to improve memory safety, buffer handling, and string operations in the SneezyMUD codebase during the recent refactor.

## Overview

The following classes of issues were addressed:
- Replacement of unsafe string functions (`strcpy`, `strcat`) with their safer counterparts (`strncpy`, `strncat`), including proper buffer size management and null-termination.
- Ensuring all buffer operations use the correct types (`char[]` vs. `sstring`) and do not mix C++ and C string APIs incorrectly.
- Fixing format string mismatches in `sprintf`/`snprintf` calls.
- Adding or correcting explicit null-termination after buffer operations where needed.
- Shoring up function logic to avoid buffer overflows and off-by-one errors.

## File-by-File Summary

### `actions.cc`
- Replaced all `strcpy` with `strncpy` and ensured explicit null-termination after copying arguments to local buffers.

### `range.cc`
- Replaced all `strcpy` with `strncpy` and ensured explicit null-termination after copying arguments to local buffers.

### `player_data.cc`
- Replaced all `strcpy` with `strncpy` and ensured explicit null-termination for all player/account string fields.
- Ensured all string buffer assignments are size-safe.

### `cmd_low.cc`
- Replaced all `strcat` with `strncat` and ensured buffer size is not exceeded.
- Fixed format string mismatches in `sprintf`/`snprintf` calls (e.g., `%d` vs `%s`).
- Ensured all string formatting operations match the argument types.

### `cmd_set.cc`, `cmd_show.cc`, `cmd_zones.cc`, `obj_book.cc`, `comm.cc`
- Replaced all `strcat` with `strncat` and ensured buffer size is not exceeded.
- Used `.c_str()` when appending `sstring` to C buffers, and used `+=` for `sstring` concatenation.
- Ensured all buffer operations are type-correct and null-terminated.

### `loadset.cc`
- Fixed misleading-indentation warnings by adding braces to ambiguous `if` statements.
- Ensured all buffer operations are safe and null-terminated.

## General Best Practices Applied
- All buffer copies and concatenations now use the buffer size minus one for null-termination.
- All string buffers are explicitly null-terminated after copy/concat operations.
- All format string operations (`sprintf`, `snprintf`) have been checked for type correctness.
- All ambiguous or potentially unsafe memory operations have been replaced with safe, bounded alternatives.

## Next Steps
- Continue to audit any remaining legacy code for unsafe memory operations.
- Consider using C++ string classes (`std::string` or `sstring`) throughout for even greater safety.
- Add unit tests for edge cases involving string and buffer handling.

---

**Last updated:** May 16, 2025
