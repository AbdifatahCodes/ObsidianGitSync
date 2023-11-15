#ifndef SSHWORKER_HPP
#define SSHWORKER_HPP

#include <string>

class SshWorker {
public:
    // This class will use libssh library and retrieve vars like SSH_KEY_NAME, ROOT_DIR, SSH_DIR, COMMITS_EMAIL from config_path using the FsManipulator method read_config_var.
    // Use the Logging method log_to_file that takes in a string to log everything, from successes to errors with errors being prepended with a string 'Error: '.
    // Then use method from FsManipulator to check if the 2 ssh keys exist using the file path ssh_dir/ssh_key_name and ssh_dir/ssh_key_name.pub, if they exist, try to verify them that they are actually SSH_KEYTYPE_ED25519 and not just random file with the right names that we looked for, if they are verified ssh keys, return 0, else return a non-zero number and use logging class to log all the returns or log worthy events.
    // 
    int is_ssh_setup(const std::string config_path);
    // Create a new SSH_KEYTYPE_ED25519 keypair using the keynames ssh_key_name and add .pub to the public key, then create a private function that will be used by the inhabitants of ssh_worker.cpp only to change the email at the end of the pub key file to the var commits_email. Log everything including errors in the way specified above.
    std::string setup_new_ssh_keypair(const std::string config_path);
};

#endif // SSHWORKER_HPP