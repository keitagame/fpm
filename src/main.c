#include <stdio.h>
#include <string.h>
#include "download.h"
#include "verify.h"
#include "extract.h"
#include "db.h"
#include "repo.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s install <pkgname> [--prefix=/path/to/install]\n", argv[0]);
        return 1;
    }

    const char *pkg = argv[2];
    const char *prefix = "/usr/local"; // デフォルト

    // オプション解析
    for (int i = 3; i < argc; i++) {
        if (strncmp(argv[i], "--prefix=", 9) == 0) {
            prefix = argv[i] + 9;
        }
    }

    PackageInfo info;
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
    return 0;
}
