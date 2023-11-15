#ifndef PROMPTING_HPP
#define PROMPTING_HPP

#include <string>

class Prompting {
public:
    std::string prompt_user_for_var(const std::string file_path, const std::string var_name, const std::string default_value = "");
    std::string is_prompt_required(const std::string file_path, const std::string var_name, const std::string default_value = "", const bool update_var = false);
};

#endif // PROMPTING_HPP