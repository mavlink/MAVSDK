# -*- coding: utf-8 -*-


import sys
from google.protobuf.compiler import plugin_pb2
from . import AutoGen


def main():
    """ Calls the autogenerator """
    # Parse request
    request = plugin_pb2.CodeGeneratorRequest()
    request.ParseFromString(sys.stdin.buffer.read())

    # Generate code and output it to stdout
    sys.stdout.buffer.write(
        AutoGen.generate_reactive(request).SerializeToString())


if __name__ == '__main__':
    main()
