# -*- coding: utf-8 -*-


import re
from jinja2 import Environment, FileSystemLoader


def is_completable(method, responses):
    """ Checks if a method is completable """
    method_output = method.output_type.split(".")[-1]
    method_response = responses[method_output]

    if (1 == len(method_response.field) and
            method_response.field[0].type_name.endswith("Result")):
        return True

    if (0 == len(method_response.field)):
        return True

    return False


def is_observable(method):
    """ Checks if a method is an observable (stream) """
    return method.server_streaming


def is_request(struct):
    """ Checks if a name ends with request """
    return struct.name.endswith("Request")


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


def is_primitive_type(field):
    """ Check if the field type is primitive (e.g. bool,
    float) or not (e.g message, enum) """
    return (not field.type in {11, 14})


def filter_out_result(fields):
    """ Filters out the result fields (".*Result$") """
    for field in fields:
        if not is_result(field):
            yield field


def is_result(field):
    """ Check if the field is a *Result """
    return extract_string_type(field).endswith("Result")


def decapitalize(s):
    """ Decapitalizes a string """
    return s[:1].lower() + s[1:] if s else ""


def remove_subscribe(name):
    return name.replace("subscribe", "")


def jinja_indent(_in_str, level):
    """ Indentation helper for the jinja2 templates """

    _in_str = str(_in_str)

    return "\n".join(
        ["" if not line else
         level * "    " + line
         for line in _in_str.split("\n")]
    )


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
        letter_case_to_delimiter=letter_case_to_delimiter,
        indent=jinja_indent,
        remove_subscribe=remove_subscribe)

    return _template_env
