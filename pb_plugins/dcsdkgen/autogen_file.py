# -*- coding: utf-8 -*-


class File(object):
    """ Represents the generated file """

    def __init__(self, plugin_name, template_env, enums, structs, methods):
        self._plugin_name = plugin_name
        self._template = template_env.get_template("file.j2")
        self._enums = enums
        self._structs = structs
        self._methods = methods

    def __repr__(self):
        return self._template.render(plugin_name=self._plugin_name,
                                     enums=self._enums.values(),
                                     structs=self._structs.values(),
                                     methods=self._methods.values())
