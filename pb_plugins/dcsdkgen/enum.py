# -*- coding: utf-8 -*-
from .name_parser import NameParser


class Enum(object):
    """ Enum """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            pb_enum,
            parent_struct=None):
        self._plugin_name = NameParser(plugin_name)
        self._package = NameParser(package)
        self._template = template_env.get_template("enum.j2")
        self._name = NameParser(pb_enum.name)
        self._values = []
        self._parent_struct = parent_struct

        for value in pb_enum.value:
            self._values.append(NameParser(value.name))

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     name=self._name,
                                     values=self._values,
                                     parent_struct=self._parent_struct)

    @staticmethod
    def collect_enums(
            plugin_name,
            package,
            enums,
            template_env,
            parent_struct=None):
        _enums = {}

        for enum in enums:
            _enums[enum.name] = Enum(
                plugin_name, package, template_env, enum, parent_struct)

        return _enums
