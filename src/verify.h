#ifndef VERIFY_H
#define VERIFY_H

// SHA256計算
int sha256_file(const char *path, unsigned char out[32]);

// 計算結果と16進文字列の比較
int sha256_compare_hex(unsigned char hash[32], const char *hex);

#endif
