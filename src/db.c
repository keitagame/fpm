#include "db.h"
#include <stdio.h>

int db_add(const char *name, const char *version) {
    FILE *fp = fopen("/var/lib/mypm/installed.txt", "a");
    if (!fp) return -1;
    fprintf(fp, "%s %s\n", name, version);
    fclose(fp);
    return 0;
}
