#pragma once

#include <filesystem>
#include <cstdint>
#include <optional>
#include <string>
#include <map>

#ifdef WINDOWS
#include <Windows.h>
#endif

namespace mavsdk {

/**
 * Simple file cache with a maximum number of files and LRU retention policy based on last access
 * Notes:
 * - fileTag defines the cache keys and the format is up to the user
 * - thread-safe
 * - multiple instances (or processes) can work on the same directory.
 *   (however there is no lock held when returning a file from a cache, so the assumption is that
 *   the cache is large enough so no other process evicts the file until it is used).
 */
class FileCache {
public:
    FileCache(std::filesystem::path path, int max_num_files, bool verbose_debugging = false);

    /**
     * Try to access a file and set the access counter
     * @param fileTag
     * @return file path or nullopt if not found
     */
    std::optional<std::filesystem::path> access(const std::string& file_tag);

    /**
     * Insert a file into the cache & remove old files if there's too many.
     * @param fileTag
     * @param fileName file to insert, will be moved (or deleted if already exists)
     * @return cached file name if inserted or already exists, nullopt on error
     */
    std::optional<std::filesystem::path>
    insert(const std::string& file_tag, const std::filesystem::path& file_name);

private:
    class DirectoryLock {
    public:
        explicit DirectoryLock(const std::filesystem::path& path);
        ~DirectoryLock();

    private:
#ifdef WINDOWS
        OVERLAPPED _overlapped{};
        const int _lock_size = 10000;
        HANDLE _lock{};
#else
        int _fd{-1};
#endif
        const std::filesystem::path _path;
    };

    static constexpr const char* _meta_extension = ".meta";
    static constexpr const char* _cache_extension = ".cache";
    static constexpr const char* _lock_file = "lock";

    using AccessCounterType = uint64_t;

    struct Meta {
        uint32_t magic{0x9a9cad0e};
        uint32_t version{0};
        AccessCounterType access_counter{0};
    };

    struct AccessCounters {
        AccessCounterType next_access_counter{0};
        std::map<AccessCounterType, std::string> cached_files;
    };

    AccessCounters load_access_counters() const;
    void remove_old_entries(const AccessCounters& access_counters) const;

    std::filesystem::path meta_filename(const std::string& file_tag) const;
    std::filesystem::path data_filename(const std::string& file_tag) const;

    const std::filesystem::path _path;
    const int _max_num_files;
    const bool _verbose_debugging;
};

} // namespace mavsdk
