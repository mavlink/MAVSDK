# -*- coding: utf-8 -*-


from .utils import (indent,
                    extract_string_type,
                    is_response,
                    is_struct)


class Struct(object):
    """ Struct """

    def __init__(self, package, template_env, pb_struct):
        self._name = pb_struct.name
        self._template = template_env.get_template("struct.j2")
        self._fields = []
        self._rpc_type = (package.title().replace(".", "_")
                          + "_"
                          + self._name)

        for field in pb_struct.field:
            self._fields.append((field.json_name, extract_string_type(field)))

    @indent(1)
    def __repr__(self):
        return self._template.render(name=self._name,
                                     fields=self._fields,
                                     rpc_type=self._rpc_type)

    @staticmethod
    def collect_structs(package, structs, template_env):
        _structs = {}

        for struct in structs:
            if is_struct(struct):
                _structs[struct.name] = Struct(package, template_env, struct)

        return _structs

    @staticmethod
    def collect_responses(package, structs, template_env):
        _responses = {}

        for struct in structs:
            if is_response(struct):
                _responses[struct.name] = struct

        return _responses
