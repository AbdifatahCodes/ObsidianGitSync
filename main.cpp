#include "Init.hpp"
#include "NetworkChecker.hpp"
#include <iostream>
#include <string>
#include <cstdlib>  // For getenv
#include <thread>
#include <chrono>

int main() {
    Init init;
    NetworkChecker networkChecker;
    std::string home_dir = getenv("HOME");
    std::string root_dir = home_dir + "/.config/ObsidianGitSync";
    std::string workspace_dir = home_dir + "/Documents/Documents/Obsidian";
    
    while(true) {
        if(networkChecker.checkConnectivity() == 0) {
            init.init_for_linux_desktop(root_dir, workspace_dir);
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(60));  // Sleep for 60 seconds
        }
    }
    return 0;
}
