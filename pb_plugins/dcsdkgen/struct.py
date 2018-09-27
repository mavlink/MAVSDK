# -*- coding: utf-8 -*-


from .utils import (is_request,
                    is_response,
                    is_struct)
from .name_parser import NameParser
from .type_info import TypeInfo
from jinja2.exceptions import TemplateNotFound


class Struct(object):
    """ Struct """

    def __init__(self, plugin_name, package, template_env, pb_struct):
        self._plugin_name = NameParser(plugin_name)
        self._package = NameParser(package)
        self._template = template_env.get_template("struct.j2")
        self._name = NameParser(pb_struct.name)
        self._fields = []

        for field in pb_struct.field:
            self._fields.append(
                (NameParser(
                    field.json_name),
                    TypeInfo(field)))

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     name=self._name,
                                     fields=self._fields)

    @staticmethod
    def collect_structs(plugin_name, package, structs, template_env):
        _structs = {}

        for struct in structs:
            if is_struct(struct):
                _structs[struct.name] = Struct(
                    plugin_name, package, template_env, struct)

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
