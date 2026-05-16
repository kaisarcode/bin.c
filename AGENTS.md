# bin.c — Blueprint / Template C CLI

## Overview

`bin.c` is the canonical blueprint for all `kcbin` standalone C CLI projects. It defines the standard project layout, build system, CLI argument parsing pattern, test harness, and error-handling conventions that every new kcbin project should copy as a starting point. Rather than a fully functional tool, it demonstrates the composable-native-tool architecture: small, single-file C programs with cross-compilation support, POSIX conventions, and a file-based output model under `bin/{arch}/{platform}/`.

As the reference template, every structural decision in `bin.c` — from the `kc_` function prefix and argument-scanning loop to the CMake options and shell-based test functions — encodes a deliberate convention. New projects are expected to preserve the skeleton (Makefile, CMakeLists.txt, test.sh, `.kcsignore` patterns) while replacing the application logic in `src/bin.c`.

## Architecture

Single-file C99/C11 program with a flat argument parser that scans for an optional verb (`"run"`), an optional positional text argument, then a flag loop. If no positional text is provided, `kc_bin_read_stdin` reads from stdin into a dynamically grown buffer. The build system is Makefile-driven with CMake/Ninja as the underlying builder, supporting 16 architecture/platform targets via cross-compilation. Testing is a standalone shell script that locates the built binary via arch/platform detection and exercises help, error, and success paths.

## Directory Layout

| Path | Contents |
|------|----------|
| `AGENTS.md` | Workspace agent instructions (this file) |
| `CMakeLists.txt` | CMake build definition (C11, compile flags, `BIN_NATIVE` option) |
| `LICENSE` | GPLv3 license text |
| `Makefile` | Top-level build orchestration (native/all/cross-compile targets, CMake wrappers) |
| `README.md` | Project documentation (CLI reference, build instructions, multiarch table) |
| `src/bin.c` | Single-source C program (main entry point, argument parsing, stdin reader) |
| `test.sh` | Shell-based test harness (arch/platform detection, binary path resolution, validation suite) |
| `.gitattributes` | Git line-ending normalization (`* text=auto`) |
| `.gitignore` | Git ignore rules (`.build/`, `bin/`) |
| `.kcsignore` | KCS validation ignore list (`.build`, `AGENTS.md`) |
| `bin/{arch}/{platform}/bin` | Build output directories (one per cross-compile target) |

## CLI Conventions

| Form | Description |
|------|-------------|
| `bin [run] <text>` | Optional `"run"` verb followed by positional text arg |
| `bin [run]` | No positional arg — reads from stdin |
| `bin -h`, `bin --help` | Print usage to stdout, exit 0 |
| `bin -v`, `bin --version` | Print version string to stdout, exit 0 |
| `bin -p <val>`, `bin --param <val>` | Set parameter value (stored in `param`) |
| `bin <text> -- <flags>` | Positional text before flags |
| stdout | Normal output data |
| stderr | Diagnostic/error messages (`bin: ...`) |

Exit codes: `0` success, `1` error.

## Data Model

### Internal Structures

No struct types defined. Internal state is stack/scalar:

| Symbol | Type | Role |
|--------|------|------|
| `verb` | `const char *` | Captured verb string (default `"run"`) |
| `param` | `const char *` | Value from `-p`/`--param` flag (default `NULL`) |
| `text` | `const char *` | Positional text argument or stdin content |
| `stdin_text` | `char *` | Dynamically allocated stdin buffer (freed before exit) |
| `data` | `char *` (local to `kc_bin_read_stdin`) | Growing input buffer |
| `length` | `size_t` (local) | Current byte count in buffer |
| `capacity` | `size_t` (local) | Allocated capacity |
| `chunk` | `char[4096]` (local) | Fixed read buffer for `fread` |

### Hard Limits

| Limit | Value | Symbol/Context |
|-------|-------|----------------|
| Read chunk size | 4096 bytes | `chunk` array in `kc_bin_read_stdin` |
| Initial capacity | 4096 bytes | First `realloc` in `kc_bin_read_stdin` |
| Growth factor | 2× | `capacity * 2` doubling strategy |
| Max stdin input | bounded by available memory | No static limit |
| Program version | `"0.1.0"` | `KC_BIN_VERSION` |

## API / Static Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| `kc_bin_read_stdin` | `static int kc_bin_read_stdin(char **out_text)` | Reads all stdin into a `malloc`'d buffer. Returns 0 on success, -1 on allocation/fread error. On empty stdin, sets `*out_text = NULL`. |
| `kc_print_help` | `static void kc_print_help(const char *name)` | Prints usage synopsis, available options (`-p`, `-h`, `-v`). |
| `kc_print_version` | `static void kc_print_version(void)` | Prints `"bin {version}"` to stdout. |
| `main` | `int main(int argc, char **argv)` | Entry point: parses optional verb, positional text, then flag loop. Reads stdin if no positional text. |

Includes: `<stdio.h>`, `<stdlib.h>`, `<string.h>`.

## Build

### Make Targets

| Target | Description |
|--------|-------------|
| `native` | Detects host arch/platform and builds that single target (default) |
| `all` | Builds every target in the cross-compile matrix |
| `{arch}/{platform}` | Builds a specific target (e.g. `x86_64/linux`) |
| `test` | Runs `sh test.sh` |
| `clean` | Removes `.build/` directory |

### Cross-Compilation Matrix

| Target | Compiler | Platform |
|--------|----------|----------|
| `x86_64/linux` | `gcc` | Linux |
| `i686/linux` | `i686-linux-gnu-gcc` | Linux |
| `aarch64/linux` | `aarch64-linux-gnu-gcc` | Linux |
| `armv7hf/linux` | `arm-linux-gnueabihf-gcc` | Linux |
| `armv7/linux` | `arm-linux-gnueabi-gcc` | Linux |
| `riscv64/linux` | `riscv64-linux-gnu-gcc` | Linux |
| `powerpc64le/linux` | `powerpc64le-linux-gnu-gcc` | Linux |
| `mips/linux` | `mips-linux-gnu-gcc` | Linux |
| `mipsel/linux` | `mipsel-linux-gnu-gcc` | Linux |
| `mips64el/linux` | `mips64el-linux-gnuabi64-gcc` | Linux |
| `s390x/linux` | `s390x-linux-gnu-gcc` | Linux |
| `loongarch64/linux` | `loongarch64-linux-gnu-gcc` | Linux |
| `x86_64/windows` | `x86_64-w64-mingw32-gcc` | Windows (MinGW) |
| `i686/windows` | `i686-w64-mingw32-gcc` | Windows (MinGW) |
| `aarch64/android` | NDK CMake toolchain (`arm64-v8a` ABI) | Android |
| `armv7/android` | NDK CMake toolchain (`armeabi-v7a` ABI) | Android |

Output: `bin/{arch}/{platform}/bin` (`.exe` on Windows).

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BIN_NATIVE` | `OFF` | Enable `-march=native` optimization |

Compile flags (GCC): `-O3 -Wall -Wextra -Werror`, defines `_GNU_SOURCE`.  
Compile flags (MSVC): `/O2 /W3 /WX`.  
C Standard: C11.  
Link (UNIX non-Android): `pthread`, `m`.

## Error Handling

| Path | Condition | Action | Exit |
|------|-----------|--------|------|
| `kc_bin_read_stdin` | `out_text == NULL` | Return -1 immediately | N/A |
| `kc_bin_read_stdin` | `realloc` fails | `free(data)`, return -1 | N/A |
| `kc_bin_read_stdin` | `ferror(stdin)` after loop | `free(data)`, return -1 | N/A |
| `kc_bin_read_stdin` | Empty stdin | `free(data)`, `*out_text = NULL`, return 0 | N/A |
| `main` | `-p`/`--param` at end of argv | `fprintf(stderr, "bin: missing value for %s\n", ...)` | 1 |
| `main` | Unknown flag | `fprintf(stderr, "bin: unknown option '%s'\n", ...)` | 1 |
| `main` | `kc_bin_read_stdin` fails | Return 1 immediately | 1 |
| `main` | No text and empty stdin | `free(stdin_text)`, return 0 | 0 |

All error messages go to stderr with `bin: ` prefix. Help and version go to stdout.

## Constraints

- `kc_` prefix is used for internal functions but is not strictly required for app projects (only for library-style kclib projects).
- The `"run"` verb is a demonstration — it is parsed but its value is suppressed via `(void)verb` to avoid unused-variable warnings.
- The `param` variable is similarly suppressed with `(void)param`. Template consumers should replace these with actual usage.
- Stdin is only read when no positional text argument is provided; the two sources are mutually exclusive.
- The stdin reader does not support binary data (`\0` termination).
- The Makefile requires CMake ≥3.14 and Ninja on the build host.
- Android builds require the NDK at `$ANDROID_HOME/ndk/27.2.12479018`.
- Cross-compilation targets assume the corresponding toolchain is installed in `PATH`.
- Test suite (`test.sh`) requires the binary to be pre-built for the host architecture.
- The template intentionally omits `libname.c` and `name.h` — unlike kclib projects, kcbin apps have no library component.
- DocBlocks are required on all functions. Inline comments are forbidden.
- Exactly one trailing newline in every file.
