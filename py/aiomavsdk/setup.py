import os
import subprocess
from os.path import join as path_join
from setuptools import setup, find_packages


def parse_long_description():
    try:
        with open(path_join(os.getcwd(), "README.md"), encoding="utf-8") as f:
            return f.read()
    except FileNotFoundError:
        return "Asyncio wrapper for MAVSDK"


def version():
    try:
        process = subprocess.Popen(
            ["git", "describe", "--tags"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        output, _ = process.communicate()
        exit_code = process.wait()
        if exit_code != 0:
            print("No git tags found, using default version")
            return "0.1.0"
        git_describe_str = output.decode("utf-8").strip()
        git_describe_str = git_describe_str.replace("-g", "+g")
        print(f"Version from git: {git_describe_str}")
        return git_describe_str
    except Exception as e:
        print(f"Could not determine version from git: {e}")
        return "0.1.0"


setup(
    name="aiomavsdk",
    version=version(),
    maintainer="Jonas Vautherin, Julian Oes",
    maintainer_email="dev@jonas.vautherin.ch, julian@oes.ch",
    description="Asyncio wrapper for MAVSDK",
    long_description=parse_long_description(),
    long_description_content_type="text/markdown",
    url="https://github.com/mavlink/MAVSDK-Python",
    packages=find_packages(),
    license="BSD-3-Clause",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Programming Language :: Python :: 3.14",
    ],
    python_requires=">=3.8",
    install_requires=[
        "pymavsdk",
    ],
)
