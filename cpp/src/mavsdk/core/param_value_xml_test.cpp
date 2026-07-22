#include "param_value.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace mavsdk;

TEST(ParamValue, SetFromXmlCommonTypes)
{
    ParamValue v;
    ASSERT_TRUE(v.set_from_xml("float", "1.25"));
    ASSERT_TRUE(v.get_float().has_value());
    EXPECT_FLOAT_EQ(v.get_float().value(), 1.25f);
    EXPECT_EQ(v.typestr(), "float");

    ASSERT_TRUE(v.set_from_xml("int32", "42"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), 42);
    EXPECT_EQ(v.typestr(), "int32_t");

    ASSERT_TRUE(v.set_from_xml("int8", "-5"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), -5);

    ASSERT_TRUE(v.set_from_xml("uint16", "65535"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), 65535);

    ASSERT_TRUE(v.set_empty_type_from_xml("float"));
    EXPECT_EQ(v.typestr(), "float");
    ASSERT_TRUE(v.get_float().has_value());
}

TEST(ParamValue, SetAsSameTypePreservesKind)
{
    ParamValue f;
    f.set(0.0f);
    ASSERT_TRUE(f.set_as_same_type("3.5"));
    ASSERT_TRUE(f.get_float().has_value());
    EXPECT_FLOAT_EQ(f.get_float().value(), 3.5f);

    ParamValue i;
    i.set(static_cast<int32_t>(1));
    ASSERT_TRUE(i.set_as_same_type("99"));
    ASSERT_TRUE(i.get_int().has_value());
    EXPECT_EQ(i.get_int().value(), 99);
}

TEST(ParamValue, GetIntFloatCustomOptional)
{
    ParamValue i;
    i.set(static_cast<int16_t>(-7));
    ASSERT_TRUE(i.get_int().has_value());
    EXPECT_EQ(i.get_int().value(), -7);
    EXPECT_FALSE(i.get_float().has_value());
    EXPECT_FALSE(i.get_custom().has_value());

    ParamValue f;
    f.set(2.0f);
    ASSERT_TRUE(f.get_float().has_value());
    EXPECT_FALSE(f.get_int().has_value());

    ParamValue s;
    s.set(std::string("tag"));
    ASSERT_TRUE(s.get_custom().has_value());
    EXPECT_EQ(s.get_custom().value(), "tag");
    EXPECT_FALSE(s.get_int().has_value());
}

TEST(ParamValue, UpdateValueTypesafe)
{
    ParamValue a;
    a.set(static_cast<int32_t>(1));
    ParamValue b;
    b.set(static_cast<int32_t>(9));
    a.update_value_typesafe(b);
    EXPECT_EQ(a.get_int().value(), 9);
}
