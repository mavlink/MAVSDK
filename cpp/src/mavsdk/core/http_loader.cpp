#include "http_loader.hpp"
#include "curl_wrapper.hpp"
#include "overloaded.hpp"

namespace mavsdk {

HttpLoader::HttpLoader(std::unique_ptr<ICurlWrapper> curl_wrapper, HttpLoaderExecutor executor) :
    _curl_wrapper(std::move(curl_wrapper)),
    _executor(std::move(executor))
{
    start();
}

HttpLoader::HttpLoader(HttpLoaderExecutor executor) :
    HttpLoader(std::make_unique<CurlWrapper>(CurlWrapper()), std::move(executor))
{}

HttpLoader::~HttpLoader()
{
    // Once the work thread has stopped, nothing new is handed to the executor. Drop what is
    // still queued, and wait for a callback that may be running right now to return.
    stop();
    _alive->store(false);
    _executor([]() {}, true);
}

void HttpLoader::start()
{
    _should_exit = false;
    _work_thread = std::thread{&HttpLoader::work_thread, this};
}

void HttpLoader::stop()
{
    _should_exit = true;
    _work_queue.stop();
    if (_work_thread.joinable()) {
        _work_thread.join();
    }
}

bool HttpLoader::download_sync(
    const std::string& url, const std::string& local_path, curl_off_t max_download_size_bytes)
{
    WorkItem item = DownloadItem{url, local_path, nullptr, max_download_size_bytes};
    bool success = do_item(item);
    return success;
}

void HttpLoader::download_async(
    const std::string& url,
    const std::string& local_path,
    ProgressCallback progress_callback,
    curl_off_t max_download_size_bytes)
{
    ProgressCallback on_io_thread{};
    if (progress_callback) {
        on_io_thread = [this, alive = _alive, progress_callback = std::move(progress_callback)](
                           int progress, HttpStatus status, CURLcode curl_code) {
            _executor(
                [alive, progress_callback, progress, status, curl_code]() {
                    if (alive->load()) {
                        progress_callback(progress, status, curl_code);
                    }
                },
                false);
        };
    }

    _work_queue.push_back(
        std::make_shared<WorkItem>(
            DownloadItem{url, local_path, std::move(on_io_thread), max_download_size_bytes}));
}

void HttpLoader::download_text_async(const std::string& url, TextDownloadCallback text_callback)
{
    TextDownloadCallback on_io_thread{};
    if (text_callback) {
        on_io_thread = [this, alive = _alive, text_callback = std::move(text_callback)](
                           bool success, const std::string& content) {
            _executor(
                [alive, text_callback, success, content]() {
                    if (alive->load()) {
                        text_callback(success, content);
                    }
                },
                false);
        };
    }

    _work_queue.push_back(
        std::make_shared<WorkItem>(DownloadTextItem{url, std::move(on_io_thread)}));
}

void HttpLoader::work_thread()
{
    while (!_should_exit) {
        std::shared_ptr<WorkItem> work;
        {
            // Only hold the queue lock while popping. The download itself blocks, and its
            // callbacks may queue another download, which would deadlock against
            // push_back() if the queue lock were still held.
            LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
            work = work_queue_guard.wait_and_pop_front();
        }

        if (!work) {
            // Queue was stopped or should exit
            break;
        }

        do_item(*work);
    }
}

bool HttpLoader::do_item(WorkItem& item)
{
    return std::visit(
        overloaded{
            [&](DownloadItem& download_item) { return do_download(download_item); },
            [&](DownloadTextItem& download_text_item) {
                return do_download_text(download_text_item);
            },
        },
        item);
}

bool HttpLoader::do_download(const DownloadItem& item)
{
    bool success = _curl_wrapper->download_file_to_path(
        item.get_url(),
        item.get_local_path(),
        item.get_progress_callback(),
        item.get_max_download_size_bytes());
    return success;
}

bool HttpLoader::do_download_text(const DownloadTextItem& item)
{
    std::string content;
    bool success = _curl_wrapper->download_text(item.get_url(), content);

    if (item.get_text_callback()) {
        item.get_text_callback()(success, content);
    }

    return success;
}

bool HttpLoader::download_text_sync(const std::string& url, std::string& content)
{
    bool success = _curl_wrapper->download_text(url, content);
    return success;
}

} // namespace mavsdk
