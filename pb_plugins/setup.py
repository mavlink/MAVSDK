from setuptools import setup
from os import path

here = path.abspath(path.dirname(__file__))

setup(
    name="protoc-gen-dcsdk",
    version="0.1",
    description="Autogenerator for the MAVSDK Bindings",
    url="@TODO",
    author="@TODO",
    author_email="@TODO",

    packages=["protoc_gen_dcsdk"],
    install_requires=["protobuf", "jinja2"],
    entry_points={
        "console_scripts": [
            "protoc-gen-dcsdk= protoc_gen_dcsdk.__main__:main"
        ]
    }
)
