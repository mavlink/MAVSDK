#include <download_manager.h>
#include <curl_wrapper.h>


namespace dronelink {

DownloadManager::DownloadManager()
{
    start();
}

DownloadManager::~DownloadManager()
{
    stop();
}

void DownloadManager::start()
{
    _should_exit = false;
    _work_thread = new std::thread(work_thread, this);
}

void DownloadManager::stop()
{
    _should_exit = true;
    _work_queue.stop();
    if (_work_thread != nullptr) {
        _work_thread->join();
        delete _work_thread;
        _work_thread = nullptr;
    }
}

bool DownloadManager::download_sync(const std::string &url, const std::string &local_path)
{
    auto work_item = std::make_shared<DownloadItem>(url, local_path, nullptr);
    bool success = do_download(work_item);
    return success;
}

void DownloadManager::download_async(const std::string &url, const std::string &local_path,
                                     const progress_callback_t &progress_callback)
{
    auto work_item = std::make_shared<DownloadItem>(url, local_path, progress_callback);
    _work_queue.enqueue(work_item);
}

bool DownloadManager::upload_sync(const std::string &target_url, const std::string &local_path)
{
    auto work_item = std::make_shared<UploadItem>(target_url, local_path, nullptr);
    bool success = do_upload(work_item);
    return success;
}

void DownloadManager::upload_async(const std::string &target_url, const std::string &local_path,
                                   const progress_callback_t &progress_callback)
{
    auto work_item = std::make_shared<UploadItem>(target_url, local_path, progress_callback);
    _work_queue.enqueue(work_item);
}

void DownloadManager::work_thread(DownloadManager *self)
{
    while (!self->_should_exit) {
        auto item = self->_work_queue.dequeue();
        if (item == nullptr) {
            continue;
        }
        do_item(item);
    }
}

void DownloadManager::do_item(const std::shared_ptr<WorkItem> &item)
{
    auto download_item = std::dynamic_pointer_cast<DownloadItem>(item);
    if (nullptr != download_item) {
        do_download(download_item);
        return;
    }

    auto upload_item = std::dynamic_pointer_cast<UploadItem>(item);
    if (nullptr != upload_item) {
        do_upload(upload_item);
        return;
    }
}

bool DownloadManager::do_download(const std::shared_ptr<DownloadItem> &item)
{
    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.download_file_to_path(item->get_url(), item->get_local_path(),
                                                      item->get_progress_callback());
    return success;
}

bool DownloadManager::do_upload(const std::shared_ptr<UploadItem> &item)
{
    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.upload_file(item->get_target_url(), item->get_local_path(),
                                            item->get_progress_callback());
    if (success == false) {
        auto callback = item->get_progress_callback();
        callback(100);
    }

    return success;
}

bool DownloadManager::download_text_sync(const std::string &url, std::string &content)
{
    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.download_text(url, content);
    return success;
}


} // namespace dronelink


