# -*- coding: utf-8 -*-
from .name_parser import NameParser


class File(object):
    """ Represents the generated file """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            enums,
            structs,
            methods,
            has_result):
        self._package = NameParser(package)
        self._plugin_name = NameParser(plugin_name)
        self._template = template_env.get_template("file.j2")
        self._enums = enums
        self._structs = structs
        self._methods = methods
        self._has_result = has_result

    def __repr__(self):
        return self._template.render(package=self._package,
                                     plugin_name=self._plugin_name,
                                     enums=self._enums.values(),
                                     structs=self._structs.values(),
                                     methods=self._methods.values(),
                                     has_result=self._has_result)
