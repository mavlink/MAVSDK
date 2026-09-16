# Python system tests

End-to-end tests for the Python bindings. Each test brings up two MAVSDK
instances in-process — one configured as a ground station, one as an autopilot —
links them over a local UDP connection, and drives a real MAVLink exchange
between them. Nothing is mocked: the assertions are made on what the server
side actually received.

They are the Python counterpart of `cpp/src/system_tests`, and they cover both
wrappers: `test_mission_raw_upload.py` for `mavsdk`, and
`test_mission_raw_upload_async.py` for `aiomavsdk`.

## Requirements

`libcmavsdk.so` has to be built and placed in `py/mavsdk/mavsdk/lib/`, which is
where `cmavsdk_loader` looks for it. From the repository root:

```bash
# 1. Build and install the C++ library
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
      -DCMAKE_INSTALL_PREFIX=install -Bbuild/release -Hcpp
cmake --build build/release --target install -- -j$(nproc)

# 2. Build and install the C bindings against it
cmake -DCMAKE_INSTALL_PREFIX=build/install -DBUILD_SHARED_LIBS=ON \
      -DCMAKE_PREFIX_PATH="$(pwd)/cpp/install;$(pwd)/cpp/build/release/third_party/install" \
      -Bc/build -Hc
cmake --build c/build --target install -- -j$(nproc)

# 3. Put the library where the Python package will find it
mkdir -p py/mavsdk/mavsdk/lib
cp -a c/build/install/lib/libcmavsdk.so* py/mavsdk/mavsdk/lib/
```

Building the C++ library with `-DBUILD_SHARED_LIBS=OFF` is what makes
`libcmavsdk.so` self-contained. If you build it shared instead, you also need
`LD_LIBRARY_PATH` to point at the C++ library when running the tests.

Then install the test dependencies and both bindings from this working tree:

```bash
pip install -r py/requirements-dev.txt -e py/mavsdk -e py/aiomavsdk
```

## Running

```bash
cd py/test
pytest -v
```

## Writing a new test

Keep each test standalone: set the two instances up, run the exchange, and
assert, all in the one test function. `test_mission_raw_upload.py` is meant to
be readable top to bottom as a template. Two things it does on purpose, and
that new tests should keep doing:

- **Register server plugins before the connection is added.** MAVSDK advertises
  component capabilities in the first `AUTOPILOT_VERSION` exchange, so a server
  plugin created after the link is up may miss the window.
- **Assert on what the server received, not just on the returned result.** The
  bugs in [#3087](https://github.com/mavlink/MAVSDK/issues/3087) were about
  arguments getting lost on the way into the C layer; several of them still
  produced a plausible-looking result code.

UDP ports are hard-coded, as in `cpp/src/system_tests`. Pick one that no other
test uses.
