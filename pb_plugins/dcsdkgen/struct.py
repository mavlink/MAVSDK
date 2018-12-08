# -*- coding: utf-8 -*-


from .enum import Enum
from .name_parser import NameParser
from .utils import (is_request,
                    is_response,
                    is_struct,
                    Param,
                    type_info_factory)
from jinja2.exceptions import TemplateNotFound


class Struct(object):
    """ Struct """

    def __init__(self, plugin_name, package, template_env, pb_struct):
        self._plugin_name = NameParser(plugin_name)
        self._package = NameParser(package)
        self._template = template_env.get_template("struct.j2")
        self._name = NameParser(pb_struct.name)
        self._fields = []
        self._nested_enums = Enum.collect_enums(plugin_name, package, pb_struct.enum_type, template_env)
        self._nested_structs = Struct.collect_structs(plugin_name, package, pb_struct.nested_type, template_env)

        for field in pb_struct.field:
            self._fields.append(
                Param(NameParser(field.json_name),
                      type_info_factory.create(field)))

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     name=self._name,
                                     fields=self._fields,
                                     nested_enums=self._nested_enums,
                                     nested_structs=self._nested_structs)

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
