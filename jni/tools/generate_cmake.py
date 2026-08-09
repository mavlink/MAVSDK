#!/usr/bin/env python3
import argparse
from pathlib import Path

import jinja2


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plugins", nargs="+", help="enabled plugins")
    args = parser.parse_args()

    plugins = args.plugins

    project_root = Path(__file__).resolve().parent.parent
    env = jinja2.Environment(loader=jinja2.FileSystemLoader(project_root / "templates"))
    template = env.get_template("plugins_generated.cmake.j2")
    output = template.render(plugins=plugins)

    output_path = project_root / "plugins_generated.cmake"
    with output_path.open("w") as f:
        f.write(output)

    print(f"Successfully wrote to {output_path}")


if __name__ == '__main__':
    main()
