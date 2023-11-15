#include "GitOperations.hpp"
#include "FsManipulator.hpp"
#include "Logging.hpp"
#include <git2.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int cred_acquire_cb(git_cred **cred, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload) {
    FsManipulator fs;

    std::string* config_path_ptr = reinterpret_cast<std::string*>(payload);
    std::string config_path = *config_path_ptr;

    std::string ssh_dir = fs.read_config_var(config_path, "SSH_DIR");
    std::string ssh_key_name = fs.read_config_var(config_path, "SSH_KEY_NAME");

    std::string ssh_pub_key_path = ssh_dir + "/" + ssh_key_name + ".pub";
    std::string ssh_priv_key_path = ssh_dir + "/" + ssh_key_name;

    return git_cred_ssh_key_new(cred, username_from_url, ssh_pub_key_path.c_str(), ssh_priv_key_path.c_str(), "");
}

int GitOperations::is_repo_present(const std::string config_path) {
    git_libgit2_init();
    FsManipulator fs;
    Logging log;
    std::string repo_dir = fs.read_config_var(config_path, "WORKSPACE_DIR") + "/" + fs.read_config_var(config_path, "MAIN_REPOSITORY");
    if (fs.is_dir_present(repo_dir) != 0) {
        log.log_to_file("Error: Repository directory not found");
        git_libgit2_shutdown();
        return -1;
    }
    git_repository *repo = nullptr;
    if (git_repository_open(&repo, repo_dir.c_str()) != 0) {
        log.log_to_file("Error: Failed to open repository");
        git_libgit2_shutdown();
        return -1;
    }
    git_repository_free(repo);
    log.log_to_file("Repository is present and valid");
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::clone_repo(const std::string config_path) {
    git_libgit2_init();
    FsManipulator fs;
    Logging log;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    git_repository *repo = nullptr;

    clone_opts.fetch_opts.callbacks.credentials = cred_acquire_cb;
    clone_opts.fetch_opts.callbacks.payload = (void*)&config_path;

    std::string url = "ssh://git@" + fs.read_config_var(config_path, "GIT_HOST") + "/" + fs.read_config_var(config_path, "GIT_USERNAME") + "/" + fs.read_config_var(config_path, "MAIN_REPOSITORY") + ".git";
    std::string path = fs.read_config_var(config_path, "WORKSPACE_DIR") + "/" + fs.read_config_var(config_path, "MAIN_REPOSITORY");

    int error = git_clone(&repo, url.c_str(), path.c_str(), &clone_opts);
    if (error != 0) {
        const git_error *err = giterr_last();
        std::string message = err->message;
        log.log_to_file("Error: Failed to clone repository with message: '" + message + "'.");
        git_libgit2_shutdown();
        return -1;
    }
    git_repository_free(repo);
    log.log_to_file("Repository cloned successfully");

    git_libgit2_shutdown();
    return 0;
}

git_repository* GitOperations::open_repo(const std::string config_path) {
    git_libgit2_init();
    // Changed return type to git_repository*
    FsManipulator fs;
    Logging log;
    std::string repo_dir = fs.read_config_var(config_path, "WORKSPACE_DIR") + "/" + fs.read_config_var(config_path, "MAIN_REPOSITORY");
    git_repository *repo = nullptr;
    if (git_repository_open(&repo, repo_dir.c_str()) != 0) {
        log.log_to_file("Error: Failed to open repository");
        git_libgit2_shutdown();
        return nullptr;  // Changed to return nullptr on error
    }
    log.log_to_file("Repository opened successfully");
    git_libgit2_shutdown();
    return repo;
}

int GitOperations::are_unindexed_changes_present(git_repository *repo) {
    git_libgit2_init();
    Logging log;
    git_status_options status_opts = GIT_STATUS_OPTIONS_INIT;
    status_opts.show = GIT_STATUS_SHOW_WORKDIR_ONLY;  // Show working directory only status
    status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX;  // Include untracked and renamed files

    git_status_list *status;
    if (git_status_list_new(&status, repo, &status_opts) != 0) {
        log.log_to_file("Error: Failed to retrieve status");
        git_libgit2_shutdown();
        return -1;
    }
    size_t status_count = git_status_list_entrycount(status);
    git_status_list_free(status);
    if (status_count == 0) {
        log.log_to_file("No Unindexed changes detected");
        git_libgit2_shutdown();
        return 1;  
    }
    log.log_to_file("Unindexed Changes detected");
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::add_to_index(git_repository *repo) {
    git_libgit2_init();
    Logging log;
    git_index *index = nullptr;
    if (git_repository_index(&index, repo) != 0) {
        log.log_to_file("Error: Failed to get repository index");
        git_libgit2_shutdown();
        return -1;
    }
    if (git_index_add_all(index, nullptr, 0, nullptr, nullptr) != 0) {
        log.log_to_file("Error: Failed to add changes to index");
        git_index_free(index);
        git_libgit2_shutdown();
        return -1;
    }
    if (git_index_write(index) != 0) {
        log.log_to_file("Error: Failed to write index");
        git_index_free(index);
        git_libgit2_shutdown();
        return -1;
    }
    git_index_free(index);
    log.log_to_file("Successfully added changes to index");
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::are_uncommitted_changes_present(git_repository *repo) {
    git_libgit2_init();
    Logging log;
    git_status_options status_opts = GIT_STATUS_OPTIONS_INIT;
    status_opts.show = GIT_STATUS_SHOW_INDEX_ONLY;  // Show index only status
    status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX;  // Include untracked and renamed files

    git_status_list *status;
    if (git_status_list_new(&status, repo, &status_opts) != 0) {
        log.log_to_file("Error: Failed to retrieve status");
        git_libgit2_shutdown();
        return -1;
    }
    size_t status_count = git_status_list_entrycount(status);
    git_status_list_free(status);
    if (status_count == 0) {
        log.log_to_file("No uncommitted changes detected");
        git_libgit2_shutdown();
        return 1;  
    }
    log.log_to_file("Uncommitted changes detected");
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::create_commit(const std::string config_path, git_repository *repo) {
    git_libgit2_init();
    FsManipulator fs;
    Logging log;
    git_signature *sig = nullptr;
    git_index *index = nullptr;
    git_oid tree_id, commit_id;
    git_tree *tree = nullptr;
    
    if (git_signature_now(&sig, fs.read_config_var(config_path, "GIT_USERNAME").c_str(), fs.read_config_var(config_path, "COMMITS_EMAIL").c_str()) != 0) {
        log.log_to_file("Error: Failed to create signature");
        git_libgit2_shutdown();
        return -1;
    }
    if (git_repository_index(&index, repo) != 0 || git_index_write_tree(&tree_id, index) != 0 || git_tree_lookup(&tree, repo, &tree_id) != 0) {
        log.log_to_file("Error: Index or tree operation failed");
        git_signature_free(sig);
        git_index_free(index);
        git_libgit2_shutdown();
        return -1;
    }
    git_oid parent_id;
    git_reference_name_to_id(&parent_id, repo, "HEAD");
    git_commit *parent = nullptr;
    git_commit_lookup(&parent, repo, &parent_id);
    if (git_commit_create_v(&commit_id, repo, "HEAD", sig, sig, NULL, "Commit made via ObsidianGitSync", tree, 1, parent) != 0) {
        log.log_to_file("Error: Failed to create commit");
        git_signature_free(sig);
        git_index_free(index);
        git_tree_free(tree);
        git_libgit2_shutdown();
        return -1;
    }
    git_signature_free(sig);
    git_index_free(index);
    git_tree_free(tree);
    log.log_to_file("Commit created successfully");
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::are_unpushed_commits_present(git_repository *repo) {
    git_libgit2_init();
    Logging log;
    int error = 0;

    git_oid local_oid, remote_oid;
    git_reference *head_ref = nullptr, *remote_ref = nullptr;
    const char *branch_name = nullptr;
    char remote_branch_ref_name[1024];

    // Get the current HEAD
    error = git_repository_head(&head_ref, repo);
    if (error != 0) {
        log.log_to_file("Error: Failed to get HEAD reference");
        git_libgit2_shutdown();
        return -1;
    }

    // Get the branch name from HEAD
    branch_name = git_reference_shorthand(head_ref);
    if (!branch_name) {
        log.log_to_file("Error: Failed to get branch name from HEAD reference");
        git_reference_free(head_ref);
        git_libgit2_shutdown();
        return -1;
    }

    // Construct the reference name of the remote tracking branch
    snprintf(remote_branch_ref_name, sizeof(remote_branch_ref_name), "refs/remotes/origin/%s", branch_name);

    // Look up the remote tracking branch
    error = git_reference_lookup(&remote_ref, repo, remote_branch_ref_name);
    if (error != 0) {
        log.log_to_file("Error: Failed to find remote tracking branch");
        git_reference_free(head_ref);
        git_libgit2_shutdown();
        return -1;
    }

    // Get the OIDs for local and remote
    const git_oid *local_oid_ptr = git_reference_target(head_ref);
    const git_oid *remote_oid_ptr = git_reference_target(remote_ref);
    if (!local_oid_ptr || !remote_oid_ptr) {
        log.log_to_file("Error: Failed to get OIDs for local or remote branch");
        git_reference_free(head_ref);
        git_reference_free(remote_ref);
        git_libgit2_shutdown();
        return -1;
    }
    git_oid_cpy(&local_oid, local_oid_ptr);
    git_oid_cpy(&remote_oid, remote_oid_ptr);

    git_reference_free(head_ref);
    git_reference_free(remote_ref);

    // Compare OIDs
    if (git_oid_cmp(&local_oid, &remote_oid) != 0) {
        log.log_to_file("Unpushed commits detected");
        git_libgit2_shutdown();
        return 0;
    }

    log.log_to_file("No unpushed commits detected");
    git_libgit2_shutdown();
    return 1;
}

int GitOperations::fetch_remote(const std::string config_path, git_repository *repo) {
    git_libgit2_init();
    Logging log;
    git_remote *remote = nullptr;
    int error = git_remote_lookup(&remote, repo, "origin");

    if (error != 0) {
        const git_error *last_error = giterr_last();
        log.log_to_file("Error: Failed to lookup remote - " + std::string(last_error->message));
        git_libgit2_shutdown();
        return -1;
    }

    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    fetch_opts.callbacks.credentials = cred_acquire_cb;
    fetch_opts.callbacks.payload = (void*)&config_path;
    
    error = git_remote_fetch(remote, nullptr, &fetch_opts, "Fetch completed");

    if (error != 0) {
        const git_error *last_error = giterr_last();
        log.log_to_file("Error: Failed to fetch from remote - " + std::string(last_error->message));
        git_remote_free(remote);
        git_libgit2_shutdown();
        return -1;
    }

    log.log_to_file("Fetched from remote successfully");
    git_remote_free(remote);
    git_libgit2_shutdown();
    return 0;
}

int GitOperations::backup_user_notes(const std::string repo_dir, const std::string backup_dir) {
    FsManipulator fs;
    Logging log;

    // Check if repo_dir and backup_dir exist
    if (fs.is_dir_present(repo_dir) != 0  && fs.is_dir_present(backup_dir) != 0) {
        log.log_to_file("Error: Repository directory or Backup directory not found.");
        return -1;
    }

    // Create backup directory if it doesn't exist
    if (fs.is_dir_present(backup_dir) != 0 && fs.create_dir(backup_dir) != 0) {
        log.log_to_file("Error: Failed to create backup directory.");
        return -1;
    }

    // Backup the contents
    try {
        // Iterate through the files and directories in repo_dir
        for (const auto& entry : fs::recursive_directory_iterator(repo_dir)) {
            const auto& path = entry.path();
            auto relative_path = fs::relative(path, repo_dir);
            if (relative_path == ".git" || relative_path.string().rfind(".git/", 0) == 0) {
                // Skip the .git directory and its contents
                continue;
            }

            fs::path target_path = backup_dir / relative_path;
            if (entry.is_directory()) {
                fs::create_directory(target_path);
            } else if (entry.is_regular_file()) {
                fs::copy(path, target_path, fs::copy_options::overwrite_existing);
            }
        }
    } catch (const std::exception& e) {
        log.log_to_file("Error: Failed to backup user notes. Exception: " + std::string(e.what()));
        return -1;
    }

    log.log_to_file("User notes backed up successfully.");
    return 0;
}

int GitOperations::recover_user_notes(const std::string repo_dir, const std::string backup_dir) {
    FsManipulator fs;
    Logging log;

    // Check if backup_dir exists
    if (fs.is_dir_present(backup_dir) != 0) {
        log.log_to_file("Error: Backup directory not found for recovery.");
        return -1;
    }

    try {
        // Clear repo_dir except for the .git directory
        for (const auto& entry : fs::directory_iterator(repo_dir)) {
            if (entry.path().filename() == ".git") {
                // Skip the .git directory
                continue;
            }

            if (entry.is_directory()) {
                fs::remove_all(entry.path());
            } else {
                fs::remove(entry.path());
            }
        }

        // Recover the contents from backup_dir
        for (const auto& entry : fs::recursive_directory_iterator(backup_dir)) {
            fs::path target_path = repo_dir / fs::relative(entry.path(), backup_dir);
            if (entry.is_directory()) {
                fs::create_directory(target_path);
            } else if (entry.is_regular_file()) {
                fs::copy(entry.path(), target_path, fs::copy_options::overwrite_existing);
            }
        }
    } catch (const std::exception& e) {
        log.log_to_file("Error: Failed to recover user notes. Exception: " + std::string(e.what()));
        return -1;
    }

    // Delete backup directory
    if (fs.delete_dir(backup_dir) != 0) {
        log.log_to_file("Warning: Failed to delete backup directory after recovery.");
        // Not returning -1 here as the primary operation of recovery is successful
    }

    log.log_to_file("User notes recovered successfully.");
    return 0;
}

int GitOperations::handle_non_fast_forward_error(const std::string config_path) {
    git_libgit2_init();
    Logging log;
    FsManipulator fs;

    std::string repo_dir = fs.read_config_var(config_path, "WORKSPACE_DIR") + "/" + fs.read_config_var(config_path, "MAIN_REPOSITORY");
    std::string backup_dir = fs.read_config_var(config_path, "BACKUP_DIR");

    // Backup everything except .git dir in the repo_dir to backup_dir
    if(backup_user_notes(repo_dir, backup_dir) != 0) {
        log.log_to_file("Error: Failed to backup user notes before dealing with non-fast-forward error.");
    }

    // Clear the contents of the local repository directory
    try {
        for (const auto& entry : fs::directory_iterator(repo_dir)) {
            if (entry.is_directory()) {
                fs::remove_all(entry.path());
            } else {
                fs::remove(entry.path());
            }
        }
    } catch (const std::exception& e) {
        log.log_to_file("Error: Failed to clear the local repository directory. Exception: " + std::string(e.what()));
        git_libgit2_shutdown();
        return -1;
    }

    // Clone the repository again
    if (clone_repo(config_path) != 0) {
        log.log_to_file("Error: Failed to clone the repository after starting the handling of non-fast-forward error.");
        git_libgit2_shutdown();
        return -1;
    }

    // Replace the contents of repo_dir except .git dir with the contents of backup_dir and delete backup_dir if success.
    if(recover_user_notes(repo_dir, backup_dir) != 0) {
        log.log_to_file("Error: Failed to Recover user notes After dealing with non-fast-forward error.");
    }

    git_libgit2_shutdown();
    return 0;
}

int GitOperations::push_to_remote(const std::string config_path, git_repository *repo) {
    git_libgit2_init();
    Logging log;
    git_remote *remote = nullptr;
    int error = git_remote_lookup(&remote, repo, "origin");

    if (error != 0) {
        const git_error *last_error = giterr_last();
        log.log_to_file("Error: Failed to lookup remote - " + std::string(last_error->message));
        git_libgit2_shutdown();
        return -1;
    }
    git_push_options push_opts = GIT_PUSH_OPTIONS_INIT;
    push_opts.callbacks.credentials = cred_acquire_cb;
    push_opts.callbacks.payload = (void*)&config_path;

    git_strarray refspecs = {NULL, 0};
    refspecs.strings = (char **)malloc(sizeof(char*));
    refspecs.strings[0] = const_cast<char*>("refs/heads/main:refs/heads/main");
    refspecs.count = 1;
    error = git_remote_push(remote, &refspecs, &push_opts);
    if (error != 0) {
        const git_error *last_error = giterr_last();
        log.log_to_file("Error: Failed to push to remote - '" + std::string(last_error->message) + "', handling it...");
        // if(auto_fetch_and_merge_with_conflict_markers(config_path) != 0) {
        //     log.log_to_file("Error: Segementation fault tracking, line 575");
        //     log.log_to_file("Error: Failed to Auto fetch and merge the remote to local.");
        //     git_libgit2_shutdown();
        //     return -1;
        // }
        if(handle_non_fast_forward_error(config_path) == 0) {
            log.log_to_file("Successfully maneuvered the non-fast-forward error above.");
            return 3;
        }
        git_remote_free(remote);
        git_libgit2_shutdown();
        return -1;
    }
    git_remote_free(remote);
    log.log_to_file("Pushed to remote successfully");
    git_libgit2_shutdown();
    return 0;
}