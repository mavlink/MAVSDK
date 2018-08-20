# -*- coding: utf-8 -*-


from google.protobuf.compiler import plugin_pb2
from .autogen_file import File
from .methods import Method
from .struct import Struct
from .enum import Enum
from .utils import get_template_env


class AutoGen(object):
    """ Autogenerator for the DroneCore bindings """

    @staticmethod
    def generate_reactive(request):

        # Get the template folder from the environment
        template_env = get_template_env(
            f"./templates/{request.parameter}")

        for proto_file in request.proto_file:
            plugin_name = proto_file.name.split('.')[0].capitalize()

            enums = Enum.collect_enums(proto_file.package,
                                       proto_file.enum_type,
                                       template_env)

            structs = Struct.collect_structs(proto_file.package,
                                             proto_file.message_type,
                                             template_env)

            requests = Struct.collect_requests(proto_file.package,
                                               proto_file.message_type)

            responses = Struct.collect_responses(proto_file.package,
                                                 proto_file.message_type)

            methods = Method.collect_methods(plugin_name,
                                             proto_file.package,
                                             proto_file.service[0].method,
                                             structs,
                                             requests,
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
            f.name = f"{plugin_name}.{request.parameter}"
            f.content = str(out_file)

            return _codegen_response
