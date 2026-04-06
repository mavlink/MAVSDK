#!/usr/bin/env python3
"""
Generate Markdown API reference docs from mavsdk and aiomavsdk Python source files.

Uses AST parsing so no import of the packages is required.
"""

import ast
import inspect
import sys
import textwrap
from pathlib import Path


# ── AST helpers ──────────────────────────────────────────────────────────────

def _clean_docstring(raw: str) -> str:
    """Clean a raw docstring with mixed indentation.

    The aiomavsdk Jinja2 template produces docstrings where the first-line
    summary and NumPy-style section headers (Raises, Parameters, …) have
    several leading spaces while body-text continuation lines have none.
    inspect.cleandoc bases its dedent on the *minimum* indentation, which
    is 0, so nothing gets stripped.

    Instead we find the minimum indentation of lines that *do* have
    indentation (> 0) and strip that amount from every line.
    """
    # First pass: standard cleandoc strips the initial blank line and trailing
    # whitespace.
    text = inspect.cleandoc(raw)
    lines = text.splitlines()

    # Find minimum non-zero indentation.
    indents = [len(l) - len(l.lstrip()) for l in lines if l.strip() and l[0] == " "]
    if not indents:
        return text

    min_indent = min(indents)
    stripped = []
    for line in lines:
        if line.startswith(" " * min_indent):
            stripped.append(line[min_indent:])
        else:
            stripped.append(line)
    return "\n".join(stripped)


def get_docstring(node):
    """Return the docstring of a function/class/module node, or ''."""
    raw = ast.get_docstring(node, clean=False)
    if raw:
        return _clean_docstring(raw)
    return ""


def get_annotation(node):
    """Return a string representation of a type annotation node."""
    if node is None:
        return ""
    return ast.unparse(node)


def get_signature(func_node):
    """Return (params_str, return_str) for a function definition node."""
    args = func_node.args
    parts = []

    # positional args with defaults
    n_defaults = len(args.defaults)
    n_args = len(args.args)
    for i, arg in enumerate(args.args):
        if arg.arg == "self":
            continue
        ann = f": {get_annotation(arg.annotation)}" if arg.annotation else ""
        default_idx = i - (n_args - n_defaults)
        if default_idx >= 0:
            default = f" = {ast.unparse(args.defaults[default_idx])}"
        else:
            default = ""
        parts.append(f"{arg.arg}{ann}{default}")

    # *args
    if args.vararg:
        ann = f": {get_annotation(args.vararg.annotation)}" if args.vararg.annotation else ""
        parts.append(f"*{args.vararg.arg}{ann}")

    # keyword-only args
    for i, arg in enumerate(args.kwonlyargs):
        ann = f": {get_annotation(arg.annotation)}" if arg.annotation else ""
        default = ""
        if args.kw_defaults[i] is not None:
            default = f" = {ast.unparse(args.kw_defaults[i])}"
        parts.append(f"{arg.arg}{ann}{default}")

    # **kwargs
    if args.kwarg:
        ann = f": {get_annotation(args.kwarg.annotation)}" if args.kwarg.annotation else ""
        parts.append(f"**{args.kwarg.arg}{ann}")

    params_str = ", ".join(parts)
    return_str = get_annotation(func_node.returns)
    return params_str, return_str


def is_private(name):
    return name.startswith("_")


# ── Markdown rendering ────────────────────────────────────────────────────────

def render_docstring_body(doc):
    """Lightly format a docstring: clean up indentation, pass through as-is."""
    return doc


def render_function(func_node, is_async=False, is_method=False, heading_level=4):
    """Render a function/method node to markdown."""
    lines = []
    name = func_node.name
    params, ret = get_signature(func_node)
    prefix = "async " if is_async else ""
    kind = "method" if is_method else "function"

    sig = f"{prefix}def {name}({params})"
    if ret:
        sig += f" → {ret}"

    h = "#" * heading_level
    lines.append(f"{h} `{name}`\n")
    lines.append(f"```python\n{sig}\n```\n")

    doc = get_docstring(func_node)
    if doc:
        lines.append(render_docstring_body(doc) + "\n")

    return "\n".join(lines)


def render_enum(class_node, heading_level=3):
    """Render an IntEnum class to markdown."""
    lines = []
    name = class_node.name
    h = "#" * heading_level
    lines.append(f"{h} `{name}`\n")

    doc = get_docstring(class_node)
    if doc:
        lines.append(doc + "\n")

    # Collect enum members (simple assignments)
    members = []
    for node in class_node.body:
        if isinstance(node, ast.Assign):
            for target in node.targets:
                if isinstance(target, ast.Name) and not target.id.startswith("_"):
                    value = ast.unparse(node.value)
                    members.append((target.id, value))

    if members:
        lines.append("| Value | Description |")
        lines.append("|-------|-------------|")
        for member_name, member_val in members:
            lines.append(f"| `{member_name}` ({member_val}) | |")
        lines.append("")

    return "\n".join(lines)


def render_struct(class_node, heading_level=3):
    """Render a data structure class to markdown."""
    lines = []
    name = class_node.name
    h = "#" * heading_level
    lines.append(f"{h} `{name}`\n")

    doc = get_docstring(class_node)
    if doc:
        lines.append(doc + "\n")

    # Fields from __init__ or annotated assignments
    fields = []
    for node in class_node.body:
        if isinstance(node, ast.FunctionDef) and node.name == "__init__":
            for arg in node.args.args:
                if arg.arg == "self":
                    continue
                ann = get_annotation(arg.annotation)
                fields.append((arg.arg, ann))

    if fields:
        lines.append("**Fields:**\n")
        for fname, ftype in fields:
            type_str = f" `{ftype}`" if ftype else ""
            lines.append(f"- `{fname}`{type_str}")
        lines.append("")

    return "\n".join(lines)


def render_plugin_class(class_node, is_async=False, heading_level=3):
    """Render the main plugin class (Action, ActionAsync, etc.) to markdown."""
    lines = []
    name = class_node.name
    h = "#" * heading_level
    lines.append(f"{h} `{name}`\n")

    doc = get_docstring(class_node)
    if doc:
        lines.append(doc + "\n")

    # Methods
    for node in class_node.body:
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
            if is_private(node.name):
                continue
            lines.append(render_function(
                node,
                is_async=isinstance(node, ast.AsyncFunctionDef),
                is_method=True,
                heading_level=heading_level + 1,
            ))

    return "\n".join(lines)


# ── Per-file parser ───────────────────────────────────────────────────────────

def is_enum_class(class_node):
    """True if the class inherits from IntEnum."""
    for base in class_node.bases:
        if ast.unparse(base) in ("IntEnum", "Enum"):
            return True
    return False


def is_error_class(class_node):
    """True if the class is an Error/Exception class."""
    return class_node.name.endswith("Error") or any(
        "Exception" in ast.unparse(b) or "Error" in ast.unparse(b)
        for b in class_node.bases
    )


def is_plugin_class(class_node, plugin_name):
    """True if this looks like the main plugin class."""
    # Main class name matches plugin name (e.g. Action, ActionAsync)
    return class_node.name.lower().startswith(plugin_name.lower())


def parse_plugin_file(path: Path, plugin_name: str, pkg_name: str) -> str:
    """Parse a plugin source file and return markdown."""
    source = path.read_text()
    tree = ast.parse(source)

    lines = []

    # Module docstring
    module_doc = get_docstring(tree)
    if module_doc:
        lines.append(module_doc + "\n")

    enums = []
    structs = []
    plugin_classes = []
    error_classes = []

    for node in tree.body:
        if not isinstance(node, ast.ClassDef):
            continue
        if is_error_class(node):
            error_classes.append(node)
        elif is_enum_class(node):
            enums.append(node)
        elif is_plugin_class(node, plugin_name):
            plugin_classes.append(node)
        else:
            structs.append(node)

    if enums:
        lines.append("## Enums\n")
        for e in enums:
            lines.append(render_enum(e))

    if structs:
        lines.append("## Structs\n")
        for s in structs:
            lines.append(render_struct(s))

    for cls in plugin_classes:
        is_async = "Async" in cls.name
        lines.append(f"## `{cls.name}`\n")
        doc = get_docstring(cls)
        if doc:
            lines.append(doc + "\n")
        for node in cls.body:
            if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
                if is_private(node.name):
                    continue
                lines.append(render_function(
                    node,
                    is_async=isinstance(node, ast.AsyncFunctionDef),
                    is_method=True,
                    heading_level=3,
                ))

    return "\n".join(lines)


# ── Index file ────────────────────────────────────────────────────────────────

def render_index(pkg_name: str, pkg_title: str, pkg_description: str, plugins: list[str]) -> str:
    lines = [
        f"# {pkg_title} API Reference\n",
        f"{pkg_description}\n",
        "## Plugins\n",
    ]
    for plugin in sorted(plugins):
        title = plugin.replace("_", " ").title()
        lines.append(f"- [{title}]({plugin}.md)")
    lines.append("")
    return "\n".join(lines)


# ── Main ──────────────────────────────────────────────────────────────────────

def generate(pkg_dir: Path, out_dir: Path, pkg_name: str, pkg_title: str, pkg_description: str):
    plugins_dir = pkg_dir / "plugins"
    out_dir.mkdir(parents=True, exist_ok=True)

    plugins = []
    for plugin_dir in sorted(plugins_dir.iterdir()):
        if not plugin_dir.is_dir() or plugin_dir.name.startswith("_"):
            continue
        plugin_name = plugin_dir.name
        plugin_file = plugin_dir / f"{plugin_name}.py"
        if not plugin_file.exists():
            continue

        md = parse_plugin_file(plugin_file, plugin_name, pkg_name)
        title = plugin_name.replace("_", " ").title()
        header = f"# {title}\n\n"
        (out_dir / f"{plugin_name}.md").write_text(header + md)
        plugins.append(plugin_name)
        print(f"  {plugin_name}.md")

    index_md = render_index(pkg_name, pkg_title, pkg_description, plugins)
    (out_dir / "index.md").write_text(index_md)
    print(f"  index.md ({len(plugins)} plugins)")


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Generate Python API reference markdown")
    parser.add_argument("--py-root", required=True, help="Path to py/ directory")
    parser.add_argument("--out-root", required=True, help="Output root (docs/en/python/api_reference)")
    args = parser.parse_args()

    py_root = Path(args.py_root)
    out_root = Path(args.out_root)

    print("Generating mavsdk API reference...")
    generate(
        pkg_dir=py_root / "mavsdk" / "mavsdk",
        out_dir=out_root / "mavsdk",
        pkg_name="mavsdk",
        pkg_title="mavsdk",
        pkg_description=(
            "Synchronous Python bindings for MAVSDK. "
            "Calls the C library directly — no gRPC dependency."
        ),
    )

    print("Generating aiomavsdk API reference...")
    generate(
        pkg_dir=py_root / "aiomavsdk" / "aiomavsdk",
        out_dir=out_root / "aiomavsdk",
        pkg_name="aiomavsdk",
        pkg_title="aiomavsdk",
        pkg_description=(
            "Asyncio wrapper around `mavsdk`, providing an async/await API "
            "similar to the original gRPC-based MAVSDK-Python."
        ),
    )


if __name__ == "__main__":
    main()
