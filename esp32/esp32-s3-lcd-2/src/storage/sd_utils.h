#ifndef SD_UTILS_H
#define SD_UTILS_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/**
 * @brief test whether sd card is working
 */
void setupSD();

/**
 * `listDir(SD, "/", 2);`
 */
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

/**
 * `createDir(SD, "/mydir");`
 */
void createDir(fs::FS &fs, const char *path);

/**
 * `removeDir(SD, "/mydir");`
 */
void removeDir(fs::FS &fs, const char *path);

/**
 * `writeFile(SD, "/hello.txt", "Hello ");`
 */
void writeFile(fs::FS &fs, const char *path, const char *message);

/**
 * `appendFile(SD, "/hello.txt", "World!");`
 */
void appendFile(fs::FS &fs, const char *path, const char *message);

/**
 * `readFile(SD, "/hello.txt");`
 */
void readFile(fs::FS &fs, const char *path);

/**
 * `deleteFile(SD, "/foo.txt");`
 */
void deleteFile(fs::FS &fs, const char *path);

/**
 * `testFileIo(SD, "/test.txt");`
 */
void testFileIo(fs::FS &fs, const char *path);

/**
 * @brief Log sd card type, size and paths
 */
void logSDInfo();

/**
 * @brief test adding/removing/listing files for sd card
 */
void testSD();

#endif