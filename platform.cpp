#include <iostream>

int main() {
    #ifdef _WIN32
        std::cout << "Running on Windows" << std::endl;
    #elif __APPLE__
        std::cout << "Running on Apple" << std::endl;
    #elif __linux__
        #ifdef __ANDROID__
            std::cout << "Running on Android" << std::endl;
        #else
            std::cout << "Running on Real Linux" << std::endl;
        #endif
    #else
        std::cout << "Unknown platform" << std::endl;
    #endif
    return 0;
}
