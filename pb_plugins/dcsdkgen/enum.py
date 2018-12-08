# -*- coding: utf-8 -*-
from .name_parser import NameParser


class Enum(object):
    """ Enum """

    def __init__(self, plugin_name, package, template_env, pb_enum):
        self._plugin_name = NameParser(plugin_name)
        self._package = NameParser(package)
        self._template = template_env.get_template("enum.j2")
        self._name = NameParser(pb_enum.name)
        self._values = []

        for value in pb_enum.value:
            self._values.append(NameParser(value.name))

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     package=self._package,
                                     name=self._name,
                                     values=self._values)

    @staticmethod
    def collect_enums(plugin_name, package, enums, template_env):
        _enums = {}

        for enum in enums:
            _enums[enum.name] = Enum(plugin_name, package, template_env, enum)

        return _enums
