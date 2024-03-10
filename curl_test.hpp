#ifndef CURL_TEST_HPP
#define CURL_TEST_HPP

#include <curl/curl.h>
#include <iostream>

void Post()
{
    CURL *curl;
    CURLcode res;
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        std::cout << "curl init success" << std::endl;
    }
    
}

#endif