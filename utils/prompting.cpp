// Prompting.cpp
#include "Prompting.hpp"
#include "FsManipulator.hpp"
#include <string>
#include <iostream>
#include <limits>  // For std::numeric_limits

std::string Prompting::prompt_user_for_var(const std::string file_path, const std::string var_name, const std::string default_value) {
    FsManipulator fs;
    std::string current_value = fs.read_config_var(file_path, var_name);

    std::cout << "Enter value for " << var_name;
    if (!default_value.empty()) {
        std::cout << " (default: " << default_value << ")";
    }
    std::cout << ": ";
    
    std::string user_input;
    std::getline(std::cin, user_input);
    if (user_input.empty() && !default_value.empty()) {
        user_input = default_value;
    }
    while (user_input.empty()) {
        std::cout << var_name << " is required. Please enter value for " << var_name << ": ";
        std::getline(std::cin, user_input);
    }

    if(current_value == "VAR_NOT_FOUND") {
        fs.add_config_var(file_path, var_name, user_input);
    }

    fs.update_config_var(file_path, var_name, user_input);
    return user_input;
}

std::string Prompting::is_prompt_required(const std::string file_path, const std::string var_name, const std::string default_value, const bool update_var) {
    FsManipulator fs;
    std::string current_value = fs.read_config_var(file_path, var_name);
    // std::cout << current_value << "\n";
    if (current_value == "VAR_NOT_FOUND" || current_value == "ERROR" || current_value.empty()) {
        return prompt_user_for_var(file_path, var_name, default_value);
    } else if(!update_var){
        return current_value;
    } else {
        std::cout << "Current value of " << var_name << " is " << current_value << "." << std::endl;
        std::string input;
        int choice = 2;  // Default choice is 2
        std::cout << "Would you like to change it? (1 - Change, 2 - Don't change): ";
        std::getline(std::cin, input);  // Read a line of input

        // If input is not empty, try to convert it to an integer
        if (!input.empty()) {
            try {
                choice = std::stoi(input);  // Convert string to int
            } catch (const std::invalid_argument&) {
                std::cout << "Invalid input. Assuming choice 2." << std::endl;
                choice = 2;
            }
        }

        // Now process the choice
        if (choice == 1) {
            return prompt_user_for_var(file_path, var_name, default_value);
        } else {
            return current_value;
        }
    }
    return current_value;
}
