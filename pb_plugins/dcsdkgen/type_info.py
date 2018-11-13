import json
from os import environ


class TypeInfoFactory:

    def __init__(self):
        self._conversion_dict = self._load_conversions_dict()

    def create(self, field):
        return TypeInfo(field, self._conversion_dict)

    def _load_conversions_dict(self):
        try:
            _template_path = environ.get("TEMPLATE_PATH", "./")
            _conversion_dict_path = f"{_template_path}/type_conversions"
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
        inner_type = self._extract_inner_type()

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

    def _extract_inner_type(self):
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
    def is_result(self):
        """ Check if the field is a *Result """
        return self.name.endswith("Result")

    @property
    def is_primitive(self):
        """ Check if the field type is primitive (e.g. bool,
        float) or not (e.g message, enum) """
        return self._field.type not in {11, 14}

    @property
    def is_repeated(self):
        """ Check if the field is a repeated type (in which
        case it is converted to a list) """
        return self._field.label == 3
