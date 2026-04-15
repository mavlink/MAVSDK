# Python

MAVSDK provides two Python packages under `py/` that call the C library directly — no gRPC, no separate `mavsdk_server` process required.

- **[mavsdk](api_reference/mavsdk/index.md)** — Synchronous bindings with a threads-and-callbacks API.
- **[aiomavsdk](api_reference/aiomavsdk/index.md)** — Asyncio wrapper around `mavsdk` that exposes an `async`/`await` API.

## Background and Relationship to MAVSDK-Python

The original [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python) (in a separate repository) works by spawning a `mavsdk_server` sidecar process and communicating with it over gRPC. While this design works well, it adds deployment complexity and a runtime dependency on the server binary.

These new wrappers take a different approach: they call `libcmavsdk.so` directly via the C wrapper layer. The result is a self-contained Python package with no gRPC dependency and no external process to manage. The long-term intent is for these packages to replace MAVSDK-Python.

## Choosing a Package

**Use `aiomavsdk`** if you are writing async/await code or migrating from MAVSDK-Python. The API is deliberately similar to MAVSDK-Python, so existing code should translate with minimal changes. `aiomavsdk` depends on `mavsdk` and wraps it in an asyncio layer.

**Use `mavsdk`** if you are working in a synchronous or multithreaded context where asyncio is not appropriate. This package exposes callbacks and blocking calls, consistent with a traditional threaded Python application.

## Installation

See the [QuickStart](quickstart.md) for full installation instructions, including how to build and link the required C library (`libcmavsdk.so`).

A brief summary:

```sh
# Install the synchronous package
pip install ./py/mavsdk

# Install the asyncio wrapper (depends on mavsdk)
pip install ./py/aiomavsdk
```

::: tip
The packages are not yet published to PyPI. Install them from the source tree as shown above.
:::

## Guides

- [QuickStart](quickstart.md)

## API Reference

- [mavsdk API Reference](api_reference/mavsdk/index.md)
- [aiomavsdk API Reference](api_reference/aiomavsdk/index.md)

## For Developers

The API reference pages are generated automatically from the Python source using an AST-based tool — no package import is required. To regenerate the docs after modifying `py/mavsdk` or `py/aiomavsdk`, run:

```sh
tools/generate_docs.sh
```

The generator itself lives at `py/tools/generate_markdown.py`. It parses docstrings from both packages and writes Markdown files into `docs/en/python/api_reference/`.
