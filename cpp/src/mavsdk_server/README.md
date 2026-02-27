# MAVSDK with GRPC

### Build with mavsdk_server

From the `cpp/` directory:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -Bbuild -S.
cmake --build build -j8
```

### Run the mavsdk_server

The mavsdk_server is built as an executable:

```bash
./build/src/mavsdk_server/mavsdk_server_bin
```
