# -*- coding: utf-8 -*-

from .utils import (decapitalize,
                    remove_subscribe,
                    extract_string_type,
                    is_primitive_type,
                    filter_out_result,
                    is_completable,
                    is_observable)


class Method(object):
    """ Method """

    def __init__(
            self,
            plugin_name,
            package,
            pb_method,
            requests,
            responses):
        self._plugin_name = plugin_name
        self._package = package
        self._name = decapitalize(pb_method.name)
        self._request_rpc_type = (package.title().replace(".", "_")
                                  + "_"
                                  + pb_method.name
                                  + "Request")
        self.extract_params(pb_method, requests)
        self.extract_return_type_and_name(pb_method, responses)
        self._is_observable = False
        self._is_completable = False
        self._is_single = False

    def extract_params(self, pb_method, requests):
        method_input = pb_method.input_type.split(".")[-1]
        request = requests[method_input]
        nb_fields = len(request.field)

        self._params = []

        for field in request.field:
            self._params.append({"name": field.name,
                                 "type": extract_string_type(field),
                                 "is_primitive": is_primitive_type(field)})

    def extract_return_type_and_name(self, pb_method, responses):
        method_output = pb_method.output_type.split(".")[-1]
        response = responses[method_output]
        nb_fields = len(response.field)

        return_params = list(filter_out_result(response.field))
        if len(return_params) > 1:
            raise Exception(
                "Responses cannot have more than 1 return parameter (and an optional '*Result')!\nError in {}".format(method_output))

        if len(return_params) == 1:
            self._return_type = extract_string_type(return_params[0])
            self._is_return_type_primitive = is_primitive_type(
                return_params[0])
            self._return_name = return_params[0].json_name

    @property
    def is_observable(self):
        return self._is_observable

    @property
    def is_completable(self):
        return self._is_completable

    @property
    def is_single(self):
        return self._is_single

    @property
    def plugin_name(self):
        return self._plugin_name

    @property
    def package(self):
        return self._package

    @property
    def name(self):
        return self._name

    @property
    def request_rpc_type(self):
        return self._request_rpc_type

    @staticmethod
    def collect_methods(
            plugin_name,
            package,
            methods,
            structs,
            requests,
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
                                                          method,
                                                          requests,
                                                          responses)

            # Check if method is observable
            elif (is_observable(method)):
                _methods[method.name] = ObservableMethod(plugin_name,
                                                         package,
                                                         template_env,
                                                         method,
                                                         requests,
                                                         responses)

            else:
                _methods[method.name] = SingleMethod(plugin_name,
                                                     package,
                                                     template_env,
                                                     method,
                                                     requests,
                                                     responses)

        return _methods

    def __repr__(self):
        return "method: {}".format(self._name)


class CompletableMethod(Method):
    """ A completable method doesn't return any value,
    but either succeeds or fails """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(plugin_name, package, pb_method, requests, responses)
        self._template = template_env.get_template("method_completable.j2")
        self._is_completable = True

    def __repr__(self):
        return self._template.render(name=self._name,
                                     package=self._package,
                                     params=self._params,
                                     plugin_name=self._plugin_name,
                                     request_rpc_type=self._request_rpc_type)


class SingleMethod(Method):
    """ A single method returns a value once """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(plugin_name, package, pb_method, requests, responses)
        self._template = template_env.get_template("method_single.j2")
        self._is_single = True

    def __repr__(self):
        return self._template.render(name=self._name,
                                     params=self._params,
                                     return_type=self._return_type,
                                     return_name=self._return_name,
                                     is_return_type_primitive=self._is_return_type_primitive,
                                     plugin_name=self._plugin_name,
                                     request_rpc_type=self._request_rpc_type)


class ObservableMethod(Method):
    """ An observable method emits a stream of values """

    def __init__(
            self,
            plugin_name,
            package,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(plugin_name, package, pb_method, requests, responses)
        self._name = remove_subscribe(decapitalize(pb_method.name))
        self._capitalized_name = pb_method.name
        self._request_name = self._name + "Request"
        self._template = template_env.get_template("method_observable.j2")
        self._is_observable = True

    def __repr__(self):
        return self._template.render(name=self._name,
                                     capitalized_name=self._capitalized_name,
                                     params=self._params,
                                     return_type=self._return_type,
                                     return_name=self._return_name,
                                     is_return_type_primitive=self._is_return_type_primitive,
                                     plugin_name=self._plugin_name,
                                     request_name=self._request_name,
                                     request_rpc_type=self._request_rpc_type)
