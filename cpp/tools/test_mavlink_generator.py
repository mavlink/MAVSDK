#!/usr/bin/env python3
"""Build the patched MAVLink dependency with no pip and no package downloads.

Pass a local MAVLink checkout with its pymavlink submodule initialized.
The test clones both repositories into a temporary directory before patching.
"""

import argparse
import os
import subprocess
import tempfile
import venv
from pathlib import Path


def run(*args, cwd=None):
    subprocess.run([str(arg) for arg in args], cwd=cwd, check=True)


def check_generator(source, work):
    checkout = work / "mavlink"
    run("git", "clone", "--no-hardlinks", source, checkout)
    run("git", "clone", "--no-hardlinks", source / "pymavlink", checkout / "pymavlink")
    patch = Path(__file__).resolve().parents[1] / "third_party/mavlink/mavlink.patch"
    run("git", "apply", "--check", patch, cwd=checkout)
    run("git", "apply", patch, cwd=checkout)

    environment = work / "python"
    venv.create(environment, with_pip=False)
    python = environment / ("Scripts/python.exe" if os.name == "nt" else "bin/python")
    run(python, "-c", "import importlib.util; assert importlib.util.find_spec('pip') is None")
    run(
        "cmake",
        "-S",
        checkout,
        "-B",
        work / "build",
        f"-DPython_EXECUTABLE={python}",
        "-DMAVLINK_DIALECT=ardupilotmega",
        "-DMAVLINK_VERSION=2.0",
    )
    run("cmake", "--build", work / "build", "--target", "generate_c_headers")
    headers = work / "build/include/mavlink"
    for header in ("ardupilotmega/mavlink.h", "common/mavlink_msg_command_long.h"):
        if not (headers / header).is_file():
            raise RuntimeError(f"Missing generated header: {header}")
    if (work / "build/pip-dependencies").exists():
        raise RuntimeError("Generator created a pip installation directory")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="mavsdk-generator-") as directory:
        check_generator(args.source.resolve(), Path(directory))
    print("Pinned MAVLink generator works without pip")


if __name__ == "__main__":
    main()
