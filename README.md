# ObsidianGitSync
A tool to help you sync your Obsidian notes to a git host like GitHub. This tool eliminates the need for paid synchronization services.

## Prerequisites
Before installing ObsidianGitSync, ensure that the [Obsidian Desktop App](https://obsidian.md/Download) for Linux is installed on your system.

## Installation
To install ObsidianGitSync on most Linux distributions, use the provided installation script. Run the following commands to download and execute the script:

```bash
sudo curl -L -o installation.sh "https://github.com/AbdifatahCodes/ObsidianGitSync/releases/download/installation/installation.sh"
sudo bash installation.sh
```

This script performs several actions:

- Installs all necessary dependencies.
- Downloads and installs the ObsidianGitSync binary.
- Initializes required files and directories for operation.
- Adds default configuration variables to ~/.config/ObsidianGitSync/config.txt.
- Generates an SSH key pair in ~/.config/ObsidianGitSync/.ssh/.

## Setup.
Follow these steps to configure ObsidianGitSync for use:

### 1 - Add the New SSH Key to your GitHub Account.
- Locate the public SSH key at `~/.config/ObsidianGitSync/.ssh/ObsidianGitSync.pub`.
- Copy its contents.
- Go to Add a [New SSH Key](https://github.com/settings/ssh/new) on GitHub.
- Paste the copied key and set "ObsidianGitSync" as the title.

### 2 - Create a Blank Repository on GitHub for Your Obsidian Vault.
- Visit [GitHub](https://github.com) and create a new blank repository.
- Copy the name of this new repository.
- Modify the `MAIN_REPOSITORY` variable in `~/.config/ObsidianGitSync/config.txt` to match the copied repository name.

### 3 - Personalize the Configuration File.
- Open `~/.config/ObsidianGitSync/config.txt` in a text editor.
- Change the `GIT_USERNAME` variable to your Git username.
- Adjust other variables as needed, e.g., change `GIT_HOST` to gitlab.com if you use GitLab.
- Leave `WORKSPACE_DIR` as `~/Documents/Documents/Obsidian`. This directory is where your repository will be cloned. Changing this path may cause the tool to malfunction.

### 4 - Finish installation.
Complete the installation process by running ObsidianGitSync as a background service:
```
sudo systemctl enable obsidian-git-sync.service
sudo systemctl start obsidian-git-sync.service
```

To stop the the service: 
```
sudo systemctl stop obsidian-git-sync.service
```

## Issues.
If you encounter any issues after following the instructions:

- Open an issue in the [ObsidianGitSync GitHub repository](https://github.com/AbdifatahCodes/ObsidianGitSync/issues).
- Include the contents of your `config.txt` and `logfile.txt` for troubleshooting assistance.

