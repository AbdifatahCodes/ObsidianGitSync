#include "LoadEnv.hpp"
#include "Prompting.hpp"
#include <list>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

std::string LoadEnv::load_env_variables(const std::string file_path, const nlohmann::json json_data, const bool update_var) {
    Prompting prompting;
    std::string result;
    for (const auto& var_obj : json_data) {
        std::string var_name = var_obj["name"];
        std::string default_value = var_obj["default"];
        std::string value = prompting.is_prompt_required(file_path, var_name, default_value, update_var);
    }
    return "Success!";
}

std::string LoadEnv::get_env_var(const std::string file_path, const std::string var_name, const bool update_var) {
    Prompting prompting;
    std::string value = prompting.is_prompt_required(file_path, var_name, "", update_var);
    return value;
}
