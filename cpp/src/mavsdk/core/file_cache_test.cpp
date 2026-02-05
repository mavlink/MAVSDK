
#include <gtest/gtest.h>
#include <fstream>
#include "file_cache.h"
#include "fs_utils.h"

using namespace mavsdk;

class FileCacheTest : public testing::Test {
protected:
    void SetUp() override
    {
        _cache_dir = get_cache_directory().value() / "mavsdk-test-file-cache";
        _tmp_files_dir = create_tmp_directory("mavsdk-test-file-cache").value();

        cleanup();

        std::filesystem::create_directories(_tmp_files_dir);

        for (int i = 0; i < 30; ++i) {
            TmpFile t;
            t.content = std::to_string(i);
            t.path = _tmp_files_dir / (t.content + ".txt");
            char buf[100];
            snprintf(buf, sizeof(buf), "_tag_%08i_xy", i);
            t.cache_tag = buf;
            std::ofstream f(t.path, std::ios::out | std::ios::binary);
            f.write(t.content.c_str(), t.content.length());
            f.close();
            _tmp_files.push_back(t);
        }
    }

    void TearDown() override { cleanup(); }

    void cleanup()
    {
        std::error_code ec;
        std::filesystem::remove_all(_tmp_files_dir, ec);
        std::filesystem::remove_all(_cache_dir, ec);
    }

    struct TmpFile {
        std::filesystem::path path;
        std::string cache_tag;
        std::string content;
        std::filesystem::path cached_path;
    };

    std::vector<TmpFile> _tmp_files;

    std::filesystem::path _cache_dir;
    std::filesystem::path _tmp_files_dir;
};

TEST_F(FileCacheTest, basic_test)
{
    FileCache cache(_cache_dir, 10, true);

    ASSERT_TRUE(std::filesystem::is_directory(_cache_dir));

    _tmp_files[0].cached_path = cache.insert(_tmp_files[0].cache_tag, _tmp_files[0].path).value();
    EXPECT_TRUE(std::filesystem::exists(_tmp_files[0].cached_path));
    EXPECT_FALSE(std::filesystem::exists(_tmp_files[0].path));

    EXPECT_EQ(cache.access(_tmp_files[0].cache_tag).value(), _tmp_files[0].cached_path);

    std::ifstream f(_tmp_files[0].cached_path);
    EXPECT_TRUE(f.good());
    std::stringstream buffer;
    buffer << f.rdbuf();
    EXPECT_EQ(buffer.str(), _tmp_files[0].content);
}

TEST_F(FileCacheTest, lru_test)
{
    FileCache cache(_cache_dir, 3, true);

    auto insert = [&](int idx) {
        auto cached_path_option = cache.insert(_tmp_files[idx].cache_tag, _tmp_files[idx].path);
        ASSERT_TRUE(cached_path_option);
        _tmp_files[idx].cached_path = cached_path_option.value();
    };
    insert(1);
    insert(3);
    insert(0);

    EXPECT_EQ(cache.access(_tmp_files[0].cache_tag).value(), _tmp_files[0].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);

    insert(4);

    EXPECT_FALSE(cache.access(_tmp_files[0].cache_tag));
    EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[4].cache_tag).value(), _tmp_files[4].cached_path);

    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);

    insert(5);

    EXPECT_FALSE(cache.access(_tmp_files[4].cache_tag));
    EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[5].cache_tag).value(), _tmp_files[5].cached_path);

    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
    insert(6);
    insert(7);

    EXPECT_FALSE(cache.access(_tmp_files[4].cache_tag));
    EXPECT_FALSE(cache.access(_tmp_files[1].cache_tag));
    EXPECT_FALSE(cache.access(_tmp_files[5].cache_tag));
    EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[6].cache_tag).value(), _tmp_files[6].cached_path);
    EXPECT_EQ(cache.access(_tmp_files[7].cache_tag).value(), _tmp_files[7].cached_path);
}

TEST_F(FileCacheTest, multi_test)
{
    auto insert = [&](FileCache& cache, int idx) {
        auto cached_path_option = cache.insert(_tmp_files[idx].cache_tag, _tmp_files[idx].path);
        ASSERT_TRUE(cached_path_option);
        _tmp_files[idx].cached_path = cached_path_option.value();
    };

    {
        FileCache cache(_cache_dir, 5, true);
        for (int i = 0; i < 5; ++i) {
            insert(cache, i);
            EXPECT_EQ(cache.access(_tmp_files[i].cache_tag).value(), _tmp_files[i].cached_path);
        }
        EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
    }
    {
        // reduce cache size and ensure oldest entries are evicted
        FileCache cache(_cache_dir, 3, true);
        EXPECT_FALSE(cache.access(_tmp_files[0].cache_tag));
        EXPECT_EQ(cache.access(_tmp_files[1].cache_tag).value(), _tmp_files[1].cached_path);
        EXPECT_FALSE(cache.access(_tmp_files[2].cache_tag));
        EXPECT_EQ(cache.access(_tmp_files[3].cache_tag).value(), _tmp_files[3].cached_path);
        EXPECT_EQ(cache.access(_tmp_files[4].cache_tag).value(), _tmp_files[4].cached_path);

        insert(cache, 10);
        EXPECT_FALSE(cache.access(_tmp_files[1].cache_tag));
        EXPECT_EQ(cache.access(_tmp_files[10].cache_tag).value(), _tmp_files[10].cached_path);
    }
}
