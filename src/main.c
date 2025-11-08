#include <stdio.h>
#include <string.h>
#include "download.h"
#include "verify.h"
#include "extract.h"
#include "db.h"
#include "repo.h"


void show_progress(size_t current, size_t total) {
    int width = 50; // バーの幅
    float ratio = (float)current / total;
    int filled = (int)(ratio * width);

    printf("\r[");
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%%", (int)(ratio * 100));
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s install <pkgname> [--prefix=/path/to/install]\n", argv[0]);
        return 1;
    }
    
    for(int a = 2; a < argc; a++){
    
    const char *pkg = argv[a];
    const char *prefix = "/usr/local"; // デフォルト
    PackageInfo info;
    prefix=info.path;
    // オプション解析
    for (int i = 3; i < argc; i++) {
        if (strncmp(argv[i], "--prefix=", 9) == 0) {
            prefix = argv[i] + 9;
        }
    }

    
    if (repo_lookup(pkg, &info) != 0) {
        fprintf(stderr, "Package not found: %s\n", pkg);
        return 1;
    }

    printf("Downloading %s...\n", info.url);
    if (http_download(info.url, "pkg.tar.gz") != 0) {
        fprintf(stderr, "Download failed\n");
        return 1;
    }

    unsigned char hash[32];
    sha256_file("pkg.tar.gz", hash);
    if (!sha256_compare_hex(hash, info.sha256)) {
        fprintf(stderr, "SHA256 mismatch!\n");
        return 1;
    }

    printf("Extracting to %s...\n", prefix);
    if (extract_tar_gz("pkg.tar.gz", prefix) != 0) {
        fprintf(stderr, "Extraction failed\n");
        return 1;
    }

    db_add(pkg, info.version);
    printf("Installed %s %s into %s\n", pkg, info.version, prefix);
    }
    return 0;
}
