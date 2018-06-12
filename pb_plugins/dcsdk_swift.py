#!/usr/bin/env python

import sys

from google.protobuf.compiler import plugin_pb2
from google.protobuf.descriptor_pb2 import DescriptorProto, EnumDescriptorProto, ServiceDescriptorProto

from jinja2 import Environment, FileSystemLoader
from jinja2 import Template

import functools

decapitalize = lambda s: s[:1].lower() + s[1:] if s else ''

def indent(level):
    """ Decorator to add a constant ident to a block, level times 4 spaces """

    def decorator(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            lines = f(*args, **kwargs).split("\n")
            return lines[0] + "\n" + "\n".join(
                [line if not line else level * "    " + line for line in lines[1:]])

        return wrapper

    return decorator

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def collect_enums(package, enums, template_env):
    _enums = {}

    for enum in enums:
        _enums[enum.name] = Enum(package, template_env, enum)

    return _enums

class Enum(object):
    """ Enum """

    def __init__(self, package, template_env, pb_enum):
        self._package = package
        self._template = template_env.get_template("enum.j2")
        self._name = pb_enum.name
        self._values = []

        for value in pb_enum.value:
            self._values.append(value.name)

    @indent(1)
    def __repr__(self):
         return self._template.render(name = self._name, values = self._values, package = self._package)

def collect_structs(package, structs, template_env):
    _structs = {}
    _responses = {}

    for struct in structs:
        if is_struct(struct):
            _structs[struct.name] = Struct(package, template_env, struct)
        elif is_response(struct):
            _responses[struct.name] = struct

    return _structs, _responses

def is_response(struct):
    return struct.name.endswith("Response")

def is_struct(struct):
    """ Check if the message is a data structure or an rpc request/response/result """
    return not struct.name.endswith("Request") and not struct.name.endswith("Response") and not struct.name.endswith("Result")

class Struct(object):
    """ Struct """

    def __init__(self, package, template_env, pb_struct):
        self._name = pb_struct.name
        self._template = template_env.get_template("struct.j2")
        self._fields = []
        self._rpc_type = package.title().replace(".", "_") + "_" + self._name

        for field in pb_struct.field:
            self._fields.append((field.json_name, extract_string_type(field)))

    @indent(1)
    def __repr__(self):
        return self._template.render(name = self._name, fields = self._fields, rpc_type = self._rpc_type)

def extract_string_type(field):
    types = {
            1: "Double",
            2: "Float",
            4: "UInt64",
            5: "Int",
            8: "Bool",
            9: "String",
            13: "UInt32",
            11: str(field.type_name.split(".")[-1]),
            14: str(field.type_name.split(".")[-1])
            }

    if (field.type in types):
        return types[field.type]
    else:
        return "UNKNOWN_TYPE"

def collect_methods(plugin_name, package, methods, structs, responses, template_env):
    _methods = {}

    for method in methods:
        if (is_completable(method, responses)):
            _methods[method.name] = CompletableMethod(plugin_name, package, template_env, method)
        elif (is_observable(method)):
            _methods[method.name] = ObservableMethod(plugin_name, package, template_env, method)
        else:
            _methods[method.name] = SingleMethod(plugin_name, package, template_env, method)

    return _methods

def is_completable(method, responses):
    method_output = method.output_type.split(".")[-1]
    method_response = responses[method_output]

    if (len(method_response.field) == 1 and method_response.field[0].type_name.endswith("Result")):
        return True

    return False

def is_observable(method):
    return method.server_streaming

class Method(object):
    """ Method """

    def __init__(self, plugin_name, package, pb_method):
        self._plugin_name = plugin_name
        self._package = package
        self._name = pb_method.name
        self._request_rpc_type = package.title().replace(".", "_") + "_" + pb_method.name + "Request"

    def __repr__(self):
        return "method: {}".format(self._name)

class CompletableMethod(Method):
    """ A completable method doesn't return any value, but either succeeds or fails """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._template = template_env.get_template("method_completable.j2")

    @indent(1)
    def __repr__(self):
        return self._template.render(name = self._name, package = self._package, plugin_name = self._plugin_name, request_rpc_type = self._request_rpc_type)

class SingleMethod(Method):
    """ A single method returns a value once """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._template = template_env.get_template("method_single.j2")
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float" # TODO

    @indent(1)
    def __repr__(self):
        return self._template.render(name = self._name, elem_type = self._type, request_rpc_type = self._request_rpc_type)

class ObservableMethod(Method):
    """ An observable method emits a stream of values """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._name = decapitalize(pb_method.name) + "Observable"
        self._capitalized_name = pb_method.name + "Observable"
        self._request_name = self._name + "Request"
        self._template = template_env.get_template("method_observable.j2")
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float" # TODO

    @indent(1)
    def __repr__(self):
        return self._template.render(name = self._name, capitalized_name = self._capitalized_name, elem_type = self._type, request_name = self._request_name, request_rpc_type = self._request_rpc_type)

class File(object):
    """ Represents the generated file """

    def __init__(self, plugin_name, template_env, enums, structs, methods):
        self._plugin_name = plugin_name
        self._template = template_env.get_template("file.j2")
        self._enums = enums
        self._structs = structs
        self._methods = methods

    def __repr__(self):
        return self._template.render(plugin_name = self._plugin_name, enums = self._enums.values(), structs = self._structs.values(), methods = self._methods.values())

def generate_code(request, response):
    template_loader = FileSystemLoader(searchpath="./{}".format(request.parameter))
    template_env = Environment(loader=template_loader)

    for proto_file in request.proto_file:
        plugin_name = proto_file.name.split('.')[0].capitalize()

        enums = collect_enums(proto_file.package, proto_file.enum_type, template_env)
        structs, responses = collect_structs(proto_file.package, proto_file.message_type, template_env)
        methods = collect_methods(plugin_name, proto_file.package, proto_file.service[0].method, structs, responses, template_env)

        swift_file = File(plugin_name, template_env, enums, structs, methods)

        # Fill response
        f = response.file.add()
        f.name = plugin_name + '.swift'
        f.content = str(swift_file)

if __name__ == '__main__':
    # Read request message from stdin
    data = (sys.stdin.buffer if sys.version_info[0] >= 3 else sys.stdin).read()
    #data = sys.stdin.read()

    # Parse request
    request = plugin_pb2.CodeGeneratorRequest()
    request.ParseFromString(data)

    # Create response
    response = plugin_pb2.CodeGeneratorResponse()

    # Generate code
    generate_code(request, response)

    # Serialize response message
    output = response.SerializeToString()

    # Write to stdout
    sys.stdout.buffer.write(output) if sys.version_info[0] >= 3 else sys.stdout.write(output)
