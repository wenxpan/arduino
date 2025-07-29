#ifndef SD_UTILS_H
#define SD_UTILS_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/**
 * @brief test whether sd card is working
 */
void sd_setup();

/**
 * `list_dir(SD, "/", 2);`
 */
void list_dir(fs::FS &fs, const char *dirname, uint8_t levels);

/**
 * `create_dir(SD, "/mydir");`
 */
void create_dir(fs::FS &fs, const char *path);

/**
 * `remove_dir(SD, "/mydir");`
 */
void remove_dir(fs::FS &fs, const char *path);

/**
 * `write_file(SD, "/hello.txt", "Hello ");`
 */
void write_file(fs::FS &fs, const char *path, const char *message);

/**
 * `append_file(SD, "/hello.txt", "World!");`
 */
void append_file(fs::FS &fs, const char *path, const char *message);

/**
 * `read_file(SD, "/hello.txt");`
 */
void read_file(fs::FS &fs, const char *path);

/**
 * `delete_file(SD, "/foo.txt");`
 */
void delete_file(fs::FS &fs, const char *path);

/**
 * `test_file_io(SD, "/test.txt");`
 */
void test_file_io(fs::FS &fs, const char *path);

/**
 * @brief Log sd card type, size and paths
 */
void log_sd_info();

/**
 * @brief test adding/removing/listing files for sd card
 */
void test_sd();

#endif