import re


class NameParser:

    def __init__(self, name):
        """ Tries to detect the input format and save name as upper camel case.
            This assumes that the input is either given as lower_snake_case,
            upper_snake_case, UPPERCASE, UPPERCASE_SNAKE, UpperCamelCase or 
            lowerCamelCase. Other inputs are undefined behavior.
        """

        if '_' in name: # Snake case
            words = name.split('_')
            self._name = ''.join(word.title() for word in words)
        else:
            if name[0].isupper():
                if name.isupper(): # All uppercase
                    self._name = name.title()
                else: # Already UpperCamelCase
                    self._name = name
            else: # lowerCamelCase
                self._name = name[0].upper() + name[1:]

    @property
    def uppercase(self):
        return self.upper_snake_case.upper()

    @property
    def upper_camel_case(self):
        return self._name

    @property
    def lower_camel_case(self):
        return self._name[0].lower() + self._name[1:] if self._name else ""

    @property
    def upper_snake_case(self):
        return re.sub('(?<!^)(?=[A-Z])',
                      '_',
                      self._name)

    @property
    def lower_snake_case(self):
        return self.upper_snake_case.lower()
