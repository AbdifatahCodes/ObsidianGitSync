# ObsidianGitSync
A tool to help you sync your Obsidian notes to a git host like github. No need to pay for sync from now on.

To install all the necessary libraries associated with the linking command -lgit2 -lssh2 -lconfig++ -lssh -lcurl on a Linux system, you need to install the development packages for each of these libraries. The exact command to install these packages can vary depending on the Linux distribution you are using. Here are the commands for some of the most common distributions:
Ubuntu/Debian-based systems:

bash

sudo apt-get update
sudo apt-get install libgit2-dev libssh2-1-dev libconfig++-dev libssh-dev libcurl4-openssl-dev

Red Hat/CentOS/Fedora systems:

For CentOS/RHEL, you might need to enable EPEL (Extra Packages for Enterprise Linux) repository to find all packages:

bash

sudo yum install epel-release
sudo yum install libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel

Or, if you are using dnf (such as on Fedora):

bash

sudo dnf install libgit2-devel libssh2-devel libconfig-devel libssh-devel libcurl-devel

Arch Linux:

bash

sudo pacman -Syu
sudo pacman -S libgit2 libssh2 libconfig libssh curl

Notes:

    Development Packages: It's important to install the development versions of these libraries (typically suffixed with -dev or -devel) as they include the header files required for compilation.

    Package Names: The exact names of the packages might slightly vary between distributions. If a package is not found, you can search your distribution's package repository or consider alternative packages that provide the same functionality.

    Dependencies: Some of these libraries might have their own dependencies, which should be automatically handled by the package manager.

After running these commands, the development libraries you need should be installed on your system, and you should be able to compile your program with the specified linking flags.
