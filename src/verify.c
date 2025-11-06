#include "verify.h"
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

int sha256_file(const char *path, unsigned char out[32]) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;
    SHA256_CTX ctx; SHA256_Init(&ctx);
    unsigned char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        SHA256_Update(&ctx, buf, n);
    }
    fclose(fp);
    SHA256_Final(out, &ctx);
    return 0;
}

int sha256_compare_hex(unsigned char hash[32], const char *hex) {
    char buf[65];
    for (int i=0; i<32; i++) sprintf(buf+2*i, "%02x", hash[i]);
    buf[64] = '\0';
    return strcmp(buf, hex) == 0;
}
