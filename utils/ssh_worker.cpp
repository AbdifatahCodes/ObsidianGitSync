#include "SshWorker.hpp"
#include "FsManipulator.hpp"
#include "Logging.hpp"
#include <libssh/libssh.h>
#include <fstream>

void change_email_in_key(const std::string& pub_key_path, const std::string& email) {
    Logging logging;

    std::ifstream pub_key_file(pub_key_path);
    if (!pub_key_file) {
        logging.log_to_file("Error: Failed to open public key file for reading.");
        return;
    }

    std::string pub_key_content((std::istreambuf_iterator<char>(pub_key_file)), std::istreambuf_iterator<char>());
    size_t pos = pub_key_content.rfind(' ');
    if (pos != std::string::npos) {
        pub_key_content.replace(pos + 1, std::string::npos, email);
    } else {
        logging.log_to_file("Error: Space not found in public key file.");
        return;
    }

    std::ofstream pub_key_out(pub_key_path);
    if (!pub_key_out) {
        logging.log_to_file("Error: Failed to open public key file for writing.");
        return;
    }
    pub_key_out << pub_key_content;
    logging.log_to_file("Changed email in public key file to '" + email + "'.");
}

int SshWorker::is_ssh_setup(const std::string config_path) {
    Logging logging;
    FsManipulator fsManipulator;

    std::string ssh_key_name = fsManipulator.read_config_var(config_path, "SSH_KEY_NAME");
    std::string ssh_dir = fsManipulator.read_config_var(config_path, "SSH_DIR");
    std::string commits_email = fsManipulator.read_config_var(config_path, "COMMITS_EMAIL");

    std::string private_key_path = ssh_dir + "/" + ssh_key_name;
    std::string public_key_path = private_key_path + ".pub";

    if (fsManipulator.is_file_present(private_key_path) != 0 || fsManipulator.is_file_present(public_key_path) != 0) {
        fsManipulator.create_dir(ssh_dir);
        logging.log_to_file("Error: SSH key files do not exist.");
        return -1;
    }

    // Verifying the key type for both private and public keys
    ssh_key private_key, public_key;
    if (ssh_pki_import_privkey_file(private_key_path.c_str(), nullptr, nullptr, nullptr, &private_key) != SSH_OK ||
        ssh_pki_import_pubkey_file(public_key_path.c_str(), &public_key) != SSH_OK) {
        logging.log_to_file("Error: Failed to import SSH keys.");
        return -2;
    }

    int key_type = ssh_key_type(private_key);
    if (key_type != SSH_KEYTYPE_ED25519) {
        logging.log_to_file("Error: SSH key type is not ED25519.");
        ssh_key_free(private_key);
        ssh_key_free(public_key);
        return -3;
    }

    ssh_key_free(private_key);
    ssh_key_free(public_key);
    logging.log_to_file("SSH keys are set up and verified.");
    return 0;
}

std::string SshWorker::setup_new_ssh_keypair(const std::string config_path) {
    Logging logging;
    FsManipulator fsManipulator;

    std::string ssh_key_name = fsManipulator.read_config_var(config_path, "SSH_KEY_NAME");
    std::string ssh_dir = fsManipulator.read_config_var(config_path, "SSH_DIR");
    std::string commits_email = fsManipulator.read_config_var(config_path, "COMMITS_EMAIL");

    std::string private_key_path = ssh_dir + "/" + ssh_key_name;
    std::string public_key_path = private_key_path + ".pub";

    ssh_key key;
    if (ssh_pki_generate(SSH_KEYTYPE_ED25519, 0, &key) != SSH_OK ||
        ssh_pki_export_privkey_file(key, nullptr, nullptr, nullptr, private_key_path.c_str()) != SSH_OK ||
        ssh_pki_export_pubkey_file(key, public_key_path.c_str()) != SSH_OK) {
        logging.log_to_file("Error: Generating or exporting SSH key pair failed.");
        ssh_key_free(key);
        return "Error";
    }

    change_email_in_key(public_key_path, commits_email);
    ssh_key_free(key);
    logging.log_to_file("New SSH key pair has been set up.");
    return "Success!";
}
