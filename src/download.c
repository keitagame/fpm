#include "download.h"
#include <curl/curl.h>
#include <stdio.h>

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    FILE *fp = (FILE*)userdata;
    return fwrite(ptr, size, nmemb, fp);
}

int http_download(const char *url, const char *out_path) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    FILE *fp = fopen(out_path, "wb");
    if (!fp) { curl_easy_cleanup(curl); return -2; }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    CURLcode res = curl_easy_perform(curl);
    fclose(fp);
    curl_easy_cleanup(curl);
    return res == CURLE_OK ? 0 : -3;
}
