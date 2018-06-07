#!/usr/bin/env python

import sys

from google.protobuf.compiler import plugin_pb2
from google.protobuf.descriptor_pb2 import DescriptorProto, EnumDescriptorProto, ServiceDescriptorProto

from jinja2 import Template

decapitalize = lambda s: s[:1].lower() + s[1:] if s else ''

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def collect_enums(package, enums):
    _enums = {}

    for enum in enums:
        _enums[enum.name] = Enum(package, enum)

    return _enums

class Enum(object):
    """ Enum """

    tpl = Template("""\
public enum {{ name }} {
{% for value in values %}\
    case {{ value }}
{% endfor %}\

    internal var rpc{{ name }}: {{ package }} {
        switch self {
{% for value in values %}\
        case .{{ value }}:
            return .{{ value }}
{% endfor %}\
        }
    }

    internal static func translateFromRPC(_ rpc{{ name }}: {{ package }}) -> {{ name }} {
        switch rpc{{ name }} {
{% for value in values %}\
        case .{{ value }}:
            return .{{ value }}
{% endfor %}\
        }
    }
}
""")

    def __init__(self, package, pb_enum):
        self._is_used = False
        self._package = package
        self._name = pb_enum.name
        self._values = []

        for value in pb_enum.value:
            self._values.append(value.name)

    def __repr__(self):
         return self.tpl.render(name = self._name, values = self._values, package = self._package)

def collect_structs(package, structs):
    _structs = {}
    _responses = {}

    for struct in structs:
        if is_struct(struct):
            _structs[struct.name] = Struct(package, struct)
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

    tpl = Template("""\
public struct {{ name }}: Equatable {
{% for field in fields %}\
    public let {{ field[0] }}: {{ field[1] }}
{% endfor %}\

    public init({% for field in fields[:-1] %}{{ field[0] }}: {{ field[1] }}, {% endfor %}{{ fields[-1][0] }}: {{ fields[-1][1] }}) {
{% for field in fields %}\
        self.{{ field[0] }} = {{ field[0] }}
{% endfor %}\
    }

    internal var rpc{{ name }}: {{ rpc_type }} {
        var rpc{{ name }} = {{ rpc_type }}()

{% for field in fields %}\
        rpc {{ name }}.{{ field[0] }} = {{ field[0] }}
{% endfor %}\

        return rpc{{ name }}
    }

    internal static func translateFromRPC(_ rpc{{ name }}: {{ rpc_type }}) -> {{ name }} {
        return {{ name }}({% for field in fields[:-1] %}{{ field[0] }}: rpc{{ name }}.{{ field[0] }}, {% endfor %}{{ fields[-1][0] }}: rpc{{ name }}.{{ fields[-1][0] }})
    }

    public static func == (lhs: {{ name }}, rhs:: {{ name }}) -> Bool {
        return lhs.{{ fields[0][0] }} == rhs.{{ fields[0][0] }}
{% for field in fields[1:] %}\
            && lhs.{{ field[0] }} == rhs.{{ field[0] }}
{% endfor %}\
    }
}
""")

    def __init__(self, package, pb_struct):
        self._name = pb_struct.name
        self._fields = []
        self._rpc_type = package.title().replace(".", "_") + "_" + self._name

        for field in pb_struct.field:
            self._fields.append((field.json_name, extract_string_type(field)))

    def __repr__(self):
        return self.tpl.render(name = self._name, fields = self._fields, rpc_type = self._rpc_type)

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

def collect_methods(plugin_name, package, methods, structs, responses):
    _methods = {}

    for method in methods:
        if (is_completable(method, responses)):
            _methods[method.name] = CompletableMethod(plugin_name, package, method)
        elif (is_observable(method)):
            _methods[method.name] = ObservableMethod(plugin_name, package, method)
        else:
            _methods[method.name] = SingleMethod(plugin_name, package, method)

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

    tpl = Template("""\
public func {{ name }}() -> Completable {
    return Completable.create { completable in
    let {{ name }}Request = {{ request_rpc_type }}()

    do {
        let {{ name }}Response = try self.service.{{ name.lower() }}({{ name }}Request)
        if ({{ name }}Response.{{ plugin_name.title() }}Result.result == {{ package }}_{{ plugin_name.title() }}Result.Result.success) {
            completable(.completed)
        } else {
            completable(.error(\"Cannot {{ name }}: \\({{ name }}Response.{{ plugin_name.title() }}Result.result)\"))
        }
    } catch {
        completable(.error(error))
    }
    return Disposables.create()
}
""")

    def __repr__(self):
        return self.tpl.render(name = self._name, package = self._package, plugin_name = self._plugin_name, request_rpc_type = self._request_rpc_type)

class SingleMethod(Method):
    """ A single method returns a value once """

    tpl = Template("""\
public func {{ name }}() -> Single<{{ elem_type }}> {
    return Single<{{ elem_type }}>.create { single in
        let {{ name }}Request = {{ request_rpc_type }}()

        do {
            let {{ name }}Response = try self.service.{{ name.lower() }}({{ name }}Request)
            single(.success({{ name }}Response.BLAH))
        } catch {
            single(.error(error))
        }

        return Disposables.create()
    }
}
""")

    def __init__(self, plugin_name, package, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float" # TODO

    def __repr__(self):
        return self.tpl.render(name = self._name, elem_type = self._type, request_rpc_type = self._request_rpc_type)

class ObservableMethod(Method):
    """ An observable method emits a stream of values """

    tpl = Template("""\
public lazy var {{ name }}: Observable<{{ elem_type }}> = create{{ capitalized_name }}()

private func create{{ name }}() -> Observable<{{ elem_type }}> {
    return Observable.create { observer in
        let {{ request_name }} = {{ request_rpc_type }}()

        do {
            let call = try self.service.subscribe{{ name.lower() }}({{ request_name }}, completion: nil)
            while let response = try? call.receive() {
                let position = Position(latitudeDeg: response.position.latitudeDeg, longitudeDeg: response.position.longitudeDeg, absoluteAltitudeM: response.position.absoluteAltitudeM, relativeAltitudeM: response.position.relativeAltitudeM)

                observer.onNext(position)
            }
        } catch {
            observer.onError("Failed to subscribe to {{ name }} stream")
        }

        return Disposables.create()
    }.subscribeOn(self.scheduler)
}
""")

    def __init__(self, plugin_name, package, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._name = decapitalize(pb_method.name) + "Observable"
        self._capitalized_name = pb_method.name + "Observable"
        self._request_name = self._name + "Request"
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float" # TODO

    def __repr__(self):
        return self.tpl.render(name = self._name, capitalized_name = self._capitalized_name, elem_type = self._type, request_name = self._request_name, request_rpc_type = self._request_rpc_type)

class File(object):
    """ Represents the generated file """

    tpl = Template("""\
import Foundation
import gRPC
import RxSwift

public class {{ plugin_name }} {
{% for enum in enums %}\
    {{ enum }}
{% endfor %}\

{% for struct in structs %}\
    {{ struct }}
{% endfor %}\

{% for method in methods %}\
    {{ method }}
{% endfor %}\
}\
""")

    def __init__(self, plugin_name, enums, structs, methods):
        self._plugin_name = plugin_name
        self._enums = enums
        self._structs = structs
        self._methods = methods

    def __repr__(self):
        return self.tpl.render(plugin_name = self._plugin_name, enums = self._enums.values(), structs = self._structs.values(), methods = self._methods.values())

def generate_code(request, response):
    for proto_file in request.proto_file:
        plugin_name = proto_file.name.split('.')[0].capitalize()

        enums = collect_enums(proto_file.package, proto_file.enum_type)
        structs, responses = collect_structs(proto_file.package, proto_file.message_type)
        methods = collect_methods(plugin_name, proto_file.package, proto_file.service[0].method, structs, responses)

        swift_file = File(plugin_name, enums, structs, methods)

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
