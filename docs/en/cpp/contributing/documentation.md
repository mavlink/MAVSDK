# Contributing to Documentation

Making quick fixes to existing text or raising issues is very easy.
If you want to help, [get in touch](../../index.md#getting-help).

::: tip
You will need a [Github](https://github.com/) login to make and submit changes to this guide.
:::

## Overview

This guide is written in [markdown](https://vitepress.dev/guide/markdown) and stored in the Github [mavlink/MAVSDK](https://github.com/mavlink/MAVSDK) repository under the `docs/` folder.

The [API Reference](../api_reference/index.md) section is compiled from source code into markdown using a [separate toolchain](#api-reference) and then checked in.
Updates to the reference should be made in the [source code](https://github.com/mavlink/MAVSDK) (see [API Reference](#api-reference) below for more information).

The guide is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) - if you make any changes then they will be made available under the same license.


## Making a Quick Fix

Simple fixes to an existing page can be made directly on Github:

1. Navigate to the relevant file in the [docs/en/](https://github.com/mavlink/MAVSDK/tree/main/docs/en) folder.
1. Click the pencil (edit) icon to open the file editor.
1. Make the required changes.
1. At the bottom of the page, enter a commit message and select **Create a new branch** to open a pull request.


## Raising an Issue

To raise an issue against the documentation, open a [Github issue](https://github.com/mavlink/MAVSDK/issues) and include the page URL and a description of the problem.


## Building the Docs Locally

The docs use [VitePress](https://vitepress.dev/). To build and preview locally:

```bash
cd docs
npm install
npm run dev
```

This starts a local dev server at `http://localhost:5173`.


## API Reference

The C++ public header files are annotated using docstrings using [Doxygen](https://www.doxygen.nl/manual/index.html) syntax.
You can extract the documentation to markdown files (one per class) on macOS or Linux using the instructions in [Build API Reference Documentation](../guide/build_docs.md).

In order to include new API reference in the documentation:
- Copy the generated files into the [docs/en/cpp/api_reference](https://github.com/mavlink/MAVSDK/tree/main/docs/en/cpp/api_reference) folder
- *New* APIs should be added to the appropriate sections in [docs/en/SUMMARY.md](https://github.com/mavlink/MAVSDK/tree/main/docs/en/SUMMARY.md) and [docs/en/cpp/api_reference/index.md](https://github.com/mavlink/MAVSDK/tree/main/docs/en/cpp/api_reference/index.md).
