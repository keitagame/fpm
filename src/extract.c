#include "extract.h"
#include <archive.h>
#include <archive_entry.h>
#include <limits.h>
int extract_tar_gz(const char *archive_path, const char *dest_dir) {
    struct archive *a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
    if (archive_read_open_filename(a, archive_path, 10240) != ARCHIVE_OK) return -1;

    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext,
        ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
        ARCHIVE_EXTRACT_ACL  | ARCHIVE_EXTRACT_FFLAGS);

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *current = archive_entry_pathname(entry);

        // dest_dir と結合
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dest_dir, current);
        archive_entry_set_pathname(entry, fullpath);

        if (archive_write_header(ext, entry) != ARCHIVE_OK) return -2;

        const void *buff; size_t size; la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) return -3;
        }
    }
    archive_write_close(ext);
    archive_write_free(ext);
    archive_read_close(a);
    archive_read_free(a);
    return 0;
}
