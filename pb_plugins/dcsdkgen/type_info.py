import json
from os import environ


class TypeInfo:
    _default_types = {
        1: "double",
        2: "float",
        4: "uint64_t",
        5: "int",
        8: "bool",
        9: "std::string",
        13: "uint32_t"
    }

    try:
        _conversion_dict_path = environ.get(
            "TEMPLATE_PATH", "./") + "/type_conversions"
        with open(_conversion_dict_path, "r") as handle:
            _conversion_dict = json.loads(handle.read())
    except FileNotFoundError:
        _conversion_dict = {}

    def __init__(self, field):
        self._field = field

    @property
    def name(self):
        """ Extracts the string types """
        type_id = self._field.type

        if not self.is_primitive:
            return str(self._field.type_name.split(".")[-1])
        elif type_id not in self._default_types:
            raise ValueError("UNKNOWN_TYPE")
        elif self._default_types[self._type_id] in self._conversion_dict:
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
