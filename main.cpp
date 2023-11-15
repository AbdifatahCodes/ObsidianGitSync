#include "Init.hpp"
#include <iostream>
#include <string>
#include <cstdlib>  // For getenv

int main() {
    Init init;
    std::string home_dir = getenv("HOME");
    std::string root_dir = home_dir + "/.config/ObsidianGitSync";
    std::string workspace_dir = home_dir + "/Documents/Documents/Obsidian";
    
    init.init_for_linux_desktop(root_dir, workspace_dir);
    return 0;
}
