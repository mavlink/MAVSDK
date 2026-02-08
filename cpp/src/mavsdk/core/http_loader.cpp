#include "http_loader.h"
#include "curl_wrapper.h"
#include "overloaded.h"

namespace mavsdk {

HttpLoader::HttpLoader() : _curl_wrapper(std::make_unique<CurlWrapper>(CurlWrapper()))
{
    start();
}

HttpLoader::~HttpLoader()
{
    stop();
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

bool HttpLoader::download_sync(const std::string& url, const std::string& local_path)
{
    WorkItem item = DownloadItem{url, local_path, nullptr};
    bool success = do_item(item);
    return success;
}

void HttpLoader::download_async(
    const std::string& url, const std::string& local_path, ProgressCallback progress_callback)
{
    _work_queue.push_back(
        std::make_shared<WorkItem>(DownloadItem{url, local_path, progress_callback}));
}

void HttpLoader::download_text_async(const std::string& url, TextDownloadCallback text_callback)
{
    _work_queue.push_back(std::make_shared<WorkItem>(DownloadTextItem{url, text_callback}));
}

void HttpLoader::work_thread()
{
    while (!_should_exit) {
        LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);

        auto work = work_queue_guard.wait_and_pop_front();

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
        item.get_url(), item.get_local_path(), item.get_progress_callback());
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
