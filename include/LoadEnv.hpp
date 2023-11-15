#ifndef LOADENV_HPP
#define LOADENV_HPP

#include <string>
#include <nlohmann/json.hpp>

class LoadEnv {
public:
    std::string load_env_variables(const std::string file_path, const nlohmann::json json_data, const bool update_var);
    std::string get_env_var(const std::string file_path, const std::string var_name, const bool update_var);
};

#endif // LOADENV_HPP