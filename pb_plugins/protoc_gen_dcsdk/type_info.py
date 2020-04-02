import json


class TypeInfoFactory:
    _conversion_dict = {}

    def create(self, field):
        return TypeInfo(field, self._conversion_dict)

    def set_template_path(self, template_path):
        self._conversion_dict = self._load_conversions_dict(template_path)

    def _load_conversions_dict(self, template_path):
        try:
            _conversion_dict_path = f"{template_path}/type_conversions"
            with open(_conversion_dict_path, "r") as handle:
                return json.loads(handle.read())
        except FileNotFoundError:
            return {}


class TypeInfo:
    _default_types = {
        1: "double",
        2: "float",
        3: "int64_t",
        4: "uint64_t",
        5: "int32_t",
        8: "bool",
        9: "std::string",
        12: "std::byte",
        13: "uint32_t",
        "repeated": {"prefix": "std::vector<", "suffix": ">"}
    }

    def __init__(self, field, conversion_dict):
        self._field = field
        self._conversion_dict = conversion_dict

    @property
    def name(self):
        """ Extracts the string types """
        inner_type = self.inner_name

        if self.is_repeated:
            if "repeated" in self._conversion_dict:
                prefix = prefix = self._conversion_dict["repeated"]["prefix"]
                suffix = self._conversion_dict["repeated"]["suffix"]
            else:
                prefix = self._default_types["repeated"]["prefix"]
                suffix = self._default_types["repeated"]["suffix"]

            return f"{prefix}{inner_type}{suffix}"
        else:
            return inner_type

    @property
    def inner_name(self):
        """ Extracts inner type name. Only different for repeated types,
        where e.g. type name "std::vector<int>" has "int"
        as its inner_name """
        type_id = self._field.type

        if not self.is_primitive:
            return str(self._field.type_name.split(".")[-1])
        elif type_id not in self._default_types:
            raise ValueError("UNKNOWN_TYPE")
        elif self._default_types[type_id] in self._conversion_dict:
            return self._conversion_dict[self._default_types[type_id]]
        else:
            return self._default_types[type_id]

    @property
    def parent_type(self):
        """ Extracts parent type. This assumes that nesting is not deeper
        than 1 level.

        Note that it also assumes that the package name has 3 words. It
        will start with a '.', giving something like:
        '.io.mavlink.mavsdk.ActionResult.Result'."""
        if self.is_primitive:
            return None

        type_tree = self._field.type_name.split(".")

        if len(type_tree) <= 5:
            return None

        return str(type_tree[-2])

    @property
    def is_result(self):
        """ Check if the field is a *Result """
        return self.name.endswith("Result")

    @property
    def is_primitive(self):
        """ Check if the field type is primitive (e.g. bool,
        float) or not (e.g message, enum) """
        return self._field.type not in {11, 14}

    @property
    def is_enum(self):
        """ Check if the field type is an enum"""
        return self._field.type == 14

    @property
    def is_repeated(self):
        """ Check if the field is a repeated type (in which
        case it is converted to a list) """
        return self._field.label == 3
