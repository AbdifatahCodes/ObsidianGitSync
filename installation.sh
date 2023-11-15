#!/bin/bash

# Function to install dependencies on Debian-based systems
install_on_debian() {
    echo "Detected a Debian-based Linux distribution."
    sudo apt-get update
    sudo apt-get install -y libgit2-dev libssh2-1-dev libconfig++-dev libssh-dev libcurl4-openssl-dev coreutils
}

# Function to install dependencies on Red Hat-based systems
install_on_redhat() {
    echo "Detected a Red Hat-based Linux distribution."
    sudo yum install -y epel-release
    sudo yum install -y libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel coreutils
}

# Function to install dependencies on Fedora
install_on_fedora() {
    echo "Detected Fedora."
    sudo dnf install -y libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel coreutils
}

# Function to install dependencies on Arch Linux
install_on_arch() {
    echo "Detected Arch Linux."
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm libgit2 libssh2 libconfig libssh curl coreutils
}

# Detect the distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    case $ID in
        ubuntu | debian | pop)
            install_on_debian
            ;;
        rhel | centos)
            install_on_redhat
            ;;
        fedora)
            install_on_fedora
            ;;
        arch | manjaro)
            install_on_arch
            ;;
        *)
            echo "Your distribution ($ID) is not recognized. Please manually install the dependencies."
            exit 1
            ;;
    esac
else
    echo "Cannot identify the Linux distribution. Please manually install the dependencies."
    exit 1
fi

# Create Required directories
create_dirs() {
    mkdir ~/.config/ObsidianGitSync ~/Documents/Documents/Obsidian
}

# Function to download the binary package
download_binary() {
    echo "Downloading the binary package from GitHub releases..."
    binary_url="https://github.com/AbdifatahCodes/ObsidianGitSync/releases/download/trial/ObsidianGitSync"
    curl -L -o ObsidianGitSync "$binary_url"

    echo "Making the binary executable..."
    chmod +x ObsidianGitSync

    sudo mv ObsidianGitSync /bin/usr

}

create_config_file() {
    config_dir="/home/$USER/.config/ObsidianGitSync"
    config_file="$config_dir/config.txt"
    
    # Create directory if it doesn't exist
    mkdir -p "$config_dir"

    echo "Creating configuration file..."
    echo "GIT_HOST = \"github.com\";" > "$config_file"
    echo "GIT_USERNAME = \"username\";" >> "$config_file"
    echo "MAIN_REPOSITORY = \"ObsidianGitVault\";" >> "$config_file"
    echo "SSH_KEY_NAME = \"ObsidianGitSync\";" >> "$config_file"
    echo "ROOT_DIR = \"$config_dir\";" >> "$config_file"
}

create_systemd_service() {
    service_file="/etc/systemd/system/obsidian-git-sync.service"

    echo "Creating systemd service file..."
    echo "[Unit]" > "$service_file"
    echo "Description=ObsidianGitSync Service." >> "$service_file"
    echo "After=network.target" >> "$service_file"
    echo "" >> "$service_file"
    echo "[Service]" >> "$service_file"
    echo "ExecStart=ObsidianGitSync" >> "$service_file"
    echo "Restart=on-failure" >> "$service_file"
    echo "User=$USER" >> "$service_file"
    echo "" >> "$service_file"
    echo "[Install]" >> "$service_file"
    echo "WantedBy=multi-user.target" >> "$service_file"
    
}

generate_config_and_ssh() {
    timeout 60s ObsidianGitSync
}

# Check if the ObsidianGitSync does not exist
if [ ! -f "$file_path" ]; then
    echo "ObsidianGitSync does not exist. Downloading..."
    # Download binary github releases
    download_binary
else
    echo "ObsidianGitSync exists."
fi

create_dirs
create_config_file
create_systemd_service
generate_config_and_ssh
