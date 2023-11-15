#include "NetworkChecker.hpp"
#include "Logging.hpp"
#include <curl/curl.h>

int NetworkChecker::checkConnectivity() {
    CURL *curl;
    CURLcode res;
    Logging logging;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request, no body
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            logging.log_to_file("Network connection is Up, Continuing...");
            return 0;
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        logging.log_to_file("Error: Network connection is Down, Aborting for now.");
        return -1;
    }
    curl_global_cleanup();
    logging.log_to_file("Error: Failed to initialize curl.");
    return -1;  // Return -1 if curl initialization failed
}
