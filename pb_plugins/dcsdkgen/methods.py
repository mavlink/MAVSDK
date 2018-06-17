# -*- coding: utf-8 -*-


from .utils import (decapitalize,
                    is_completable,
                    is_observable)


class Method(object):
    """ Method """

    def __init__(self, plugin_name, package, pb_method):
        self._plugin_name = plugin_name
        self._package = package
        self._name = pb_method.name
        self._request_rpc_type = (package.title().replace(".", "_")
                                  + "_"
                                  + pb_method.name
                                  + "Request")

    @staticmethod
    def collect_methods(
            plugin_name,
            package,
            methods,
            structs,
            responses,
            template_env):
        """ Collects all methods for the plugin """
        _methods = {}
        for method in methods:
            # Check if method is completable
            if (is_completable(method, responses)):
                _methods[method.name] = CompletableMethod(plugin_name,
                                                          package,
                                                          template_env,
                                                          method)

            # Check if method is observable
            elif (is_observable(method)):
                _methods[method.name] = ObservableMethod(plugin_name,
                                                         package,
                                                         template_env,
                                                         method)

            else:
                _methods[method.name] = SingleMethod(plugin_name,
                                                     package,
                                                     template_env,
                                                     method)

        return _methods

    def __repr__(self):
        return "method: {}".format(self._name)


class CompletableMethod(Method):
    """ A completable method doesn't return any value,
    but either succeeds or fails """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._template = template_env.get_template("method_completable.j2")

    def __repr__(self):
        return self._template.render(name=self._name,
                                     package=self._package,
                                     plugin_name=self._plugin_name,
                                     request_rpc_type=self._request_rpc_type)


class SingleMethod(Method):
    """ A single method returns a value once """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._template = template_env.get_template("method_single.j2")
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float"  # TODO

    def __repr__(self):
        return self._template.render(name=self._name,
                                     elem_type=self._type,
                                     plugin_name=self._plugin_name,
                                     request_rpc_type=self._request_rpc_type)


class ObservableMethod(Method):
    """ An observable method emits a stream of values """

    def __init__(self, plugin_name, package, template_env, pb_method):
        super().__init__(plugin_name, package, pb_method)
        self._name = decapitalize(pb_method.name) + "Observable"
        self._capitalized_name = pb_method.name + "Observable"
        self._request_name = self._name + "Request"
        self._template = template_env.get_template("method_observable.j2")
        self.extract_field_type_and_name(pb_method)

    def extract_field_type_and_name(self, pb_method):
        self._type = "Float"  # TODO

    def __repr__(self):
        return self._template.render(name=self._name,
                                     capitalized_name=self._capitalized_name,
                                     elem_type=self._type,
                                     plugin_name=self._plugin_name,
                                     request_name=self._request_name,
                                     request_rpc_type=self._request_rpc_type)

