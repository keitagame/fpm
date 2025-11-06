#include "extract.h"
#include <archive.h>
#include <archive_entry.h>

int extract_tar_gz(const char *archive_path, const char *dest_dir) {
    struct archive *a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
    if (archive_read_open_filename(a, archive_path, 10240) != ARCHIVE_OK) return -1;

    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM);

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        archive_write_header(ext, entry);
        const void *buff; size_t size; la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            archive_write_data_block(ext, buff, size, offset);
        }
    }
    archive_write_close(ext); archive_write_free(ext);
    archive_read_close(a); archive_read_free(a);
    return 0;
}
