# -*- coding: utf-8 -*-
from .utils import (filter_out_result,
                    is_stream,
                    name_parser_factory,
                    no_return,
                    Param,
                    remove_subscribe,
                    type_info_factory)


class Method(object):
    """ Method """

    def __init__(
            self,
            plugin_name,
            package,
            method_description,
            pb_method,
            requests,
            responses):
        self._is_stream = False
        self._no_return = False
        self._has_result = False
        self._returns = False
        self._plugin_name = name_parser_factory.create(plugin_name)
        self._package = name_parser_factory.create(package)
        self._method_description = method_description
        self._name = name_parser_factory.create(pb_method.name)
        self.extract_params(pb_method, requests)
        self.extract_return_type_and_name(pb_method, responses)

    def extract_params(self, pb_method, requests):
        method_input = pb_method.input_type.split(".")[-1]
        request = requests[method_input]['struct']
        params_description = requests[method_input]['docs']['params']

        self._params = []

        field_id = 0
        for field in request.field:
            self._params.append(
                Param(
                    name=name_parser_factory.create(field.name),
                    type_info=type_info_factory.create(field),
                    description=params_description[field_id])
            )

            field_id += 1

    def extract_return_type_and_name(self, pb_method, responses):
        method_output = pb_method.output_type.split(".")[-1]
        response = responses[method_output]['struct']
        response_docs = responses[method_output]['docs']

        return_params = list(
            filter_out_result(
                response.field,
                response_docs['params']))

        if len(return_params) < len(response.field):
            self._has_result = True

        if len(return_params) > 1:
            raise Exception(
                "Responses cannot have more than 1 return parameter" +
                f"(and an optional '*Result')!\nError in {method_output}")

        if len(return_params) == 1:
            self._return_type = type_info_factory.create(
                return_params[0]['field'])
            self._return_name = name_parser_factory.create(
                return_params[0]['field'].json_name)
            self._return_description = return_params[0]['docs']

    @property
    def is_stream(self):
        return self._is_stream

    @property
    def no_return(self):
        return self._no_return

    @property
    def returns(self):
        return self._returns

    @property
    def plugin_name(self):
        return self._plugin_name

    @property
    def package(self):
        return self._package

    @property
    def name(self):
        return self._name

    @staticmethod
    def collect_methods(
            plugin_name,
            package,
            docs,
            methods,
            structs,
            requests,
            responses,
            template_env):
        """ Collects all methods for the plugin """
        _methods = {}

        method_id = 0
        for method in methods:
            # Extract method description
            method_description = docs['methods'][method_id].strip()

            # Check if stream
            if (is_stream(method)):
                _methods[method.name] = Stream(plugin_name,
                                               package,
                                               method_description,
                                               template_env,
                                               method,
                                               requests,
                                               responses)

            # Check if method is just a call
            elif (no_return(method, responses)):
                _methods[method.name] = Call(plugin_name,
                                             package,
                                             method_description,
                                             template_env,
                                             method,
                                             requests,
                                             responses)

            else:
                _methods[method.name] = Request(plugin_name,
                                                package,
                                                method_description,
                                                template_env,
                                                method,
                                                requests,
                                                responses)

            method_id += 1

        return _methods

    def __repr__(self):
        return "method: {}".format(self._name)


class Call(Method):
    """ A call method doesn't return any value, but either succeeds or fails
    """

    def __init__(
            self,
            plugin_name,
            package,
            method_description,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(
            plugin_name,
            package,
            method_description,
            pb_method,
            requests,
            responses)
        self._template = template_env.get_template("call.j2")
        self._no_return = True

    def __repr__(self):
        return self._template.render(name=self._name,
                                     params=self._params,
                                     plugin_name=self._plugin_name,
                                     package=self._package,
                                     method_description=self._method_description,
                                     has_result=self._has_result)


class Request(Method):
    """ Requests a value """

    def __init__(
            self,
            plugin_name,
            package,
            method_description,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(
            plugin_name,
            package,
            method_description,
            pb_method,
            requests,
            responses)
        self._template = template_env.get_template("request.j2")
        self._method_description = method_description
        self._returns = True

    def __repr__(self):
        return self._template.render(
            name=self._name,
            params=self._params,
            return_type=self._return_type,
            return_name=self._return_name,
            return_description=self._return_description,
            plugin_name=self._plugin_name,
            package=self._package,
            method_description=self._method_description,
            has_result=self._has_result)


class Stream(Method):
    """ A stream of values """

    def __init__(
            self,
            plugin_name,
            package,
            method_description,
            template_env,
            pb_method,
            requests,
            responses):
        super().__init__(
            plugin_name,
            package,
            method_description,
            pb_method,
            requests,
            responses)
        self._is_stream = True
        self._name = name_parser_factory.create(
            remove_subscribe(pb_method.name))
        self._template = template_env.get_template("stream.j2")

    def __repr__(self):
        return self._template.render(
            name=self._name,
            params=self._params,
            return_type=self._return_type,
            return_name=self._return_name,
            return_description=self._return_description,
            plugin_name=self._plugin_name,
            package=self._package,
            method_description=self._method_description,
            has_result=self._has_result)
