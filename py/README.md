# Python wrappers for MAVSDK

This folder contains the `mavsdk` Python package, which exposes two interfaces:

* `mavsdk` is an old school Python API (threads and callbacks, no asyncio).
* `mavsdk.asyncio` is an asyncio wrapper around `mavsdk` above.

Both ship in the same distribution, so there is nothing extra to install to use
either one.

Moreover, there is no longer a dependency on gRPC, unlike
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The `mavsdk.asyncio`
API tries to be as close as possible to MAVSDK-Python in order to facilitate
transition. This is still new as compared to MAVSDK-Python, but the idea is to
eventually replace it. It has all the features of MAVSDK-Python, it just hasn't
been tested as much.

## Debug symbols

The wheels contain a `RelWithDebInfo` build of `libcmavsdk` with the debug info
split off, so a backtrace out of the box names the functions but has no line
numbers. The debug info that goes with it is attached to every
[release](https://github.com/mavlink/MAVSDK/releases) as
`mavsdk-debug-symbols-<platform tag>.tar.gz`, using the same platform tag as the
wheel it belongs to, e.g. `manylinux_2_28_x86_64`.

The library sits here:

```
python3 -c "import mavsdk, pathlib; print(pathlib.Path(mavsdk.__file__).parent / 'lib')"
```

On Linux the archive is laid out by build ID, which is how gdb finds it:

```
mkdir -p ~/.mavsdk-debug
tar -C ~/.mavsdk-debug -xzf mavsdk-debug-symbols-manylinux_2_28_x86_64.tar.gz
gdb -ex "set debug-file-directory $HOME/.mavsdk-debug" --args python3 your_script.py
```

On macOS, unpack `libcmavsdk.dylib.dSYM` into the directory printed above, next
to `libcmavsdk.dylib`, and lldb picks it up from there.

On Windows, unpack `cmavsdk.pdb` into that directory, next to `cmavsdk.dll`.
