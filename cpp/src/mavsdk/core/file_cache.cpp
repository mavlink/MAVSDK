
#include "file_cache.h"
#include "log.h"

#include <cstring>
#include <fstream>

// For directory locking
#ifdef WINDOWS
#include <Windows.h>
#else
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace mavsdk {

FileCache::DirectoryLock::DirectoryLock(const std::filesystem::path& path) : _path(path)
{
#ifdef WINDOWS
    _lock =
        CreateFileA(_path.string().c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (!LockFileEx(_lock, LOCKFILE_EXCLUSIVE_LOCK, 0, _lock_size, 0, &_overlapped)) {
        LogErr() << "Cannot take lock file " << GetLastError();
    }
#else
    mode_t m = umask(0);
    _fd = open(_path.c_str(), O_RDWR | O_CREAT, 0666);
    umask(m);
    if (_fd < 0) {
        LogErr() << "Cannot open file " << _path;
    } else {
        if (flock(_fd, LOCK_EX) == -1) {
            LogErr() << "Cannot lock file " << strerror(errno);
        }
    }
#endif
}
FileCache::DirectoryLock::~DirectoryLock()
{
#ifdef WINDOWS
    if (_lock != INVALID_HANDLE_VALUE) {
        UnlockFileEx(_lock, 0, _lock_size, 0, &_overlapped);
        CloseHandle(_lock);
    }
#else
    if (_fd >= 0) {
        flock(_fd, LOCK_UN);
        close(_fd);
    }
#endif
}

FileCache::FileCache(std::filesystem::path path, int max_num_files, bool verbose_debugging) :
    _path(std::move(path)),
    _max_num_files(max_num_files),
    _verbose_debugging(verbose_debugging)
{
    if (!std::filesystem::exists(_path)) {
        std::error_code err;
        if (!std::filesystem::create_directories(_path, err)) {
            LogErr() << "Failed to create cache directory: " << err.message();
        }
    }
}

std::filesystem::path FileCache::meta_filename(const std::string& file_tag) const
{
    return _path / (file_tag + _meta_extension);
}

std::filesystem::path FileCache::data_filename(const std::string& file_tag) const
{
    return _path / (file_tag + _cache_extension);
}

std::optional<std::filesystem::path> FileCache::access(const std::string& file_tag)
{
    const DirectoryLock directory_lock(_path / _lock_file); // also serves as multi-threadding lock

    const std::filesystem::path meta(meta_filename(file_tag));
    std::filesystem::path data(data_filename(file_tag));
    if (!std::filesystem::exists(meta) || !std::filesystem::exists(data)) {
        if (_verbose_debugging) {
            LogDebug() << "Cache miss for " << file_tag;
        }
        return std::nullopt;
    }

    const auto access_counters = load_access_counters();
    remove_old_entries(access_counters); // Only needed if the cache size got reduced
    // Check if entry is still valid (it could have been removed if the metadata is invalid)
    if (!std::filesystem::exists(data)) {
        if (_verbose_debugging) {
            LogDebug() << "Cache miss for " << file_tag;
        }
        return std::nullopt;
    }

    if (_verbose_debugging) {
        LogDebug() << "Cache hit for " << file_tag;
    }

    // Mark access
    Meta m{};
    std::fstream meta_file(meta, std::ios::binary | std::ios::in | std::ios::out);
    if (meta_file.good()) {
        if (meta_file.read(reinterpret_cast<char*>(&m), sizeof(m)) &&
            meta_file.gcount() == sizeof(m)) {
            m.access_counter = access_counters.next_access_counter;
            meta_file.seekg(0);
            if (!meta_file.write(reinterpret_cast<const char*>(&m), sizeof(m))) {
                LogErr() << "Meta write failed " << meta;
            }
        } else {
            LogErr() << "Meta read failed " << meta;
        }
    } else {
        LogErr() << "Failed to open " << meta;
    }

    return data;
}

std::optional<std::filesystem::path>
FileCache::insert(const std::string& file_tag, const std::filesystem::path& file_name)
{
    const DirectoryLock directory_lock(_path / _lock_file); // also serves as multi-threadding lock

    const std::filesystem::path meta(meta_filename(file_tag));
    std::filesystem::path data(data_filename(file_tag));
    auto access_counters = load_access_counters();

    if (std::filesystem::exists(meta) && std::filesystem::exists(data)) {
        if (_verbose_debugging) {
            LogDebug() << "Not inserting, entry already exists: " << file_tag;
        }
        std::filesystem::remove(file_name);
        return data;
    }

    // Move the file to the cache location
    std::error_code err;
    std::filesystem::rename(file_name, data, err);
    if (err) {
        if (err.value() == EXDEV) { // different file system
            std::filesystem::copy_file(file_name, data, err);
            if (!err) {
                std::filesystem::remove(file_name, err);
            }
        }
        if (err) {
            LogWarn() << "File rename failed from " << file_name << " to " << data << ": "
                      << err.message();
            return std::nullopt;
        }
    }

    // Write meta data
    Meta m{};
    m.access_counter = access_counters.next_access_counter++;
    std::fstream meta_file(meta, std::ios::binary | std::ios::out | std::ios::trunc);
    if (meta_file.good()) {
        if (!meta_file.write(reinterpret_cast<const char*>(&m), sizeof(m))) {
            LogWarn() << "Meta write failed " << meta;
        }
        access_counters.cached_files[m.access_counter] = file_tag;
    } else {
        LogWarn() << "Failed to open " << meta;
    }

    remove_old_entries(access_counters);
    return data;
}

FileCache::AccessCounters FileCache::load_access_counters() const
{
    AccessCounters access_counters;

    for (const auto& path : std::filesystem::directory_iterator(_path)) {
        if (path.path().extension() != _meta_extension) {
            continue;
        }

        bool validation_failed = false;
        std::filesystem::path data(path);
        data.replace_extension(_cache_extension);
        if (!std::filesystem::exists(data)) {
            validation_failed = true;
        }

        // Read meta + validate
        Meta m{};
        const uint32_t expected_magic = m.magic;
        const uint32_t expected_version = m.version;
        std::ifstream meta_file(path.path(), std::ios::binary);
        if (meta_file.read(reinterpret_cast<char*>(&m), sizeof(m)) &&
            meta_file.gcount() == sizeof(m)) {
            if (m.magic != expected_magic || m.version != expected_version) {
                validation_failed = true;
            }
        } else {
            validation_failed = true;
        }

        if (validation_failed) {
            LogWarn() << "Validation failed, removing cache files " << path.path();
            std::filesystem::remove(path.path());
            std::filesystem::remove(data);
        } else {
            // Extract the tag
            const std::string tag = path.path().stem().string();
            access_counters.cached_files[m.access_counter] = tag;

            // LogDebug() << "Found cached file: " << path.path() << " counter: " <<
            // m.access_counter;

            if (m.access_counter >= access_counters.next_access_counter) {
                access_counters.next_access_counter = m.access_counter + 1;
            }
        }
    }
    return access_counters;
}

void FileCache::remove_old_entries(const AccessCounters& access_counters) const
{
    int num_delete = static_cast<int>(access_counters.cached_files.size()) - _max_num_files;
    auto iter = access_counters.cached_files.begin();
    while (num_delete > 0) {
        if (_verbose_debugging) {
            LogDebug() << "Removing cache entry num:counter:file" << num_delete << iter->first
                       << iter->second;
        }
        std::filesystem::remove(meta_filename(iter->second));
        std::filesystem::remove(data_filename(iter->second));
        --num_delete;
        ++iter;
    }
}

} // namespace mavsdk
