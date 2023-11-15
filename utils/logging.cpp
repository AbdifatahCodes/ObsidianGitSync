#include "Logging.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

int is_dir_present(const std::string dir_path) {
    try {
        if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
            return 0;
        }
        return -1;
    } catch (const fs::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

int is_file_present(const std::string file_path) {
    try {
        if (fs::exists(file_path) && fs::is_regular_file(file_path)) {
            return 0;
        }
        return -1;
    } catch (const fs::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

int Logging::log_to_file(const std::string new_data) {
    std::string temp_log_path = fs::path(std::string(std::getenv("HOME")) + "/.tempOGRlog.txt");
    std::string ideal_log_dir = fs::path(std::string(std::getenv("HOME")) + "/.config/ObsidianGitSync");
    std::string ideal_log_path = ideal_log_dir + "/logfile.txt";

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " -:- " << new_data;
    std::string timestamped_data = ss.str();
    
    if (is_file_present(ideal_log_path) == 0) {
        std::ofstream log_file(ideal_log_path, std::ios::app);
        log_file << timestamped_data << std::endl;
        log_file.close();
    } else {
        if (is_dir_present(ideal_log_dir) == 0) {
            if (is_file_present(temp_log_path) == 0) {
                fs::rename(temp_log_path, ideal_log_path);
                std::ofstream log_file(ideal_log_path, std::ios::app);
                log_file << timestamped_data << std::endl;
                log_file.close();
            } else {
                std::ofstream log_file(ideal_log_path);
                log_file << timestamped_data << std::endl;
                log_file.close();
            }
        } else {
            if (is_file_present(temp_log_path) == 0) {
                std::ofstream log_file(temp_log_path, std::ios::app);
                log_file << timestamped_data << std::endl;
                log_file.close();
            } else {
                std::ofstream log_file(temp_log_path);
                log_file << timestamped_data << std::endl;
                log_file.close();
            }
        }
    }

    // Use the ideal log path only when present
    std::string log_path = temp_log_path;
    if (is_file_present(ideal_log_path) == 0) {
        log_path = ideal_log_path;
    }

    // Ensure log file size does not exceed 10MB.
    if (fs::file_size(log_path) > 10 * 1024 * 1024) {
        std::vector<std::string> lines;
        std::string line;
        std::ifstream log_file(log_path);
        while (std::getline(log_file, line)) {
            lines.push_back(line);
        }
        log_file.close();
        lines.erase(lines.begin(), lines.begin() + (lines.size() / 10));  // Remove first half of the log entries.
        std::ofstream log_file_out(log_path);
        for (const auto& log_line : lines) {
            log_file_out << log_line << std::endl;
        }
        log_file_out.close();
    }

    return 0;
}
