#include "extract.h"
#include <archive.h>
#include <archive_entry.h>
#include <limits.h>
#include <stdio.h>

static void show_progress(int current, int total) {
    int width = 50;
    double ratio = (double)current / total;
    int filled = (int)(ratio * width);

    printf("\r[");
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%% (%d/%d)", (int)(ratio * 100), current, total);
    fflush(stdout);
}

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
    int total_files = 0, current_file = 0;

    // まずファイル数を数える
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        total_files++;
        archive_read_data_skip(a); // データをスキップ
    }
    archive_read_close(a);
    archive_read_free(a);

    // 再度開いて展開
    a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
    if (archive_read_open_filename(a, archive_path, 10240) != ARCHIVE_OK) return -1;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        current_file++;
        const char *current = archive_entry_pathname(entry);

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dest_dir, current);
        archive_entry_set_pathname(entry, fullpath);

        if (archive_write_header(ext, entry) != ARCHIVE_OK) goto cleanup;

        const void *buff; size_t size; la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) goto cleanup;
        }

        show_progress(current_file, total_files);
    }

    printf("\n"); // 完了後改行

cleanup:
    archive_write_close(ext);
    archive_write_free(ext);
    archive_read_close(a);
    archive_read_free(a);
    return 0;
}
