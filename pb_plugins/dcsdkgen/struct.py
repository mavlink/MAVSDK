# -*- coding: utf-8 -*-


from .utils import (extract_string_type,
                    is_request,
                    is_response,
                    is_struct)
from .name import Name
from jinja2.exceptions import TemplateNotFound


class Struct(object):
    """ Struct """

    def __init__(self, package, template_env, pb_struct):
        try:
            self._template = template_env.get_template("struct.j2")
        except TemplateNotFound:
            # We don't always generate code related to structs (but still
            # collect them)
            pass

        self._name = Name(pb_struct.name)
        self._fields = []
        self._rpc_type = (package.title().replace(".", "_")
                          + "_"
                          + self._name)

        for field in pb_struct.field:
            self._fields.append((field.json_name, extract_string_type(field)))

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
    def collect_requests(package, structs):
        _requests = {}

        for struct in structs:
            if is_request(struct):
                _requests[struct.name] = struct

        return _requests

    @staticmethod
    def collect_responses(package, structs):
        _responses = {}

        for struct in structs:
            if is_response(struct):
                _responses[struct.name] = struct

        return _responses
