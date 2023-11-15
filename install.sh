#!/bin/bash

# Function to install dependencies on Debian-based systems
install_on_debian() {
    echo "Detected a Debian-based Linux distribution."
    sudo apt-get update
    sudo apt-get install -y libgit2-dev libssh2-1-dev libconfig++-dev libssh-dev libcurl4-openssl-dev
}

# Function to install dependencies on Red Hat-based systems
install_on_redhat() {
    echo "Detected a Red Hat-based Linux distribution."
    sudo yum install -y epel-release
    sudo yum install -y libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel
}

# Function to install dependencies on Fedora
install_on_fedora() {
    echo "Detected Fedora."
    sudo dnf install -y libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel
}

# Function to install dependencies on Arch Linux
install_on_arch() {
    echo "Detected Arch Linux."
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm libgit2 libssh2 libconfig libssh curl
}

# Detect the distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    case $ID in
        ubuntu | debian)
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

# Function to download and run the binary package
download_and_run() {
    echo "Downloading the binary package from GitHub releases..."
    # Replace with the direct link to your binary in GitHub releases
    binary_url="https://github.com/username/repository/releases/download/v1.0/your-binary"
    curl -L -o your-binary "$binary_url"

    echo "Making the binary executable..."
    chmod +x your-binary

    echo "Running the binary..."
    ./your-binary
}

# After installing dependencies, download and run the binary
download_and_run