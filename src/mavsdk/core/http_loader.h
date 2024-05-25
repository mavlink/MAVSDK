#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include "safe_queue.h"
#include "curl_wrapper.h"

namespace mavsdk {

class ICurlWrapper;

class HttpLoader {
public:
#ifdef TESTING
    HttpLoader(const std::shared_ptr<ICurlWrapper>& curl_wrapper);
#endif

    explicit HttpLoader();
    ~HttpLoader();

    void start();
    void stop();

    bool download_sync(const std::string& url, const std::string& local_path);
    bool download_text_sync(const std::string& url, std::string& content);
    void download_async(
        const std::string& url,
        const std::string& local_path,
        const ProgressCallback& progress_callback = nullptr);

    bool upload_sync(const std::string& target_url, const std::string& local_path);
    void upload_async(
        const std::string& target_url,
        const std::string& local_path,
        const ProgressCallback& progress_callback = nullptr);

    // Non-copyable
    HttpLoader(const HttpLoader&) = delete;
    const HttpLoader& operator=(const HttpLoader&) = delete;

private:
    class WorkItem {
    public:
        WorkItem() = default;
        virtual ~WorkItem() = default;

        WorkItem(WorkItem&) = delete;
        WorkItem operator=(WorkItem&) = delete;
    };

    class DownloadTextItem : public WorkItem {
    public:
        explicit DownloadTextItem(std::string url) : _url(std::move(url)) {}

        [[nodiscard]] std::string get_url() const { return _url; }

        DownloadTextItem(DownloadTextItem&) = delete;
        DownloadTextItem operator=(DownloadTextItem&) = delete;

    private:
        std::string _url;
    };

    class DownloadItem : public WorkItem {
    public:
        DownloadItem(std::string url, std::string local_path, ProgressCallback progress_callback) :
            _url(std::move(url)),
            _local_path(std::move(local_path)),
            _progress_callback(std::move(progress_callback))
        {}

        virtual ~DownloadItem() = default;

        [[nodiscard]] std::string get_local_path() const { return _local_path; }

        [[nodiscard]] std::string get_url() const { return _url; }

        [[nodiscard]] ProgressCallback get_progress_callback() const { return _progress_callback; }

        DownloadItem(DownloadItem&) = delete;
        DownloadItem operator=(DownloadItem&) = delete;

    private:
        std::string _url;
        std::string _local_path;
        ProgressCallback _progress_callback{};
    };

    static void work_thread(HttpLoader* self);
    static void do_item(
        const std::shared_ptr<WorkItem>& item, const std::shared_ptr<ICurlWrapper>& curl_wrapper);
    static bool do_download(
        const std::shared_ptr<DownloadItem>& item,
        const std::shared_ptr<ICurlWrapper>& curl_wrapper);

    std::shared_ptr<ICurlWrapper> _curl_wrapper;

    SafeQueue<std::shared_ptr<WorkItem>> _work_queue{};
    std::thread* _work_thread = nullptr;

    std::atomic<bool> _should_exit{false};
};

} // namespace mavsdk
