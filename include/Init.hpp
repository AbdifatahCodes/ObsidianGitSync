#ifndef INIT_HPP
#define INIT_HPP

#include <string>

class Init {
public:
    // root_dir in linux is "~/.config/ObsidianGitSync".
    std::string init_for_linux_desktop(const std::string root_dir, const std::string workspace_dir);
};

#endif // INIT_HPP