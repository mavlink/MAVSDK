# -*- coding: utf-8 -*-


import functools
import re
from jinja2 import Environment, FileSystemLoader


def is_completable(method, responses):
    """ Checks if a method is completable """
    method_output = method.output_type.split(".")[-1]
    method_response = responses[method_output]

    if (1 == len(method_response.field) and
            method_response.field[0].type_name.endswith("Result")):
        return True

    return False


def is_observable(method):
    """ Checks if a method is an observable (stream) """
    return method.server_streaming


def is_response(struct):
    """ Checks if a name ends with response """
    return struct.name.endswith("Response")


def is_struct(struct):
    """ Check if the message is a data structure or an rpc
    request/response/result """
    return (not struct.name.endswith("Request") and
            not struct.name.endswith("Response") and
            not struct.name.endswith("Result"))


def extract_string_type(field):
    """ Extracts the string types """
    types = {
            1: "Double",
            2: "Float",
            4: "UInt64",
            5: "Int",
            8: "Bool",
            9: "String",
            13: "UInt32",
            11: str(field.type_name.split(".")[-1]),
            14: str(field.type_name.split(".")[-1])
            }

    if (field.type in types):
        return types[field.type]
    else:
        return "UNKNOWN_TYPE"


def decapitalize(s):
    """ Decapitalizes a string """
    return s[:1].lower() + s[1:] if s else ""


def indent(level):
    """ Decorator to add a constant indent to a block, level times 4 spaces """

    def decorator(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            lines = f(*args, **kwargs).split("\n")
            return lines[0] + "\n" + "\n".join(
                [line if not line else level * "    " +
                    line for line in lines[1:]])

        return wrapper

    return decorator


def letter_case_to_delimiter(_str_in):
    """ Converts TestAbcM to test_abc_m """
    return re.sub('(?<!^)(?=[A-Z])',
                  '_',
                  _str_in).lower()


def get_template_env(_searchpath):
    """ Generates the template environment """
    _template_env = Environment(loader=FileSystemLoader(
        searchpath=_searchpath))

    # Register some functions we need to access in the template
    _template_env.globals.update(
            letter_case_to_delimiter=letter_case_to_delimiter)

    return _template_env
