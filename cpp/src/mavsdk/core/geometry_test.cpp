#include "geometry.hpp"
#include <gtest/gtest.h>
#include <cmath>

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

TEST(Geometry, LocalToGlobalAndBack)
{
    CoordinateTransformation::GlobalCoordinate spot{-38.227562, 176.506076};
    CoordinateTransformation ct(spot);
    CoordinateTransformation::LocalCoordinate location{-140.0, 240.0};
    auto location_again = ct.local_from_global(ct.global_from_local(location));

    EXPECT_NEAR(location.north_m, location_again.north_m, 1e-8);
    EXPECT_NEAR(location.east_m, location_again.east_m, 1e-8);
}

TEST(Geometry, IdentityAtReference)
{
    CoordinateTransformation::GlobalCoordinate ref{47.397742, 8.545594};
    CoordinateTransformation ct(ref);
    auto local = ct.local_from_global(ref);
    EXPECT_NEAR(local.north_m, 0.0, 1e-9);
    EXPECT_NEAR(local.east_m, 0.0, 1e-9);

    auto back = ct.global_from_local({0.0, 0.0});
    EXPECT_NEAR(back.latitude_deg, ref.latitude_deg, 1e-12);
    EXPECT_NEAR(back.longitude_deg, ref.longitude_deg, 1e-12);
}

TEST(Geometry, PureNorthOffset)
{
    CoordinateTransformation ct({0.0, 0.0});
    // ~111.32 km per degree latitude near equator with R=6371000
    auto global = ct.global_from_local({111194.92664455874, 0.0});
    EXPECT_NEAR(global.latitude_deg, 1.0, 1e-6);
    EXPECT_NEAR(global.longitude_deg, 0.0, 1e-9);

    auto local = ct.local_from_global({1.0, 0.0});
    EXPECT_NEAR(local.north_m, 111194.92664455874, 1.0);
    EXPECT_NEAR(local.east_m, 0.0, 1e-6);
}

TEST(Geometry, PureEastOffsetAtEquator)
{
    CoordinateTransformation ct({0.0, 0.0});
    // longitude degrees ≈ east_m / (R * cos(lat)); at equator cos=1
    auto global = ct.global_from_local({0.0, 111194.92664455874});
    EXPECT_NEAR(global.latitude_deg, 0.0, 1e-6);
    EXPECT_NEAR(global.longitude_deg, 1.0, 1e-6);

    auto local = ct.local_from_global({0.0, 1.0});
    EXPECT_NEAR(local.north_m, 0.0, 1.0);
    EXPECT_NEAR(local.east_m, 111194.92664455874, 1.0);
}

TEST(Geometry, SmallSouthEastRoundtrip)
{
    CoordinateTransformation ct({47.397742, 8.545594});
    CoordinateTransformation::LocalCoordinate loc{-25.5, 40.25};
    auto again = ct.local_from_global(ct.global_from_local(loc));
    EXPECT_NEAR(loc.north_m, again.north_m, 1e-6);
    EXPECT_NEAR(loc.east_m, again.east_m, 1e-6);
}

TEST(Geometry, EastScaleAtMidLatitude)
{
    // WGS84 ENU: same east meters span more longitude delusions at higher |lat|.
    // Compare equator vs 60° for identical local east offset.
    constexpr double east_m = 1000.0;
    CoordinateTransformation eq({0.0, 0.0});
    CoordinateTransformation mid({60.0, 0.0});

    auto g_eq = eq.global_from_local({0.0, east_m});
    auto g_mid = mid.global_from_local({0.0, east_m});

    // Pure-ish east: latitude should stay near reference (small curvature terms ok).
    EXPECT_NEAR(g_eq.latitude_deg, 0.0, 1e-4);
    EXPECT_NEAR(g_mid.latitude_deg, 60.0, 1e-4);

    // Higher latitude → larger longitude change for same east meters.
    EXPECT_GT(std::abs(g_mid.longitude_deg), std::abs(g_eq.longitude_deg));
    // Rough spherical factor ~1/cos(60°)=2; allow generous float/sphere model slack.
    EXPECT_NEAR(std::abs(g_mid.longitude_deg) / std::abs(g_eq.longitude_deg), 2.0, 0.15);

    // Roundtrip at mid latitude.
    auto again = mid.local_from_global(g_mid);
    EXPECT_NEAR(again.north_m, 0.0, 1.0);
    EXPECT_NEAR(again.east_m, east_m, 1.0);
}

TEST(Geometry, WestOffsetRoundtrip)
{
    CoordinateTransformation ct({47.397742, 8.545594});
    CoordinateTransformation::LocalCoordinate west{10.0, -125.5};
    auto again = ct.local_from_global(ct.global_from_local(west));
    EXPECT_NEAR(west.north_m, again.north_m, 1e-6);
    EXPECT_NEAR(west.east_m, again.east_m, 1e-6);
}

TEST(Geometry, PureSouthOffset)
{
    CoordinateTransformation ct({0.0, 0.0});
    constexpr double meters_per_deg_lat = 111194.92664455874;
    auto global = ct.global_from_local({-meters_per_deg_lat, 0.0});
    EXPECT_NEAR(global.latitude_deg, -1.0, 1e-6);
    EXPECT_NEAR(global.longitude_deg, 0.0, 1e-9);
}
