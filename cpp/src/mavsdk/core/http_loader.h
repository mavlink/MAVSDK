#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <functional>
#include "locked_queue.h"
#include "curl_wrapper.h"

namespace mavsdk {

class ICurlWrapper;

using TextDownloadCallback = std::function<void(bool success, const std::string& content)>;

class HttpLoader {
public:
#ifdef TESTING
    HttpLoader(std::unique_ptr<ICurlWrapper> curl_wrapper) : _curl_wrapper(std::move(curl_wrapper))
    {
        start();
    }
#endif

    HttpLoader();
    ~HttpLoader();

    void start();
    void stop();

    bool download_sync(const std::string& url, const std::string& local_path);
    bool download_text_sync(const std::string& url, std::string& content);
    void download_async(
        const std::string& url,
        const std::string& local_path,
        ProgressCallback progress_callback = nullptr);
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
        DownloadItem(std::string url, std::string local_path, ProgressCallback progress_callback) :
            _url(std::move(url)),
            _local_path(std::move(local_path)),
            _progress_callback(std::move(progress_callback))
        {}

        [[nodiscard]] std::string get_local_path() const { return _local_path; }

        [[nodiscard]] std::string get_url() const { return _url; }

        [[nodiscard]] ProgressCallback get_progress_callback() const { return _progress_callback; }

    private:
        std::string _url;
        std::string _local_path;
        ProgressCallback _progress_callback{};
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
