//
// Example how to use an FTP client with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>

#include <functional>
#include <future>
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace mavsdk;
using std::chrono::seconds;

void usage(const std::string& bin_name)
{
    std::cerr
        << "Usage : " << bin_name
        << " <connection_url> <server component id> <command> <parameters>\n"
        << '\n'
        << "Connection URL format should be :\n"
        << " For TCP : tcp://[server_host][:server_port]\n"
        << " For UDP : udp://[bind_host][:bind_port]\n"
        << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
        << "For example, to connect to the simulator use URL: udp://:14540\n"
        << '\n'
        << "Server component id is for example 1 for autopilot or 195 for companion computer,\n"
        << "which is being used if you run the ftp_server example\n"
        << '\n'
        << '\n'
        << "Supported commands :\n"
        << " put <file> <path>    : Upload one file to remote directory\n"
        << " get <file> <path>    : Download remote file to local directory\n"
        << " delete <file>        : Delete remote file\n"
        << " rename <old> <new>   : Rename file\n"
        << " dir <path>           : List contents of remote directory\n"
        << " mkdir <path>         : Make directory on remote machine\n"
        << " rmdir [-r] <path>    : Remove directory on remote machine. [-r] recursively" << '\n'
        << " cmp <local> <remote> : Compare local and remote file\n"
        << '\n'
        << "Return codes:\n"
        << " 0 : Success\n"
        << " 1 : Failure\n"
        << " 2 : File does not exist\n"
        << " 3 : Files are different (cmp command)\n";
}

Ftp::Result reset_server(Ftp& ftp)
{
    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.reset_async([&prom](Ftp::Result result) { prom.set_value(result); });
    return future_result.get();
}

Ftp::Result create_directory(Ftp& ftp, const std::string& path)
{
    std::cerr << "Creating directory: " << path << '\n';
    auto prom = std::promise<Ftp::Result>();
    auto future_result = prom.get_future();
    ftp.create_directory_async(path, [&prom](Ftp::Result result) { prom.set_value(result); });

    return future_result.get();
}

Ftp::Result remove_file(Ftp& ftp, const std::string& path)
{
    std::cerr << "Removing file: " << path << '\n';
    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.remove_file_async(path, [&prom](Ftp::Result result) { prom.set_value(result); });

    return future_result.get();
}

Ftp::Result remove_directory(Ftp& ftp, const std::string& path, bool recursive = true)
{
    if (recursive) {
        auto prom = std::promise<std::pair<Ftp::Result, std::vector<std::string>>>{};
        auto future_result = prom.get_future();
        ftp.list_directory_async(path, [&prom](Ftp::Result result, std::vector<std::string> list) {
            prom.set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
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
    std::cerr << "Removing dir:  " << path << '\n';

    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.remove_directory_async(path, [&prom](Ftp::Result result) { prom.set_value(result); });

    return future_result.get();
}

Ftp::Result list_directory(Ftp& ftp, const std::string& path)
{
    std::cerr << "List directory: " << path << '\n';
    auto prom = std::promise<std::pair<Ftp::Result, std::vector<std::string>>>{};
    auto future_result = prom.get_future();
    ftp.list_directory_async(path, [&prom](Ftp::Result result, std::vector<std::string> list) {
        prom.set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
    });

    std::pair<Ftp::Result, std::vector<std::string>> result = future_result.get();
    if (result.first == Ftp::Result::Success) {
        for (auto entry : result.second) {
            std::cerr << entry << '\n';
        }
    }
    return result.first;
}

Ftp::Result
download_file(Ftp& ftp, const std::string& remote_file_path, const std::string& local_path)
{
    std::cerr << "Download file: " << remote_file_path << " to " << local_path << '\n';
    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.download_async(
        remote_file_path, local_path, [&prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.total_bytes > 0 ?
                                     progress.bytes_transferred * 100 / progress.total_bytes :
                                     0;
                std::cerr << "\rDownloading [" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred >= progress.total_bytes) {
                    std::cerr << '\n';
                }
            } else {
                prom.set_value(result);
            }
        });

    return future_result.get();
}

Ftp::Result
upload_file(Ftp& ftp, const std::string& local_file_path, const std::string& remote_path)
{
    std::cerr << "Upload file: " << local_file_path << " to " << remote_path << '\n';
    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.upload_async(
        local_file_path, remote_path, [&prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.total_bytes > 0 ?
                                     progress.bytes_transferred * 100 / progress.total_bytes :
                                     0;
                std::cerr << "\rUploading "
                          << "[" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred == progress.total_bytes) {
                    std::cerr << '\n';
                }
            } else {
                prom.set_value(result);
            }
        });
    return future_result.get();
}

Ftp::Result rename_file(Ftp& ftp, const std::string& old_name, const std::string& new_name)
{
    std::cerr << "Rename file: " << old_name << " to " << new_name << '\n';
    auto prom = std::promise<Ftp::Result>{};
    auto future_result = prom.get_future();
    ftp.rename_async(old_name, new_name, [&prom](Ftp::Result result) { prom.set_value(result); });

    return future_result.get();
}

int main(int argc, char** argv)
{
    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // Instantiate plugins.

    auto ftp = Ftp{system.value()};
    try {
        ftp.set_target_compid(std::stoi(argv[2]));
    } catch (...) {
        std::cerr << "Invalid argument: " << argv[2] << '\n';
        return 1;
    }

    Ftp::Result res;
    res = reset_server(ftp);
    if (res != Ftp::Result::Success) {
        std::cerr << "Reset server error: " << res << '\n';
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
            std::cerr << "File uploaded.\n";
        } else {
            std::cerr << "File upload error: " << res << '\n';
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "get") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = download_file(ftp, argv[4], (argc == 6) ? argv[5] : ".");
        if (res == Ftp::Result::Success) {
            std::cerr << "File downloaded.\n";
        } else {
            std::cerr << "File download error: " << res << '\n';
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "rename") {
        if (argc < 6) {
            usage(argv[0]);
            return 1;
        }
        res = rename_file(ftp, argv[4], argv[5]);
        if (res == Ftp::Result::Success) {
            std::cerr << "File renamed.\n";
        } else {
            std::cerr << "File rename error: " << res << '\n';
            return (res == Ftp::Result::FileDoesNotExist) ? 2 : 1;
        }
    } else if (command == "mkdir") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = create_directory(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cerr << "Directory created.\n";
        } else if (res == Ftp::Result::FileExists) {
            std::cerr << "Directory already exists.\n";
        } else {
            std::cerr << "Create directory error: " << res << '\n';
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
            std::cerr << "Directory removed.\n";
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cerr << "Directory does not exist.\n";
            return 2;
        } else {
            std::cerr << "Remove directory error: " << res << '\n';
            return 1;
        }
    } else if (command == "dir") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = list_directory(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cerr << "Directory listed.\n";
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cerr << "Directory does not exist.\n";
            return 2;
        } else {
            std::cerr << "List directory error: " << res << '\n';
            return 1;
        }
    } else if (command == "delete") {
        if (argc < 5) {
            usage(argv[0]);
            return 1;
        }
        res = remove_file(ftp, argv[4]);
        if (res == Ftp::Result::Success) {
            std::cerr << "File deleted.\n";
        } else if (res == Ftp::Result::FileDoesNotExist) {
            std::cerr << "File does not exist.\n";
            return 2;
        } else {
            std::cerr << "Delete file error: " << res << '\n';
            return 1;
        }
    } else if (command == "cmp") {
        if (argc < 6) {
            usage(argv[0]);
            return 1;
        }

        auto prom = std::promise<std::pair<Ftp::Result, bool>>{};
        auto future_result = prom.get_future();

        ftp.are_files_identical_async(
            argv[4], argv[5], [&prom](Ftp::Result result, bool identical) {
                prom.set_value(std::make_pair<>(result, identical));
            });

        auto result = future_result.get();

        if (result.first != Ftp::Result::Success) {
            std::cerr << "Error comparing files:" << result.first << '\n';
            return 1;
        }

        if (result.second) {
            std::cerr << "Files are equal\n";
        } else {
            std::cerr << "Files are different\n";
            return 3;
        }
    } else {
        std::cerr << "Unknown command: " << command << '\n';
        return 1;
    }

    return 0;
}
