#ifndef GITOPERATIONS_HPP
#define GITOPERATIONS_HPP

#include <git2.h>
#include <string>

class GitOperations {
public:
    // For the is_repo_present method, use the var workspace_dir to look for a folder with the same name as var main_repository.
    // If the folder is available, verify that it is a valid git repo using the var imported from the config.
    // Try connecting to it using the provided credentials and links from the config.
    // If all passes, return 0, if not, return a non-zero and log the error using Logging class methods.
    int is_repo_present(const std::string config_path);
    // Clone the repo using git2 and return a 0 for success and a non zero for error while logging everything.
    int clone_repo(const std::string config_path);
    git_repository* open_repo(const std::string config_path);
    // are_changes_present checks for changes that are not commited and returns a 0 for true and non-zero for errors. Log everything.
    int are_unindexed_changes_present(git_repository *repo);
    // add_to_index would detect any new changes in the repo and add them to index to prepare them for a commit.
    int add_to_index(git_repository *repo);
    int are_uncommitted_changes_present(git_repository *repo);
    int are_unpushed_commits_present(git_repository *repo);
    // create_commit commits everything in the index and uses the vars git_username and commits_email from the config file. Return 0 or non-zero as above and log everthing.
    int create_commit(const std::string config_path, git_repository *repo);
    // push_to_remote pushes the commits to remote. Return 0 or non-zero like above and log everything.
    int push_to_remote(const std::string config_path, git_repository *repo);
    // Fetches latest changes from the remote repository.
    int fetch_remote(const std::string config_path, git_repository *repo);
    // Method to handle non-fast-forward error
    int handle_non_fast_forward_error(const std::string config_path);
    // Backup everything in repo_dir except .git dir to backup_dir
    int backup_user_notes(const std::string repo_dir, const std::string backup_dir);
    // Replace the contents of repo_dir except .git dir with the contents of backup_dir and delete backup_dir if success.
    int recover_user_notes(const std::string repo_dir, const std::string backup_dir);

};

#endif // GITOPERATIONS_HPP