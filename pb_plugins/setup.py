import os
import subprocess
import sys

from distutils.command.build import build
from distutils.spawn import find_executable
from setuptools import setup

script_dir = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))

class custom_build(build):
    """
    Class that overrides the build step to add a custom pre-build step
    """

    def run(self):
        if 'PROTOC' in os.environ and os.path.exists(os.environ['PROTOC']):
            protoc = os.environ['PROTOC']
        else:
            protoc = find_executable('protoc')

        if protoc is None:
            sys.stderr.write("'protoc' not found. Install it globally or set the PROTOC environment variable to point to it.")
            sys.exit(1)

        proto_path = f"{script_dir}/../protos"
        output_path = f"{script_dir}/protoc_gen_dcsdk"
        proto_file = "mavsdk_options.proto"

        if not os.path.exists(f"{proto_path}/{proto_file}"):
            sys.stderr.write(f"Can't find required file: {proto_file} in {proto_path}!")
            sys.exit(1)
        elif not os.path.exists(output_path):
            sys.stderr.write(f"Can't find output path: {output_path}!")
            sys.exit(1)

        command = [protoc, '-I', proto_path, f"--python_out={output_path}", proto_file]
        print(command)
        if subprocess.call(command) != 0:
            sys.exit(1)

        build.run(self)

setup(
    name="protoc-gen-dcsdk",
    version="0.1",
    description="Autogenerator for the MAVSDK bindings",
    url="@TODO",
    author="@TODO",
    author_email="@TODO",

    packages=["protoc_gen_dcsdk"],
    install_requires=["protobuf", "jinja2"],
    entry_points={
        "console_scripts": [
            "protoc-gen-dcsdk= protoc_gen_dcsdk.__main__:main"
        ]
    },
    cmdclass={'build': custom_build}
)
