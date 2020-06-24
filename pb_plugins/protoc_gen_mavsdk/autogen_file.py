# -*- coding: utf-8 -*-
from .utils import name_parser_factory


class File(object):
    """ Represents the generated file """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            docs,
            enums,
            structs,
            methods,
            has_result):
        self._package = name_parser_factory.create(package)
        self._plugin_name = name_parser_factory.create(plugin_name)
        self._template = template_env.get_template("file.j2")
        self._class_description = docs['class'].strip()
        self._enums = enums
        self._structs = structs
        self._methods = methods
        self._has_result = has_result

    def __repr__(self):
        return self._template.render(package=self._package,
                                     plugin_name=self._plugin_name,
                                     class_description=self._class_description,
                                     enums=self._enums.values(),
                                     structs=self._structs.values(),
                                     methods=self._methods.values(),
                                     has_result=self._has_result)
