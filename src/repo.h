#ifndef REPO_H
#define REPO_H

// パッケージ情報構造体
typedef struct {
    char name[64];
    char version[32];
    char url[256];
    char sha256[65];
    char path[256];
} PackageInfo;

// index.jsonからパッケージ情報を検索
int repo_lookup(const char *name, PackageInfo *out);

#endif
