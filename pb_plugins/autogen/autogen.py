# -*- coding: utf-8 -*-


from google.protobuf.compiler import plugin_pb2
from jinja2 import Environment, FileSystemLoader
from .autogen_file import File
from .methods import Method
from .struct import Struct
from .enum import Enum


class AutoGen(object):
    """ Autogenerator for the DroneCore bindings """

    @staticmethod
    def generate_reactive(request):

        # Get the template folder from the environment
        template_env = Environment(loader=FileSystemLoader(
            searchpath=f"./templates/{request.parameter}"))

        for proto_file in request.proto_file:
            plugin_name = proto_file.name.split('.')[0].capitalize()

            enums = Enum.collect_enums(proto_file.package,
                                       proto_file.enum_type,
                                       template_env)

            structs = Struct.collect_structs(proto_file.package,
                                             proto_file.message_type,
                                             template_env)

            responses = Struct.collect_responses(proto_file.package,
                                                 proto_file.message_type,
                                                 template_env)

            methods = Method.collect_methods(plugin_name,
                                             proto_file.package,
                                             proto_file.service[0].method,
                                             structs,
                                             responses,
                                             template_env)

            out_file = File(plugin_name,
                            template_env,
                            enums,
                            structs,
                            methods)

            _codegen_response = plugin_pb2.CodeGeneratorResponse()

            # Fill response
            f = _codegen_response.file.add()
            f.name = f"generated/{plugin_name}.{request.parameter}"
            f.content = str(out_file)

            return _codegen_response
