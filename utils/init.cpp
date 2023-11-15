#include "Init.hpp"
#include "FsManipulator.hpp"
#include "LoadEnv.hpp"
#include "Logging.hpp"
#include "SshWorker.hpp"
#include "GitOperations.hpp"
#include "NetworkChecker.hpp"
#include <string>
#include <list>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>


std::string Init::init_for_linux_desktop(const std::string root_dir, const std::string workspace_dir) {
    std::string config_file_path = root_dir + "/config.txt";
    GitOperations gitOperations;
    FsManipulator fsManipulator;
    NetworkChecker networkChecker;
    SshWorker sshWorker;
    LoadEnv loadEnv;
    Logging logging;

    logging.log_to_file("\n\n\n------------STARTED THE PROGRAM FROM THE COMMAND LINE--------------\n");
    
    // Check for root directory and create if not present
    if (fsManipulator.is_dir_present(root_dir) != 0) {
        int dir_creation_result = fsManipulator.create_dir(root_dir);
        if (dir_creation_result != 0) {
            logging.log_to_file("Error: Error creating root directory: '" + root_dir + "'.");
            return "Error";
        }
    }
    
    // Check for config file and create if not present
    if (fsManipulator.is_file_present(config_file_path) != 0) {
        int file_creation_result = fsManipulator.create_file(config_file_path);
        if (file_creation_result != 0) {
            logging.log_to_file("Error: Error creating config file: '" + config_file_path + "'.");
            return "Error";
        }
    }
    
    // Load environment variables from config file
    nlohmann::json json_data = nlohmann::json::array({
        {{"name", "GIT_HOST"}, {"default", "github.com"}},
        {{"name", "GIT_USERNAME"}, {"default", "username"}},
        {{"name", "MAIN_REPOSITORY"}, {"default", "ObsidianGitVault"}},
        {{"name", "SSH_KEY_NAME"}, {"default", "ObsidianGitSync"}}
    });
    std::string load_env_result = loadEnv.load_env_variables(config_file_path, json_data, false);

    if (load_env_result != "Success!") {
        logging.log_to_file("Error: Initialization failed: '" + load_env_result + "'.");
        return "Failed!";  // Return an error code
    }

    std::string git_host = fsManipulator.read_config_var(config_file_path, "GIT_HOST");
    std::string git_username = fsManipulator.read_config_var(config_file_path, "GIT_USERNAME");
    std::string git_repo_name = fsManipulator.read_config_var(config_file_path, "MAIN_REPOSITORY");

    std::string programEmail = "264351479+" + git_username + "+ObsidianGitSync@users.noreply." + git_host;
    std::string backup_dir = workspace_dir + "/." + git_repo_name;

    if(fsManipulator.read_config_var(config_file_path, "ROOT_DIR") != root_dir){
        if(fsManipulator.read_config_var(config_file_path, "ROOT_DIR") == "VAR_NOT_FOUND"){
            fsManipulator.add_config_var(config_file_path, "ROOT_DIR", root_dir);
        } else {
            fsManipulator.update_config_var(config_file_path, "ROOT_DIR", root_dir);
        }
    }
    if(fsManipulator.read_config_var(config_file_path, "WORKSPACE_DIR") != workspace_dir){
        if(fsManipulator.read_config_var(config_file_path, "WORKSPACE_DIR") == "VAR_NOT_FOUND"){
            fsManipulator.add_config_var(config_file_path, "WORKSPACE_DIR", workspace_dir);
        } else {
            fsManipulator.update_config_var(config_file_path, "WORKSPACE_DIR", workspace_dir);
        }
    }
    if(fsManipulator.read_config_var(config_file_path, "BACKUP_DIR") != backup_dir){
        if(fsManipulator.read_config_var(config_file_path, "BACKUP_DIR") == "VAR_NOT_FOUND"){
            fsManipulator.add_config_var(config_file_path, "BACKUP_DIR", backup_dir);
        } else {
            fsManipulator.update_config_var(config_file_path, "BACKUP_DIR", backup_dir);
        }
    }
    if(fsManipulator.read_config_var(config_file_path, "SSH_DIR") != root_dir + "/.ssh"){
        if(fsManipulator.read_config_var(config_file_path, "SSH_DIR") == "VAR_NOT_FOUND"){
            fsManipulator.add_config_var(config_file_path, "SSH_DIR", root_dir + "/.ssh");
        } else {
            fsManipulator.update_config_var(config_file_path, "SSH_DIR", root_dir + "/.ssh");
        }
    }
    if(fsManipulator.read_config_var(config_file_path, "COMMITS_EMAIL") != programEmail){
        if(fsManipulator.read_config_var(config_file_path, "COMMITS_EMAIL") == "VAR_NOT_FOUND"){
            fsManipulator.add_config_var(config_file_path, "COMMITS_EMAIL", programEmail);
        } else {
            fsManipulator.update_config_var(config_file_path, "COMMITS_EMAIL", programEmail);
        }
    }

    if(sshWorker.is_ssh_setup(config_file_path) != 0) {
        sshWorker.setup_new_ssh_keypair(config_file_path);
    }

    if(gitOperations.is_repo_present(config_file_path) != 0) {
        gitOperations.clone_repo(config_file_path);
    }

    git_repository *repo = gitOperations.open_repo(config_file_path);
    while (repo != nullptr) {  // This will run indefinitely
        logging.log_to_file("\n\n------------------Infinite Scanning Cycle Start---------------\n");
        if(networkChecker.checkConnectivity() == 0) {
            if(gitOperations.are_unindexed_changes_present(repo) == 0) {
                    if (gitOperations.are_unindexed_changes_present(repo) == 0) {
                        gitOperations.add_to_index(repo);
                    }
            }

            if(gitOperations.are_uncommitted_changes_present(repo) == 0) {
                gitOperations.create_commit(config_file_path, repo);
            }
            
            if(gitOperations.are_unpushed_commits_present(repo) == 0) {
                gitOperations.push_to_remote(config_file_path, repo);
            }

        }

        std::this_thread::sleep_for(std::chrono::seconds(60));  // Sleep for 60 seconds
        logging.log_to_file("\n\n------------------Infinite Scanning Cycle End---------------\n");
    }
    
    return "Success!";
}
