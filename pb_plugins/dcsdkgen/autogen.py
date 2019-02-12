# -*- coding: utf-8 -*-
from os import environ
from google.protobuf.compiler import plugin_pb2
from .autogen_file import File
from .methods import Method
from .struct import Struct
from .enum import Enum
from .utils import (get_template_env,
                    has_result,
                    name_parser_factory,
                    type_info_factory)


class AutoGen(object):
    """ Autogenerator for the DronecodeSDK bindings """

    @staticmethod
    def generate_reactive(request):

        params = AutoGen.parse_parameter(request.parameter)
        name_parser_factory.set_template_path(params["template_path"])
        type_info_factory.set_template_path(params["template_path"])
        template_env = get_template_env(params["template_path"])

        for proto_file in request.proto_file:
            plugin_name = proto_file.name.split('.')[0].capitalize()

            enums = Enum.collect_enums(plugin_name,
                                       proto_file.package,
                                       proto_file.enum_type,
                                       template_env)

            structs = Struct.collect_structs(plugin_name,
                                             proto_file.package,
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
                            proto_file.package,
                            template_env,
                            enums,
                            structs,
                            methods,
                            has_result(structs))

            _codegen_response = plugin_pb2.CodeGeneratorResponse()

            # Fill response
            f = _codegen_response.file.add()
            f.name = f"{plugin_name}.{params['file_ext']}"
            f.content = str(out_file)

            return _codegen_response

    @staticmethod
    def parse_parameter(parameter):
        raw_params = parameter.split(',')

        params_dict = {}
        for raw_param in raw_params:
            split_param = raw_param.split('=')
            params_dict[split_param[0]] = split_param[1]

        if 'file_ext' not in params_dict:
            raise Exception("'file_ext' option was not specified! See " +
                    "--[name]_out=file_ext=<value>,<other_options>:/path/to/output " +
                    "or --[name]_opt=file_ext=<value>,<other_options> in the protoc" +
                    "command line.")

        if 'template_path' not in params_dict:
            template_path = environ.get("TEMPLATE_PATH", "./")

            if template_path is not None:
                params_dict["template_path"] = template_path
            else:
                raise Exception("'template_path' option was not specified! See " +
                        "--[name]_out=template_path=<value>,<other_options>:/path/to/output " +
                        "or --[name]_opt=template_path=<value>,<other_options> in the protoc" +
                        "command line. Alternatively, you can set the TEMPLATE_PATH " +
                        "environment variable.")

        return params_dict

