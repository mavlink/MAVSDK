import unittest

from collections import namedtuple
from dcsdkgen.type_info import (TypeInfo, TypeInfoFactory)
from unittest.mock import patch, mock_open

class TestTypeInfo(unittest.TestCase):

    _conversion_dict_data = """ {
        "bool": "converted_bool"
    } """

    def setUp(self):
        self.primitive_field = namedtuple("primitive_field", "type")
        self.non_primitive_field = namedtuple("non_primitive_field", "type type_name")

        self.bool_id = 8
        self.bool_expected_default_str = "bool"
        self.bool_expected_converted_str = "converted_bool"

        self.double_id = 1
        self.double_expected_default_str = "double"

        self.invalid_id = 42

    @patch("builtins.open", new_callable=mock_open)
    def test_name_uses_default_when_no_conversion_file(self, mock_file):
        mock_file.side_effect = FileNotFoundError()
        type_info_factory = TypeInfoFactory()

        type_info = type_info_factory.create(self.primitive_field(self.bool_id))

        self.assertEqual(type_info.name, self.bool_expected_default_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_uses_default_when_no_conversion(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info = type_info_factory.create(self.primitive_field(self.double_id))

        self.assertEqual(type_info.name, self.double_expected_default_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_converts_type_when_conversion(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info = type_info_factory.create(self.primitive_field(self.bool_id))

        self.assertEqual(type_info.name, self.bool_expected_converted_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_raises_error_when_invalid_type(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info = type_info_factory.create(self.primitive_field(self.invalid_id))

        self.assertRaises(ValueError)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_primitive_false_for_complex_types(self, mock_file):
        type_info_factory = TypeInfoFactory()

        for type_id in {11, 14}:
            non_primitive_type = type_info_factory.create(self.primitive_field(type_id))
            self.assertFalse(non_primitive_type.is_primitive)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_primitive_true_for_primitive_types(self, mock_file):
        type_info_factory = TypeInfoFactory()

        for type_id in {1, 2, 3, 4, 5, 8, 9, 12, 13}:
            primitive_type = type_info_factory.create(self.primitive_field(type_id))
            self.assertTrue(primitive_type.is_primitive)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_false_for_primitive_types(self, mock_file):
        type_info_factory = TypeInfoFactory()

        for type_id in {1, 2, 3, 4, 5, 8, 9, 12, 13}:
            primitive_type = type_info_factory.create(self.primitive_field(type_id))
            self.assertFalse(primitive_type.is_result)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_false_for_complex_non_result_types(self, mock_file):
        type_info_factory = TypeInfoFactory()

        for type_id in {11, 14}:
            complex_type = type_info_factory.create(self.non_primitive_field(type_id, "SomeNonResultType"))
            self.assertFalse(complex_type.is_result)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_true_for_result_types(self, mock_file):
        type_info_factory = TypeInfoFactory()

        for type_id in {11, 14}:
            complex_type = type_info_factory.create(self.non_primitive_field(type_id, "SomeResult"))
            self.assertTrue(complex_type.is_result)

if __name__ == '__main__':
    unittest.main()
