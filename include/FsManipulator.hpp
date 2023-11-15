#ifndef FSMANIPULATOR_HPP
#define FSMANIPULATOR_HPP

#include <string>

class FsManipulator {
public:
    int is_dir_present(const std::string dir_path);
    int create_dir(const std::string dir_path);
    int delete_dir(const std::string dir_path);
    int is_file_present(const std::string file_path);
    int create_file(const std::string file_path);
    int delete_file(const std::string file_path);
    std::string read_config_var(const std::string file_path, const std::string config_var_name);
    std::string add_config_var(const std::string file_path, const std::string config_var_name, const std::string input_value);
    std::string update_config_var(const std::string file_path, const std::string config_var_name, const std::string input_value);
    std::string delete_config_var(const std::string file_path, const std::string config_var_name);
};

#endif // FSMANIPULATOR_HPP