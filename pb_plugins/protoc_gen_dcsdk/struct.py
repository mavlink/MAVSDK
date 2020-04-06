# -*- coding: utf-8 -*-


from .enum import Enum
from .utils import (is_request,
                    is_response,
                    is_struct,
                    name_parser_factory,
                    Param,
                    type_info_factory)
from jinja2.exceptions import TemplateNotFound


class Struct(object):
    """ Struct """

    def __init__(self, plugin_name, package,
                 template_env, pb_struct, struct_docs):
        self._plugin_name = name_parser_factory.create(plugin_name)
        self._package = name_parser_factory.create(package)
        self._template = template_env.get_template("struct.j2")
        self._struct_description = struct_docs['description'].strip(
        ) if struct_docs else None
        self._name = name_parser_factory.create(pb_struct.name)
        self._fields = []

        self._nested_enums = Enum.collect_enums(
            plugin_name,
            package,
            pb_struct.enum_type,
            template_env,
            struct_docs if 'enums' in struct_docs else None,
            parent_struct=self._name)
        self._nested_structs = Struct.collect_structs(
            plugin_name, package, pb_struct.nested_type, template_env, struct_docs if 'structs' in struct_docs else None)

        field_id = 0
        for field in pb_struct.field:

            default_value = None
            for field_descriptor in field.options.Extensions:
                if field_descriptor.name == "default_value":
                    default_value = field.options.Extensions[field_descriptor]

            self._fields.append(
                Param(name_parser_factory.create(field.json_name),
                      type_info_factory.create(field),
                      default_value=default_value,
                      description=struct_docs['params'][field_id]) if struct_docs else None)

            field_id += 1

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     struct_description=self._struct_description,
                                     name=self.name,
                                     fields=self._fields,
                                     nested_enums=self._nested_enums,
                                     nested_structs=self._nested_structs)

    @property
    def name(self):
        return self._name

    @staticmethod
    def collect_structs(plugin_name, package, structs, template_env, docs):
        _structs = {}

        struct_id = 0
        for struct in structs:
            if is_struct(struct):
                _structs[struct.name] = Struct(
                    plugin_name, package, template_env, struct, docs['structs'][struct_id] if docs else None)

            struct_id += 1

        return _structs

    @staticmethod
    def collect_requests(package, structs, docs):
        _requests = {}

        struct_id = 0
        for struct in structs:
            if is_request(struct):
                _requests[struct.name] = {
                    'struct': struct, 'docs': docs['structs'][struct_id]}

            struct_id += 1

        return _requests

    @staticmethod
    def collect_responses(package, structs, docs):
        _responses = {}

        struct_id = 0
        for struct in structs:
            if is_response(struct):
                _responses[struct.name] = {
                    'struct': struct, 'docs': docs['structs'][struct_id]}

            struct_id += 1

        return _responses
