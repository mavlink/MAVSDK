/**
 * @file ftp_client.cpp
 *
 * @brief Demonstrates how to use a FTP client with MAVSDK.
 *
 * @author Matej Frančeškin <matej@auterion.com>,
 * @date 2019-09-06
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>

#include <functional>
#include <future>
#include <iostream>
#include <iomanip>
#include <cstring>

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name
              << " <connection_url> <server component id> <command> <parameters>" << std::endl
              << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl
              << std::endl
              << "Server component id is for example 1 for autopilot or 240 for companion computer."
              << std::endl
              << std::endl
              << "Supported commands :" << std::endl
              << " put <file> <path>    : Upload one file to remote directory" << std::endl
              << " get <file> <path>    : Download remote file to local directory" << std::endl
              << " delete <file>        : Delete remote file" << std::endl
              << " rename <old> <new>   : Rename file" << std::endl
              << " dir <path>           : List contents of remote directory" << std::endl
              << " mkdir <path>         : Make directory on remote machine" << std::endl
              << " rmdir [-r] <path>    : Remove directory on remote machine. [-r] recursively"
              << std::endl
              << " cmp <local> <remote> : Compare local and remote file" << std::endl
              << std::endl
              << "Return codes:" << std::endl
              << " 0 : Success" << std::endl
              << " 1 : Failure" << std::endl
              << " 2 : File does not exist" << std::endl
              << " 3 : Files are different (cmp command)" << std::endl;
}

Ftp::Result reset_server(std::shared_ptr<Ftp>& ftp)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->reset_async([prom](Ftp::Result result) { prom->set_value(result); });
    return future_result.get();
}

Ftp::Result create_directory(std::shared_ptr<Ftp>& ftp, const std::string& path)
{
    std::cout << "Creating directory: " << path << std::endl;
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->create_directory_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

Ftp::Result remove_file(std::shared_ptr<Ftp>& ftp, const std::string& path)
{
    std::cout << "Removing file: " << path << std::endl;
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->remove_file_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

Ftp::Result
remove_directory(std::shared_ptr<Ftp>& ftp, const std::string& path, bool recursive = true)
{
    if (recursive) {
        auto prom =
            std::make_shared<std::promise<std::pair<Ftp::Result, std::vector<std::string>>>>();
        auto future_result = prom->get_future();
        ftp->list_directory_async(path, [prom](Ftp::Result result, std::vector<std::string> list) {
            prom->set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
        });

        std::pair<Ftp::Result, std::vector<std::string>> result = future_result.get();
        if (result.first == Ftp::Result::Success) {
            for (auto entry : result.second) {
                if (entry[0] == 'D') {
                    remove_directory(ftp, path + "/" + entry.substr(1, entry.size() - 1));
                } else if (entry[0] == 'F') {
                    auto i = entry.find('\t');
                    std::string name = entry.substr(1, i - 1);
                    remove_file(ftp, path + "/" + name);
                }
            }
        }
    }
    std::cout << "Removing dir:  " << path << std::endl;

    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->remove_directory_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

Ftp::Result list_directory(std::shared_ptr<Ftp>& ftp, const std::string& path)
{
    std::cout << "List directory: " << path << std::endl;
    auto prom = std::make_shared<std::promise<std::pair<Ftp::Result, std::vector<std::string>>>>();
    auto future_result = prom->get_future();
    ftp->list_directory_async(path, [prom](Ftp::Result result, std::vector<std::string> list) {
        prom->set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
    });

    std::pair<Ftp::Result, std::vector<std::string>> result = future_result.get();
    if (result.first == Ftp::Result::Success) {
        for (auto entry : result.second) {
            std::cout << entry << std::endl;
        }
    }
    return result.first;
}

Ftp::Result download_file(
    std::shared_ptr<Ftp>& ftp, const std::string& remote_file_path, const std::string& local_path)
{
    std::cout << "Download file: " << remote_file_path << " to " << local_path << std::endl;
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->download_async(
        remote_file_path, local_path, [prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.bytes_transferred * 100 / progress.total_bytes;
                std::cout << "\rDownloading [" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred >= progress.total_bytes) {
                    std::cout << std::endl;
                }
            } else {
                prom->set_value(result);
            }
        });

    return future_result.get();
}

Ftp::Result upload_file(
    std::shared_ptr<Ftp>& ftp, const std::string& local_file_path, const std::string& remote_path)
{
    std::cout << "Upload file: " << local_file_path << " to " << remote_path << std::endl;
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->upload_async(
        local_file_path, remote_path, [prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.bytes_transferred * 100 / progress.total_bytes;
                std::cout << "\rUploading "
                          << "[" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred == progress.total_bytes) {
                    std::cout << std::endl;
                }
            } else {
                prom->set_value(result);
            }
        });
    return future_result.get();
}

Ftp::Result
rename_file(std::shared_ptr<Ftp>& ftp, const std::string& old_name, const std::string& new_name)
{
    std::cout << "Rename file: " << old_name << " to " << new_name << std::endl;
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->rename_async(old_name, new_name, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

int main(int argc, char** argv)
{
    Mavsdk mavsdk;

    auto prom = std::make_shared<std::promise<void>>();
    auto future_result = prom->get_future();

    std::cout << NORMAL_CONSOLE_TEXT << "Waiting to discover system..." << std::endl;
    mavsdk.subscribe_on_change([&mavsdk, prom]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            prom->set_value();
        }
    });

    std::string connection_url;
    ConnectionResult connection_result;

    if (argc >= 4) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    auto status = future_result.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::timeout) {
        std::cout << "Timeout waiting for connection." << std::endl;
        return 1;
    }

    future_result.get();

    auto system = mavsdk.systems().at(0);
    auto ftp = std::make_shared<Ftp>(system);
    try {
        ftp->set_target_compid(std::stoi(argv[2]));
    } catch (...) {
        std::cout << ERROR_CONSOLE_TEXT << "Invalid argument: " << argv[2] << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    Ftp::Result res;
    res = reset_server(ftp);
    if (res != Ftp::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Reset server error: " << res << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    std::string command = argv[3];

    if (command == "put") {
        if (argc < 6) {
            usage(argv[0]);
            return 1;
        }
        res = upload_file(ftp, argv[4], argv[5]);
        if (res == Ftp::Result::Success) {
            std::cout << "File uploaded." << std::endl;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "File upload error: " << res << NORMAL_CONSOLE_TEXT
                      << std::endl;
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "get") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = download_file(ftp, argv[4], (argc == 6) ? argv[5] : ".");
        if (res == Ftp::Result::Success) {
            std::cout << "File downloaded." << std::endl;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "File download error: " << res << NORMAL_CONSOLE_TEXT
                      << std::endl;
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "rename") {
        if (argc < 6) {
            usage(argv[0]);
            return 1;
        }
        res = rename_file(ftp, argv[4], argv[5]);
        if (res == Ftp::Result::Success) {
            std::cout << "File renamed." << std::endl;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "File rename error: " << res << NORMAL_CONSOLE_TEXT
                      << std::endl;
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "mkdir") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = create_directory(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cout << "Directory created." << std::endl;
        } else if (res == Ftp::Result::FileExists) {
            std::cout << "Directory already exists." << std::endl;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "Create directory error: " << res
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    } else if (command == "rmdir") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        std::string path = argv[4];
        bool recursive = false;
        if (argc == 6) {
            if (path == "-r") {
                recursive = true;
                path = argv[5];
            } else if (std::string(argv[5]) == "-r") {
                recursive = true;
            }
        }
        res = remove_directory(ftp, path, recursive);
        if (res == Ftp::Result::Success) {
            std::cout << "Directory removed." << std::endl;
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cout << "Directory does not exist." << std::endl;
            return 2;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "Remove directory error: " << res
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    } else if (command == "dir") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = list_directory(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cout << "Directory listed." << std::endl;
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cout << "Directory does not exist." << std::endl;
            return 2;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "List directory error: " << res
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    } else if (command == "delete") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = remove_file(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cout << "File deleted." << std::endl;
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cout << "File does not exist." << std::endl;
            return 2;
        } else {
            std::cout << ERROR_CONSOLE_TEXT << "Delete file error: " << res << NORMAL_CONSOLE_TEXT
                      << std::endl;
            return 1;
        }
    } else if (command == "cmp") {
        if (argc < 6) {
            usage(argv[0]);
            return 1;
        }

        auto prom = std::make_shared<std::promise<std::pair<Ftp::Result, bool>>>();
        auto future_result = prom->get_future();

        ftp->are_files_identical_async(
            argv[4], argv[5], [&prom](Ftp::Result result, bool identical) {
                prom->set_value(std::make_pair<>(result, identical));
            });

        auto result = future_result.get();

        if (result.first != Ftp::Result::Success) {
            std::cout << ERROR_CONSOLE_TEXT << "Error comparing files." << NORMAL_CONSOLE_TEXT
                      << std::endl;
            return 1;
        }

        if (result.second) {
            std::cout << "Files are equal" << std::endl;
        } else {
            std::cout << "Files are different" << std::endl;
            return 3;
        }
    } else {
        std::cout << ERROR_CONSOLE_TEXT << "Unknown command: " << command << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    return 0;
}
