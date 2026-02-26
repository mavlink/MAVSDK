import os
from os.path import join as path_join
from setuptools import setup, find_packages


def parse_long_description():
    try:
        with open(path_join(os.getcwd(), "README.md"), encoding="utf-8") as f:
            return f.read()
    except FileNotFoundError:
        return "Asyncio wrapper for MAVSDK"


setup(
    name="aiomavsdk",
    use_scm_version={
        "root": "../..",
        "relative_to": __file__,
        "version_scheme": "post-release",
    },
    setup_requires=["setuptools-scm"],
    maintainer="Jonas Vautherin, Julian Oes",
    maintainer_email="dev@jonas.vautherin.ch, julian@oes.ch",
    description="Asyncio wrapper for MAVSDK",
    long_description=parse_long_description(),
    long_description_content_type="text/markdown",
    url="https://github.com/mavlink/MAVSDK",
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
        "mavsdk4",
    ],
)
