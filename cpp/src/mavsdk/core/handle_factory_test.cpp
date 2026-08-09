#include "handle_factory.hpp"
#include "handle.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(HandleFactory, DefaultHandleInvalid)
{
    Handle<> h;
    EXPECT_FALSE(h.valid());
}

TEST(HandleFactory, CreateYieldsUniqueValidIds)
{
    HandleFactory<> factory;

    auto a = factory.create();
    auto b = factory.create();
    auto c = factory.create();

    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(b.valid());
    EXPECT_TRUE(c.valid());

    EXPECT_FALSE(a == b);
    EXPECT_FALSE(b == c);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b < c);
}

TEST(HandleFactory, ConvertPreservesIdAcrossTypeArgs)
{
    HandleFactory<int> factory_int;
    auto h_int = factory_int.create();
    ASSERT_TRUE(h_int.valid());

    HandleFactory<> factory_void;
    auto h_void = factory_void.convert_from(h_int);
    EXPECT_TRUE(h_void.valid());
    EXPECT_TRUE(h_void == factory_void.convert_from(h_int));

    // convert_to should also preserve the same underlying identity
    auto h_dbl = factory_void.convert_to<double>(h_void);
    auto h_void2 = factory_void.convert_from(h_dbl);
    EXPECT_TRUE(h_void == h_void2);
}

TEST(HandleFactory, NullHandleComparesLessThanValid)
{
    HandleFactory<> factory;
    Handle<> null_h;
    auto valid = factory.create();
    EXPECT_TRUE(null_h < valid);
    EXPECT_FALSE(valid < null_h);
    EXPECT_FALSE(null_h == valid);
}
