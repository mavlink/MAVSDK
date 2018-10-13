import unittest

from dcsdkgen.name_parser import NameParser

class TestNameParser(unittest.TestCase):

    def setUp(self):
        self.arbitrary_str = "arBiTrarY_NaME"
        self.simple_upper_camel_case_str = "SimpleString"

    def test_uppercase(self):
        name = NameParser(self.arbitrary_str)
        self.assertEqual('ARBITRARY_NAME', name.uppercase)

    def test_lowercase(self):
        name = NameParser(self.arbitrary_str)
        self.assertEqual('arbitrary_name', name.lowercase)

    def test_simple_upper_camel_case(self):
        name = NameParser(self.simple_upper_camel_case_str)
        self.assertEqual(self.simple_upper_camel_case_str, name.upper_camel_case)

    def test_simple_lower_camel_case(self):
        name = NameParser(self.simple_upper_camel_case_str)
        self.assertEqual('simpleString', name.lower_camel_case)

    def test_simple_upper_snake_case(self):
        name = NameParser(self.simple_upper_camel_case_str)
        self.assertEqual('Simple_String', name.upper_snake_case)

    def test_simple_lower_snake_case(self):
        name = NameParser(self.simple_upper_camel_case_str)
        self.assertEqual('simple_string', name.lower_snake_case)

if __name__ == '__main__':
    unittest.main()
