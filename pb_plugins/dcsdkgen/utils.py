# -*- coding: utf-8 -*-
from jinja2 import Environment, FileSystemLoader
from .name_parser import NameParserFactory
from .type_info import TypeInfoFactory


name_parser_factory = NameParserFactory()
type_info_factory = TypeInfoFactory()


class Param:
    def __init__(self, name, type_info):
        self.name = name
        self.type_info = type_info


def no_return(method, responses):
    """ Checks if a method is completable """
    method_output = method.output_type.split(".")[-1]
    method_response = responses[method_output]

    if (1 == len(method_response.field) and
            type_info_factory.create(method_response.field[0]).is_result):
        return True

    if (0 == len(method_response.field)):
        return True

    return False


def is_stream(method):
    """ Checks if a method is an observable (stream) """
    return method.server_streaming


def is_request(struct):
    """ Checks if a name ends with request """
    return struct.name.endswith("Request")


def is_response(struct):
    """ Checks if a name ends with response """
    return struct.name.endswith("Response")


def is_struct(struct):
    """ Checks if the message is a data structure or an rpc
    request/response"""
    return (not struct.name.endswith("Request") and
            not struct.name.endswith("Response"))


def filter_out_result(fields):
    """ Filters out the result fields (".*Result$") """
    for field in fields:
        if not field.type_name.endswith("Result"):
            yield field


def has_result(structs):
    """ Checks if at least one struct is a *Result$. 
        The expected input is a list of struct names. """
    for struct in structs:
        if struct.endswith("Result"):
            return True

    return False


def remove_subscribe(name):
    return name.replace("Subscribe", "")


def jinja_indent(_in_str, level):
    """ Indentation helper for the jinja2 templates """

    _in_str = str(_in_str)

    return "\n".join(
        ["" if not line else
         level * "    " + line
         for line in _in_str.split("\n")]
    )


def get_template_env(_searchpath):
    """ Generates the template environment """
    _template_env = Environment(loader=FileSystemLoader(
        searchpath=_searchpath))

    # Register some functions we need to access in the template
    _template_env.globals.update(indent=jinja_indent)

    return _template_env
