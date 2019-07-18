import unittest

from collections import namedtuple
from protoc_gen_dcsdk.type_info import (TypeInfo, TypeInfoFactory)
from unittest.mock import patch, mock_open

class TestTypeInfo(unittest.TestCase):

    _conversion_dict_data = """ {
        "bool": "converted_bool"
    } """

    _conversion_dict_data_repeatable = """ {
        "repeated": { "prefix": "prefix[", "suffix": "]suffix" }
    } """

    def setUp(self):
        self.primitive_field = namedtuple("primitive_field", "type label")
        self.non_primitive_field = namedtuple("non_primitive_field", "type type_name label")

        self.bool_id = 8
        self.bool_expected_default_str = "bool"
        self.bool_expected_converted_str = "converted_bool"

        self.double_id = 1
        self.double_expected_default_str = "double"

        self.invalid_id = 42

        self.non_repeated_label = 1
        self.repeated_label = 3

    @patch("builtins.open", new_callable=mock_open)
    def test_name_uses_default_when_no_conversion_file(self, mock_file):
        mock_file.side_effect = FileNotFoundError()
        type_info_factory = TypeInfoFactory()

        type_info = type_info_factory.create(self.primitive_field(self.bool_id, self.non_repeated_label))

        self.assertEqual(type_info.name, self.bool_expected_default_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_uses_default_when_no_conversion(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.primitive_field(self.double_id, self.non_repeated_label))

        self.assertEqual(type_info.name, self.double_expected_default_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_converts_type_when_conversion(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.primitive_field(self.bool_id, self.non_repeated_label))

        self.assertEqual(type_info.name, self.bool_expected_converted_str)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_raises_error_when_invalid_type(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.primitive_field(self.invalid_id, self.non_repeated_label))

        with self.assertRaises(ValueError):
            type_info.name

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_primitive_false_for_complex_types(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")

        for type_id in {11, 14}:
            non_primitive_type = type_info_factory.create(self.primitive_field(type_id, self.non_repeated_label))
            self.assertFalse(non_primitive_type.is_primitive)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_primitive_true_for_primitive_types(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")

        for type_id in {1, 2, 3, 4, 5, 8, 9, 12, 13}:
            primitive_type = type_info_factory.create(self.primitive_field(type_id, self.non_repeated_label))
            self.assertTrue(primitive_type.is_primitive)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_false_for_primitive_types(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")

        for type_id in {1, 2, 3, 4, 5, 8, 9, 12, 13}:
            primitive_type = type_info_factory.create(self.primitive_field(type_id, self.non_repeated_label))
            self.assertFalse(primitive_type.is_result)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_false_for_complex_non_result_types(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")

        for type_id in {11, 14}:
            complex_type = type_info_factory.create(self.non_primitive_field(type_id, "SomeNonResultType", self.non_repeated_label))
            self.assertFalse(complex_type.is_result)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_is_result_true_for_result_types(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")

        for type_id in {11, 14}:
            complex_type = type_info_factory.create(self.non_primitive_field(type_id, "SomeResult", self.non_repeated_label))
            self.assertTrue(complex_type.is_result)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_repeated_false_for_non_repeated_primitive(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        non_repeated_primitive_type = type_info_factory.create(self.primitive_field(self.double_id, self.non_repeated_label))

        self.assertFalse(non_repeated_primitive_type.is_repeated)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_repeated_true_for_repeated_primitive(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        repeated_primitive_type = type_info_factory.create(self.primitive_field(self.double_id, self.repeated_label))

        self.assertTrue(repeated_primitive_type.is_repeated)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_repeated_false_for_non_repeated(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        non_repeated_type = type_info_factory.create(self.non_primitive_field(11, "SomeNonResultType", self.non_repeated_label))

        self.assertFalse(non_repeated_type.is_repeated)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_repeated_true_for_repeated(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        repeated_type = type_info_factory.create(self.non_primitive_field(11, "SomeNonResultType", self.repeated_label))

        self.assertTrue(repeated_type.is_repeated)

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_name_adds_prefix_suffix_for_repeated_primitive(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.primitive_field(self.double_id, self.repeated_label))

        self.assertEqual(type_info.name, "prefix[double]suffix")

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_adds_default_prefix_suffix_when_repeated_primitive_not_in_conversion_dict(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.primitive_field(self.double_id, self.repeated_label))

        self.assertEqual(type_info.name, "std::vector<double>")

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_name_adds_prefix_suffix_for_repeated(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.non_primitive_field(11, "SomeNonResultType", self.repeated_label))

        self.assertEqual(type_info.name, "prefix[SomeNonResultType]suffix")

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data)
    def test_name_adds_default_prefix_suffix_when_repeated_not_in_conversion_dict(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.non_primitive_field(11, "SomeNonResultType", self.repeated_label))

        self.assertEqual(type_info.name, "std::vector<SomeNonResultType>")

    @patch("builtins.open", new_callable=mock_open, read_data=_conversion_dict_data_repeatable)
    def test_inner_name_for_repeated(self, mock_file):
        type_info_factory = TypeInfoFactory()
        type_info_factory.set_template_path("random/path")
        type_info = type_info_factory.create(self.non_primitive_field(11, "SomeNonResultType", self.repeated_label))

        self.assertEqual(type_info.inner_name, "SomeNonResultType")


if __name__ == '__main__':
    unittest.main()
