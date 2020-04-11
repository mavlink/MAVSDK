# -*- coding: utf-8 -*-
from .utils import name_parser_factory


class Enum(object):
    """ Enum """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            pb_enum,
            enum_docs,
            parent_struct=None):
        self._plugin_name = name_parser_factory.create(plugin_name)
        self._package = name_parser_factory.create(package)
        self._template = template_env.get_template("enum.j2")
        self._enum_description = enum_docs['description'].strip(
        ) if enum_docs else None
        self._name = name_parser_factory.create(pb_enum.name)
        self._values = []
        self._parent_struct = parent_struct

        value_id = 0
        for value in pb_enum.value:
            # If the enum value contains the enum name as a prefix, remove it.
            # For example, if the enum "GpsFix" has a value called "GPS_FIX_NO_GPS",
            # we remove the prefix and the value becomes "NO_GPS"
            tmp_value_name = name_parser_factory.create(value.name)
            if tmp_value_name.upper_camel_case.startswith(self._name.upper_camel_case):
                value_name = name_parser_factory.create(tmp_value_name.lower_snake_case[len(self._name.lower_snake_case) + 1:])
                has_prefix = True
            else:
                value_name = tmp_value_name
                has_prefix = False

            self._values.append({'name': value_name, 'description': enum_docs['params'][value_id], 'has_prefix': has_prefix})
            value_id += 1

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     enum_description=self._enum_description,
                                     name=self._name,
                                     values=self._values,
                                     parent_struct=self._parent_struct)

    @staticmethod
    def collect_enums(
            plugin_name,
            package,
            enums,
            template_env,
            docs,
            parent_struct=None):
        _enums = {}

        enum_id = 0
        for enum in enums:
            _enums[enum.name] = Enum(plugin_name,
                                     package,
                                     template_env,
                                     enum,
                                     docs['enums'][enum_id] if docs else None,
                                     parent_struct)

            enum_id += 1

        return _enums
