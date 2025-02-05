# Generate API docs from source

The [API reference docs for C++](../api_reference/index.md) are already pre-generated. If you still would like to generate them, you find the instructions to do so below.

## Build API Reference Documentation {#build_api_reference}

The C++ source code is annotated using comments using [Doxygen](http://doxygen.nl/manual/index.html) syntax.

Extract the documentation to markdown files (one per class) on macOS or Linux using the commands:
```bash
rm -rf tools/docs # Remove previous docs
./tools/generate_docs.sh
```
The files are created in `/install/docs/markdown`.

::: info
Extracting the API reference does not yet work automatically on Windows.
:::

::: info
The *generate_docs.sh* script [builds the library](build.md), installs it locally to **/install**, and then uses *DOxygen* to create XML documentation in **/install/docs/xml**.
The [generate_markdown_from_doxygen_xml.py](https://github.com/mavlink/MAVSDK/blob/main/tools/generate_markdown_from_doxygen_xml.py) script is then run on all files in the */xml* directory to generate markdown files in **/install/docs/markdown**.
:::
