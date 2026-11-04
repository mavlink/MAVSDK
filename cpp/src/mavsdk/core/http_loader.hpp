#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <functional>
#include "locked_queue.hpp"
#include "curl_wrapper.hpp"
#include "mavsdk_export.h"

namespace mavsdk {

class ICurlWrapper;

using TextDownloadCallback = std::function<void(bool success, const std::string& content)>;

class MAVSDK_TEST_EXPORT HttpLoader {
public:
    HttpLoader(std::unique_ptr<ICurlWrapper> curl_wrapper);
    HttpLoader();
    ~HttpLoader();

    void start();
    void stop();

    bool download_sync(
        const std::string& url,
        const std::string& local_path,
        curl_off_t max_download_size_bytes = 0);
    bool download_text_sync(const std::string& url, std::string& content);
    void download_async(
        const std::string& url,
        const std::string& local_path,
        ProgressCallback progress_callback = nullptr,
        curl_off_t max_download_size_bytes = 0);
    void download_text_async(const std::string& url, TextDownloadCallback text_callback);

    // Non-copyable
    HttpLoader(const HttpLoader&) = delete;
    const HttpLoader& operator=(const HttpLoader&) = delete;

private:
    class DownloadTextItem {
    public:
        DownloadTextItem(std::string url, TextDownloadCallback text_callback) :
            _url(std::move(url)),
            _text_callback(std::move(text_callback))
        {}

        [[nodiscard]] std::string get_url() const { return _url; }
        [[nodiscard]] TextDownloadCallback get_text_callback() const { return _text_callback; }

        DownloadTextItem(const DownloadTextItem&) = delete;
        DownloadTextItem& operator=(const DownloadTextItem&) = delete;
        DownloadTextItem(DownloadTextItem&&) = default;
        DownloadTextItem& operator=(DownloadTextItem&&) = default;

    private:
        std::string _url;
        TextDownloadCallback _text_callback;
    };

    class DownloadItem {
    public:
        DownloadItem(
            std::string url,
            std::string local_path,
            ProgressCallback progress_callback,
            curl_off_t max_download_size_bytes) :
            _url(std::move(url)),
            _local_path(std::move(local_path)),
            _progress_callback(std::move(progress_callback)),
            _max_download_size_bytes(max_download_size_bytes)
        {}

        [[nodiscard]] std::string get_local_path() const { return _local_path; }

        [[nodiscard]] std::string get_url() const { return _url; }

        [[nodiscard]] ProgressCallback get_progress_callback() const { return _progress_callback; }

        [[nodiscard]] curl_off_t get_max_download_size_bytes() const
        {
            return _max_download_size_bytes;
        }

    private:
        std::string _url;
        std::string _local_path;
        ProgressCallback _progress_callback{};
        curl_off_t _max_download_size_bytes{0};
    };

    using WorkItem = std::variant<DownloadItem, DownloadTextItem>;

    void work_thread();
    bool do_item(WorkItem& item);
    bool do_download(const DownloadItem& item);
    bool do_download_text(const DownloadTextItem& item);

    std::unique_ptr<ICurlWrapper> _curl_wrapper;

    LockedQueue<WorkItem> _work_queue{};
    std::thread _work_thread{};

    std::atomic<bool> _should_exit{false};
};

} // namespace mavsdk
