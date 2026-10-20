# Agent notes

## Regenerating from proto

Proto changes drive generated code for **all** language bindings, not just C++.
After editing a `.proto` (in the `proto` submodule), run from the repo root:

```bash
tools/regenerate_all.bash
```

It regenerates every language (C++, C, Python, JNI, Kotlin), fixes C++ and
Kotlin style and regenerates the C++ API docs. If a required tool is missing
(e.g. clang-format 21, JDK 21, doxygen 1.15.0), it pulls the CI image and runs
itself in docker/podman instead. CI's "style and proto check" and "docs check"
(`.github/workflows/linux.yml`) run this same script and then
`git diff --exit-code`.

Useful options: `--skip-docs` (much faster), `--build-dir DIR` to reuse an
existing C++ build that has protoc in `third_party/install`, `--container` /
`--no-container`, `--no-pull`.

Note that `cpp/tools/generate_from_protos.sh` only regenerates C++ — that is not
enough on its own.

### Toolchain

- Python formatting needs `ruff`.
- Kotlin formatting is done by the `ktfmt` gradle plugin, run via
  `./gradlew ktfmtFormat` in `kt/mavsdk-kotlin` (needs a JDK; the `./gradlew`
  wrapper fetches ktfmt itself). It is *not* ktlint.

The Kotlin generator emits raw template output and does not format; ktfmt is a
separate step. Skipping it makes every `.kt` file diff. If you can't run gradle,
a tiny doc-only change can instead be handled by reverting the spurious kt churn
(`git checkout -- kt/`) and editing the single affected generated `.kt` file by
hand to match CI's expected output.
