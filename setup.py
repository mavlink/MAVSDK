from setuptools import setup, find_packages

def parse_long_description():
    """
    Helper function which parses the readme

    :returns: Content of the Readme
    """
    with open(path.join(getcwd(), "README.md"), encoding="utf-8") as f:
        return f.read()

def version():
    process = subprocess.Popen(["git", "describe", "--tags"], stdout=subprocess.PIPE)
    output, _ = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        raise RuntimeError(f"git describe command exited with: {exit_code}")
    git_describe_str = output.decode("utf-8").strip()
    git_describe_str = git_describe_str.replace("-g", "+g")
    print(git_describe_str)
    return git_describe_str

setup(
    name="pymavsdk",
    version=version(),
    maintainer="Jonas Vautherin, Julian Oes",
    maintainer_email="dev@jonas.vautherin.ch, julian@oes.ch",
    description="Python wrapper for mavsdk",
    long_description=parse_long_description(),
    long_description_content_type="text/markdown",
    url="https://github.com/mavlink/MAVSDK-Python",
    packages=["pymavsdk"],
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: BSD License",
        "Programming Language :: Python :: 3.14",
    ],
    python_requires=">=3.7",
    include_package_data=True,
    package_data={
        "pymavsdk": ["lib/*"],
    },
)
