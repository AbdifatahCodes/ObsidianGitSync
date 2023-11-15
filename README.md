# ObsidianGitSync.
A tool to help you sync your Obsidian notes to a git host like GitHub. No need to pay for sync from now on.

## Prerequisites.
First, install the https://obsidian.md/Download Desktop App for Linux.

## Installation.
To install this tool on most Linux Distros, you can use my installation script `installation.sh`. Use this command to download the file and run it: 

```
sudo curl -L -o installation.sh "$binary_url"
```

This will install all the dependencies and the `ObsidianGitSync` binary and initialize all the required files for it to work.
It will also add a couple of default Variables to `~/.config/ObsidianGitSync/config.txt` and generate a pair of ssh keys at `~/.config/ObsidianGitSync/.ssh/`.

## Setup.
#### 1 - Add the New SSH Key to your GitHub.
Copy the `ObsidianGitSync.pub` contents and go to https://github.com/settings/ssh/new to create a new SSH Key for the Tool to use. Set `ObsidianGitSync` as the title.

#### 2 - Create A Blank Repository on GitHub for your Obsidian Vault.
Go to GitHub and create a new blank repository. Copy the repository name and change the Variable MAIN_REPOSITORY in `~/.config/ObsidianGitSync/config.txt` to the copied repository name.

#### 3 - Personalize config file.
Open `~/.config/ObsidianGitSync/config.txt` and change the Variable GIT_USERNAME to your git username. Also, change any other Variables as necessary such as GIT_HOST to gitlab.com if you are GitLab and WORKSPACE_DIR to your Obsidian notes root but you can leave it as `/Documents/Documents/Obsidian` and that will be where your repository will be cloned.
