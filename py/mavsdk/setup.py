import os
import subprocess

from os.path import join as path_join
from setuptools import setup, find_packages
from setuptools.dist import Distribution

def parse_long_description():
    """
    Helper function which parses the readme
    :returns: Content of the Readme
    """
    try:
        with open(path_join(os.getcwd(), "README.md"), encoding="utf-8") as f:
            return f.read()
    except FileNotFoundError:
        return "Python wrapper for MAVSDK"

class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(self):
        return True

    def is_pure(self):
        return False

#if not ver.startswith("v4"):
#    raise RuntimeError(
#        f"Version '{ver}' does not start with 'v4'. "
#        "This is a reminder that if you're releasing a new major version,"
#        "you need to update the package name below from 'mavsdk4' to"
#        "'mavsdk5' (or whatever the new major is)."
#    )

setup(
    name="mavsdk4",
    use_scm_version={
        "root": "../..",
        "relative_to": __file__,
        "version_scheme": "post-release",
    },
    setup_requires=["setuptools-scm"],
    maintainer="Jonas Vautherin, Julian Oes",
    maintainer_email="dev@jonas.vautherin.ch, julian@oes.ch",
    description="Python wrapper for mavsdk",
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
    include_package_data=True,
    package_data={
        "mavsdk": ["lib/*"],
    },
    distclass=BinaryDistribution,
    options={
        "bdist_wheel": {
            "py_limited_api": "cp38",
        }
    },
)
