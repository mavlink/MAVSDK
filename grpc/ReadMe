
Use the following commands inside server folder to generate the CPP code:

 $ protoc -I ../proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../proto/dronecore.proto
 $ protoc -I ../proto --cpp_out=. ../proto/dronecore.proto

Use the following command inside python_client folder to generate the Python code:

 $ python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/dronecore.proto

Steps to run:
- make sure dronecore is already built and installed with below command
 $ make default install

- Goto server folder and generate CPP files using above mentioned command
- Run make command to generate dronecore_server application
- Run PX4 sitl
- Run dronecore_server application, which connects to vehicle and listens to localhost:50051
- In another terminal, goto python_client folder and generate python files using above mentioned command
- Run dronecore_client.py
- vehicle takeoff and landing after 10 seconds will be observed on sitl

Things to Remember:
- unset http_proxy if anything is set

## Prerequisites

###Install grpc

sudo apt install grpc # ?

###Install grpcio_tools

sudo pip install grpcio_tools
