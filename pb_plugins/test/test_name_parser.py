import unittest

from protoc_gen_dcsdk.name_parser import (NameParser, NameParserFactory)
from unittest.mock import patch, mock_open

class TestNameParser(unittest.TestCase):

    _initialisms_data = """ ["HTTP", "HTTPS", "URL", "ID"] """

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
        name = NameParser(self.upper_camel_input, [])
        self.assertEqual(self.uppercase, name.uppercase)

    def test_lower_camel_to_uppercase(self):
        name = NameParser(self.lower_camel_input, [])
        self.assertEqual(self.uppercase, name.uppercase)

    def test_upper_snake_to_uppercase(self):
        name = NameParser(self.upper_snake_input, [])
        self.assertEqual(self.uppercase, name.uppercase)

    def test_lower_snake_to_uppercase(self):
        name = NameParser(self.lower_snake_input, [])
        self.assertEqual(self.uppercase, name.uppercase)

    def test_only_upper_to_uppercase(self):
        name = NameParser(self.only_upper_input, [])
        self.assertEqual(self.single_uppercase, name.uppercase)

    def test_only_upper_snake_to_uppercase(self):
        name = NameParser(self.only_upper_snake_input, [])
        self.assertEqual(self.uppercase, name.uppercase)

    def test_upper_camel_to_upper_camel_case(self):
        name = NameParser(self.upper_camel_input, [])
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_lower_camel_to_upper_camel_case(self):
        name = NameParser(self.lower_camel_input, [])
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_upper_snake_to_upper_camel_case(self):
        name = NameParser(self.upper_snake_input, [])
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_lower_snake_to_upper_camel_case(self):
        name = NameParser(self.lower_snake_input, [])
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_only_upper_to_upper_camel_case(self):
        name = NameParser(self.only_upper_input, [])
        self.assertEqual(self.single_upper_case, name.upper_camel_case)

    def test_only_upper_snake_to_upper_camel_case(self):
        name = NameParser(self.only_upper_snake_input, [])
        self.assertEqual(self.upper_camel_case, name.upper_camel_case)

    def test_upper_camel_to_lower_camel_case(self):
        name = NameParser(self.upper_camel_input, [])
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_lower_camel_to_lower_camel_case(self):
        name = NameParser(self.lower_camel_input, [])
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_upper_snake_to_lower_camel_case(self):
        name = NameParser(self.upper_snake_input, [])
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_lower_snake_to_lower_camel_case(self):
        name = NameParser(self.lower_snake_input, [])
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_only_upper_to_lower_camel_case(self):
        name = NameParser(self.only_upper_input, [])
        self.assertEqual(self.single_lower_case, name.lower_camel_case)

    def test_only_upper_snake_to_lower_camel_case(self):
        name = NameParser(self.only_upper_snake_input, [])
        self.assertEqual(self.lower_camel_case, name.lower_camel_case)

    def test_upper_camel_to_upper_snake_case(self):
        name = NameParser(self.upper_camel_input, [])
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_lower_camel_to_upper_snake_case(self):
        name = NameParser(self.lower_camel_input, [])
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_upper_snake_to_upper_snake_case(self):
        name = NameParser(self.upper_snake_input, [])
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_lower_snake_to_upper_snake_case(self):
        name = NameParser(self.lower_snake_input, [])
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_only_upper_to_upper_snake_case(self):
        name = NameParser(self.only_upper_input, [])
        self.assertEqual(self.single_upper_case, name.upper_snake_case)

    def test_only_upper_snake_to_upper_snake_case(self):
        name = NameParser(self.only_upper_snake_input, [])
        self.assertEqual(self.upper_snake_case, name.upper_snake_case)

    def test_upper_camel_to_lower_snake_case(self):
        name = NameParser(self.upper_camel_input, [])
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_lower_camel_to_lower_snake_case(self):
        name = NameParser(self.lower_camel_input, [])
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_upper_snake_to_lower_snake_case(self):
        name = NameParser(self.upper_snake_input, [])
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_lower_snake_to_lower_snake_case(self):
        name = NameParser(self.lower_snake_input, [])
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    def test_only_upper_to_lower_snake_case(self):
        name = NameParser(self.only_upper_input, [])
        self.assertEqual(self.single_lower_case, name.lower_snake_case)

    def test_only_upper_snake_to_lower_snake_case(self):
        name = NameParser(self.only_upper_snake_input, [])
        self.assertEqual(self.lower_snake_case, name.lower_snake_case)

    @patch("builtins.open", new_callable=mock_open)
    def test_no_initialisms_when_no_initialims_file(self, mock_file):
        mock_file.side_effect = FileNotFoundError()
        name_parser_factory = NameParserFactory()
        name = name_parser_factory.create("fileUrl")

        self.assertEqual("FileUrl", name.upper_camel_case)
        self.assertEqual("fileUrl", name.lower_camel_case)
        self.assertEqual("file_url", name.lower_snake_case)
        self.assertEqual("File_Url", name.upper_snake_case)

    @patch("builtins.open", new_callable=mock_open, read_data=_initialisms_data)
    def test_initalism_is_capitalized_when_camel_case(self, mock_file):
        name_parser_factory = NameParserFactory()
        name_parser_factory.set_template_path("random/path")
        name = name_parser_factory.create("fileUrl")

        self.assertEqual("FileURL", name.upper_camel_case)
        self.assertEqual("fileURL", name.lower_camel_case)

    @patch("builtins.open", new_callable=mock_open, read_data=_initialisms_data)
    def test_initalism_is_capitalized_when_upper_snake_case(self, mock_file):
        name_parser_factory = NameParserFactory()
        name_parser_factory.set_template_path("random/path")
        name = name_parser_factory.create("fileUrl")

        self.assertEqual("File_URL", name.upper_snake_case)

    @patch("builtins.open", new_callable=mock_open, read_data=_initialisms_data)
    def test_initalism_is_not_capitalized_when_lower_snake_case(self, mock_file):
        name_parser_factory = NameParserFactory()
        name_parser_factory.set_template_path("random/path")
        name = name_parser_factory.create("fileUrl")

        self.assertEqual("file_url", name.lower_snake_case)


if __name__ == '__main__':
    unittest.main()
