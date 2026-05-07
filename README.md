# bin.c - C Application Blueprint

`bin.c` is a template for creating small, high-performance C CLI applications. It serves as a blueprint for the composable native tools in the KaisarCode ecosystem.

---

## CLI

Example CLI interface provided by the blueprint.

### Examples

Execute the default operation with a verb:

```bash
./bin/x86_64/linux/bin run "example input"
```

Provide a parameter flag:

```bash
./bin/x86_64/linux/bin run "example input" -p "value"
```

Pipe input through standard input:

```bash
echo "example input" | ./bin/x86_64/linux/bin run
```

---

### Parameters

| Command/Flag | Description |
| :--- | :--- |
| `run` | Example verb for running an operation |
| `-p`, `--param <val>` | Set parameter value |
| `-h`, `--help` | Show help and usage |
| `-v`, `--version` | Show version |

---

## Build

Compiled artifacts are generated under `bin/{arch}/{platform}/` for the host architecture running the build.

```bash
make clean && make
```

## Multiarch Builds

The project is prepared to build artifacts for multiple architectures under `bin/{arch}/{platform}/`. A plain `make` builds only the current host architecture, while the targets below build the full matrix or a specific target.

```bash
make all
make x86_64/linux
make x86_64/windows
make i686/linux
make i686/windows
make aarch64/linux
make aarch64/android
make armv7/linux
make armv7/android
make armv7hf/linux
make riscv64/linux
make powerpc64le/linux
make mips/linux
make mipsel/linux
make mips64el/linux
make s390x/linux
make loongarch64/linux
```

---

**Author:** KaisarCode

**Email:** <kaisarcode@gmail.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
