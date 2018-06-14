from setuptools import setup
from codecs import open
from os import path

here = path.abspath(path.dirname(__file__))

setup(
    name="dcsdkgen",
    version="0.1a",
    description="Autogenerator for the DroneCore SDK Bindings",
    url="@TODO",
    author="@TODO",
    author_email="@TODO",

    packages=["autogen"],
    install_requires=["protobuf", "jinja2"],
    entry_points={
        "console_scripts": [
            "dcsdkgen = dcsdkgen.__main__:main"
        ]
    }
)
