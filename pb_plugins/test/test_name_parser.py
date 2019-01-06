import unittest

from dcsdkgen.name_parser import NameParser

class TestNameParser(unittest.TestCase):

    def setUp(self):
        self.upper_camel_input = "FormattedName"
        self.lower_camel_input = "formattedName"
        self.upper_snake_input = "Formatted_Name"
        self.lower_snake_input = "formatted_name"
        self.only_upper_snake_input = "FORMATTED_NAME"
        self.only_upper_input = "NAME"

        self.uppercase = "FORMATTED_NAME"
        self.upper_camel_case = "FormattedName"
        self.lower_camel_case = "formattedName"
        self.upper_snake_case = "Formatted_Name"
        self.lower_snake_case = "formatted_name"

        self.single_uppercase = "NAME"
        self.single_upper_case = "Name"
        self.single_lower_case = "name"

    def test_upper_camel_to_uppercase(self):
        name = NameParser(self.upper_camel_input)
        self.assertEqual(self.uppercase, name.uppercase)

    def test_lower_camel_to_uppercase(self):
        name = NameParser(self.lower_camel_input)
        self.assertEqual(self.uppercase, name.uppercase)

    def test_upper_snake_to_uppercase(self):
        name = NameParser(self.upper_snake_input)
        self.assertEqual(self.uppercase, name.uppercase)

    def test_lower_snake_to_uppercase(self):
        name = NameParser(self.lower_snake_input)
        self.assertEqual(self.uppercase, name.uppercase)

    def test_only_upper_to_uppercase(self):
        name = NameParser(self.only_upper_input)
        self.assertEqual(self.single_uppercase, name.uppercase)

    def test_only_upper_snake_to_uppercase(self):
        name = NameParser(self.only_upper_snake_input)
        self.assertEqual(self.uppercase, name.uppercase)

    def test_upper_camel_to_upper_camel_case(self):
        name = NameParser(self.upper_camel_input)
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_lower_camel_to_upper_camel_case(self):
        name = NameParser(self.lower_camel_input)
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_upper_snake_to_upper_camel_case(self):
        name = NameParser(self.upper_snake_input)
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_lower_snake_to_upper_camel_case(self):
        name = NameParser(self.lower_snake_input)
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_only_upper_to_upper_camel_case(self):
        name = NameParser(self.only_upper_input)
        self.assertEqual(self.single_upper_case, name.upper_camel_case)

    def test_only_upper_snake_to_upper_camel_case(self):
        name = NameParser(self.only_upper_snake_input)
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_upper_camel_to_lower_camel_case(self):
        name = NameParser(self.upper_camel_input)
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_lower_camel_to_lower_camel_case(self):
        name = NameParser(self.lower_camel_input)
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_upper_snake_to_lower_camel_case(self):
        name = NameParser(self.upper_snake_input)
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_lower_snake_to_lower_camel_case(self):
        name = NameParser(self.lower_snake_input)
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_only_upper_to_lower_camel_case(self):
        name = NameParser(self.only_upper_input)
        self.assertEqual(self.single_lower_case, name.lower_camel_case)

    def test_only_upper_snake_to_lower_camel_case(self):
        name = NameParser(self.only_upper_snake_input)
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_upper_camel_to_upper_snake_case(self):
        name = NameParser(self.upper_camel_input)
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_lower_camel_to_upper_snake_case(self):
        name = NameParser(self.lower_camel_input)
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_upper_snake_to_upper_snake_case(self):
        name = NameParser(self.upper_snake_input)
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_lower_snake_to_upper_snake_case(self):
        name = NameParser(self.lower_snake_input)
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_only_upper_to_upper_snake_case(self):
        name = NameParser(self.only_upper_input)
        self.assertEqual(self.single_upper_case, name.upper_snake_case)

    def test_only_upper_snake_to_upper_snake_case(self):
        name = NameParser(self.only_upper_snake_input)
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_upper_camel_to_lower_snake_case(self):
        name = NameParser(self.upper_camel_input)
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_lower_camel_to_lower_snake_case(self):
        name = NameParser(self.lower_camel_input)
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_upper_snake_to_lower_snake_case(self):
        name = NameParser(self.upper_snake_input)
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_lower_snake_to_lower_snake_case(self):
        name = NameParser(self.lower_snake_input)
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_only_upper_to_lower_snake_case(self):
        name = NameParser(self.only_upper_input)
        self.assertEqual(self.single_lower_case, name.lower_snake_case)

    def test_only_upper_snake_to_lower_snake_case(self):
        name = NameParser(self.only_upper_snake_input)
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)


if __name__ == '__main__':
    unittest.main()
