#include "download.h"
#include <curl/curl.h>
#include <stdio.h>

static int progress_cb(void *clientp,
                       curl_off_t dltotal, curl_off_t dlnow,
                       curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal <= 0) return 0; // 総サイズが不明な場合はスキップ

    int width = 50;
    double ratio = (double)dlnow / (double)dltotal;
    int filled = (int)(ratio * width);

    printf("\r[");
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%%", (int)(ratio * 100));
    fflush(stdout);

    return 0; // 0を返すと継続、非0で中断
}

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

    // 進捗バーを有効化する設定は perform 前に行う
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_cb);

    CURLcode res = curl_easy_perform(curl);

    fclose(fp);
    curl_easy_cleanup(curl);

    printf("\n"); // ダウンロード完了後に改行

    return res == CURLE_OK ? 0 : -3;
}
