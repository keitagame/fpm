#include "repo.h"
#include "cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int repo_lookup(const char *name, PackageInfo *out) {
    FILE *fp = fopen("/etc/fpm/index.json", "r");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    char *buf = malloc(len+1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(buf);
    free(buf);
    if (!root) return -1;

    cJSON *pkgs = cJSON_GetObjectItem(root, "packages");
    cJSON *pkg;
    cJSON_ArrayForEach(pkg, pkgs) {
        const char *pname = cJSON_GetObjectItem(pkg, "name")->valuestring;
        if (strcmp(pname, name) == 0) {
            strcpy(out->name, pname);
            strcpy(out->version, cJSON_GetObjectItem(pkg, "version")->valuestring);
            strcpy(out->url, cJSON_GetObjectItem(pkg, "url")->valuestring);
            strcpy(out->sha256, cJSON_GetObjectItem(pkg, "sha256")->valuestring);
            cJSON_Delete(root);
            return 0;
        }
    }
    cJSON_Delete(root);
    return -1;
}
