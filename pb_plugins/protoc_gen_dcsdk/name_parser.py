import json
import re

from os import environ

class NameParserFactory:
    _initialisms = []

    def create(self, name):
        return NameParser(name, self._initialisms)

    def set_template_path(self, template_path):
        self._initialisms = self._load_initialisms(template_path)

    def _load_initialisms(self, template_path):
        try:
            _initialisms_path = f"{template_path}/initialisms"
            with open(_initialisms_path, "r") as handle:
                return json.loads(handle.read())
        except FileNotFoundError:
            return []


class NameParser:

    def __init__(self, name, initialisms):
        """ Splits the input name at each '_' or each uppercase letter,
            unless the word is fully capitalized.
        """

        if '_' in name:
            self._words = name.split('_')
        elif name.isupper():
            self._words = [name]
        else:
            self._words = re.findall('[a-zA-Z][^A-Z]*', name)

        self._initialisms = initialisms

    @property
    def uppercase(self):
        return '_'.join(word.upper() for word in self._words)

    @property
    def upper_camel_case(self):
        formatted_words = list(map(lambda word: word.title(), self._words))
        formatted_words = list(map(lambda word: word.upper() if word.upper() in self._initialisms else word, formatted_words))
        return ''.join(formatted_words)

    @property
    def lower_camel_case(self):
        formatted_words = list(map(lambda word: word.title(), self._words))
        formatted_words = list(map(lambda word: word.upper() if word.upper() in self._initialisms else word, formatted_words))
        return formatted_words[0].lower() + ''.join(formatted_words[1:])

    @property
    def upper_snake_case(self):
        formatted_words = list(map(lambda word: word.title(), self._words))
        formatted_words = list(map(lambda word: word.upper() if word.upper() in self._initialisms else word, formatted_words))
        return '_'.join(formatted_words)

    @property
    def lower_snake_case(self):
        return '_'.join(word.lower() for word in self._words)
