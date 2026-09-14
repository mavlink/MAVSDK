# Agent notes

## Regenerating from proto

Proto changes drive generated code for **all** language bindings, not just C++.
After editing a `.proto` (in the `proto` submodule), reproduce what CI's
"style and proto check" does, from the repo root:

```bash
# 1. Make sure configure step in C++ is available, e.g.
(cd cpp && cmake -Bbuild-debug-with-server -DCMAKE_BUILD_TYPE=Debug -S. -DBUILD_MAVSDK_SERVER=ON)

# 2. Regenerate every language (C++, C, Python, JNI, Kotlin)
tools/generate_from_protos.bash --build-dir cpp/build-debug-with-server

# 3. Fix C++ style
(cd cpp && tools/fix_style.py --quiet .)

# 4. Fix Kotlin style (ktfmt via the gradle wrapper)
(cd kt/mavsdk-kotlin && ./gradlew ktfmtFormat --console=plain)
```

Then commit all of the resulting changes. CI's "style and proto check"
(`.github/workflows/linux.yml`) runs exactly these steps and then
`git diff --exit-code`, so any file you forget to regenerate fails there.

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
