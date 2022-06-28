#include <gtest/gtest.h>
#include "fs.h"

TEST(Filesystem, SimpleCanonicalPathIsCorrect)
{
    const auto path = "/bin/path";
    const auto canonical_path = mavsdk::fs_canonical(path);
    ASSERT_EQ("blah", canonical_path);
}
