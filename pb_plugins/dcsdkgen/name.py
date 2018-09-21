import re


class NameParser:

    def __init__(self, name):
        self._name = name

    @property
    def upper_camel_case(self):
        return self._name

    @property
    def lower_camel_case(self):
        return self._name[0].lower() + self._name[1:] if self._name else ""

    @property
    def snake_case(self):
        return re.sub('(?<!^)(?=[A-Z])',
                      '_',
                      self._name).lower()
