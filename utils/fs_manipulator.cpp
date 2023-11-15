#include "FsManipulator.hpp"
#include "Logging.hpp"
#include <filesystem>
#include <iostream>
#include <libconfig.h++>
#include <string>
#include <fstream>

Logging logging;

int FsManipulator::is_dir_present(const std::string dir_path) {
    try {
        if (std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path)) {
            logging.log_to_file("Directory '" + dir_path + "' is Present.");
            return 0;
        }
        logging.log_to_file("Directory '" + dir_path + "' is not Present.");
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

int FsManipulator::create_dir(const std::string dir_path) {
    try {
        if (std::filesystem::create_directories(dir_path)) {
            logging.log_to_file("Directory '" + dir_path + "' was Created.");
            return 0;
        }
        logging.log_to_file("Directory '" + dir_path + "' was not Created.");
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

int FsManipulator::delete_dir(const std::string dir_path) {
    try {
        if (std::filesystem::remove_all(dir_path)) {
            logging.log_to_file("Directory '" + dir_path + "' was Deleted.");
            return 0;
        }
        logging.log_to_file("Directory '" + dir_path + "' was not Deleted.");
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

int FsManipulator::is_file_present(const std::string file_path) {
    try {
        if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)) {
            logging.log_to_file("File '" + file_path + "' is Present.");
            return 0;
        }
        logging.log_to_file("File '" + file_path + "' is not Present.");
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

int FsManipulator::create_file(const std::string file_path) {
    try {
        std::ofstream file(file_path);
        if (file.is_open()) {
            file.close();
            logging.log_to_file("File '" + file_path + "' was Created.");
            return 0;
        }
        logging.log_to_file("File '" + file_path + "' was not Created.");
        return -1;
    } catch (const std::ios_base::failure& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

int FsManipulator::delete_file(const std::string file_path) {
    try {
        if (std::filesystem::remove(file_path)) {
            logging.log_to_file("File '" + file_path + "' was Deleted.");
            return 0;
        }
        logging.log_to_file("File '" + file_path + "' was not Deleted.");
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        logging.log_to_file(e.what());
        return -1;
    }
}

std::string FsManipulator::read_config_var(const std::string file_path, const std::string config_var_name) {
    libconfig::Config config;
    try {
        config.readFile(file_path.c_str());
        const char *out_value;
        if (config.lookupValue(config_var_name, out_value)) {
            std::string result = out_value;
            logging.log_to_file("Config variable '" + config_var_name + "' was Read.");
            return result;
        } else {
            logging.log_to_file("Error: Config variable '" + config_var_name + "' was Not found.");
            return "VAR_NOT_FOUND";
        }
    } catch (const libconfig::FileIOException &fioex) {
        logging.log_to_file("Error: I/O error while reading file '" + file_path + "' for config variable '" + config_var_name + "'.");
        return "ERROR";
    } catch (const libconfig::ParseException &pex) {
        int getLine = pex.getLine();
        std::string getError = pex.getError();
        logging.log_to_file("Error: Parse error at " + file_path + ":" + std::to_string(getLine) + " - " + getError);
        return "ERROR";
    }
}

std::string FsManipulator::add_config_var(const std::string file_path, const std::string config_var_name, const std::string input_value) {
    libconfig::Config config;
    try {
        config.readFile(file_path.c_str());
        config.getRoot().add(config_var_name.c_str(), libconfig::Setting::TypeString) = input_value.c_str();
        config.writeFile(file_path.c_str());
        logging.log_to_file("Config variable '" + config_var_name + "' was Added.");
        return "SUCCESS";
    } catch (const libconfig::SettingException &sex) {
        std::string what = sex.what();
        logging.log_to_file("Error: Setting error: '" + what + "'.");
        return "ERROR";
    } catch (...) {
        logging.log_to_file("Error: An error occurred while adding the config var '" + config_var_name + "'.");
        return "ERROR";
    }
}

std::string FsManipulator::update_config_var(const std::string file_path, const std::string config_var_name, const std::string input_value) {
    libconfig::Config config;
    try {
        config.readFile(file_path.c_str());
        libconfig::Setting &setting = config.lookup(config_var_name);
        setting = input_value.c_str();
        config.writeFile(file_path.c_str());
        logging.log_to_file("Config variable '" + config_var_name + "' was updated.");
        return "SUCCESS";
    } catch (const libconfig::SettingNotFoundException &snfex) {
        logging.log_to_file("Error: No such config var '" + config_var_name + "'.");
        return "ERROR";
    } catch (...) {
        logging.log_to_file("Error: An error occurred while updating the config var '" + config_var_name + "'.");
        return "ERROR";
    }
}

std::string FsManipulator::delete_config_var(const std::string file_path, const std::string config_var_name) {
    libconfig::Config config;
    try {
        libconfig::Setting &root = config.getRoot();
        if(root.exists(config_var_name.c_str())) {
            root.remove(config_var_name.c_str());
            config.writeFile(file_path.c_str());
            logging.log_to_file("Config variable '" + config_var_name + "' was deleted.");
            return "SUCCESS";
        } else {
            logging.log_to_file("Error: No such config var '" + config_var_name + "'.");
            return "VAR_NOT_FOUND";
        }
    } catch (...) {
        logging.log_to_file("Error: An error occurred while deleting the config var '" + config_var_name + "'.");
        return "ERROR";
    }
}
