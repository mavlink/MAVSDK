#include "geometry.h"
#include <gtest/gtest.h>

using namespace mavsdk::geometry;

// Note the tests are quite coarse due to lack of better ground
// truth. This is mostly to get the ballpark and signs right.

TEST(Geometry, GlobalToLocal)
{
    CoordinateTransformation ct({47.356042, 8.519031});

    auto a_bit_south = ct.local_from_global({47.353697, 8.519124});
    EXPECT_NEAR(a_bit_south.north_m, -263.0, 5.0);
    EXPECT_NEAR(a_bit_south.east_m, 5.0, 5.0);

    auto further_west = ct.local_from_global({47.354218, 8.536610});
    EXPECT_NEAR(further_west.north_m, -207.0, 5.0);
    EXPECT_NEAR(further_west.east_m, 1320.0, 5.0);
}

TEST(Geometry, LocalToGlobal)
{
    CoordinateTransformation ct({32.812984, -117.251033});
    auto south_west = ct.global_from_local({-778.0, -1370.0});

    EXPECT_NEAR(south_west.latitude_deg, 32.805920, 0.0005);
    EXPECT_NEAR(south_west.longitude_deg, -117.265296, 0.0005);
}

TEST(Geometry, GlobalToLocalAndBack)
{
    CoordinateTransformation::GlobalCoordinate spot{-26.668028, 153.108583};
    CoordinateTransformation ct({-26.693518, 153.104172});
    auto spot_again = ct.global_from_local(ct.local_from_global(spot));

    EXPECT_NEAR(spot.latitude_deg, spot_again.latitude_deg, 1e-9);
    EXPECT_NEAR(spot.longitude_deg, spot_again.longitude_deg, 1e-9);
}

TEST(Geoemtry, LocalToGlobalAndBack)
{
    CoordinateTransformation::GlobalCoordinate spot{-38.227562, 176.506076};
    CoordinateTransformation ct(spot);
    CoordinateTransformation::LocalCoordinate location{-140.0, 240.0};
    auto location_again = ct.local_from_global(ct.global_from_local(location));

    EXPECT_NEAR(location.north_m, location_again.north_m, 1e-8);
    EXPECT_NEAR(location.east_m, location_again.east_m, 1e-8);
}
