
#include "FS.h"
#include "SD.h"
#include "SPI.h"

int sck = 39;
int miso = 40;
int mosi = 38;
int cs = 41;

uint64_t cardSize;

void sd_setup()
{
  USBSerial.println("SDSC");

  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs))
  {
    USBSerial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    USBSerial.println("No SD card attached");
    return;
  }

  USBSerial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    USBSerial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    USBSerial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    USBSerial.println("SDHC");
  }
  else
  {
    USBSerial.println("UNKNOWN");
  }
}

void list_dir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  USBSerial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    USBSerial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    USBSerial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      USBSerial.print("  DIR : ");
      USBSerial.println(file.name());
      if (levels)
      {
        list_dir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      USBSerial.print("  FILE: ");
      USBSerial.print(file.name());
      USBSerial.print("  SIZE: ");
      USBSerial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void create_dir(fs::FS &fs, const char *path)
{
  USBSerial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    USBSerial.println("Dir created");
  }
  else
  {
    USBSerial.println("mkdir failed");
  }
}

void remove_dir(fs::FS &fs, const char *path)
{
  USBSerial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    USBSerial.println("Dir removed");
  }
  else
  {
    USBSerial.println("rmdir failed");
  }
}

void read_file(fs::FS &fs, const char *path)
{
  USBSerial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    USBSerial.println("Failed to open file for reading");
    return;
  }

  USBSerial.print("Read from file: ");
  while (file.available())
  {
    USBSerial.write(file.read());
  }
  file.close();
}

void write_file(fs::FS &fs, const char *path, const char *message)
{
  USBSerial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    USBSerial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    USBSerial.println("File written");
  }
  else
  {
    USBSerial.println("Write failed");
  }
  file.close();
}

void append_file(fs::FS &fs, const char *path, const char *message)
{
  USBSerial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    USBSerial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    USBSerial.println("Message appended");
  }
  else
  {
    USBSerial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  USBSerial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    USBSerial.println("File renamed");
  }
  else
  {
    USBSerial.println("Rename failed");
  }
}

void delete_file(fs::FS &fs, const char *path)
{
  USBSerial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    USBSerial.println("File deleted");
  }
  else
  {
    USBSerial.println("Delete failed");
  }
}

void test_file_io(fs::FS &fs, const char *path)
{
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    USBSerial.printf("%u bytes read for %lu ms\n", flen, end);
    file.close();
  }
  else
  {
    USBSerial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    USBSerial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  end = millis() - start;
  USBSerial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
  file.close();
}

void log_sd_info()
{
  cardSize = SD.cardSize() / (1024 * 1024);
  list_dir(SD, "/", 2);
  USBSerial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  USBSerial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  USBSerial.printf("SD Card Size: %lluMB\n", cardSize);
}

void test_sd()
{
  list_dir(SD, "/", 0);
  create_dir(SD, "/mydir");
  list_dir(SD, "/", 0);
  remove_dir(SD, "/mydir");
  list_dir(SD, "/", 2);

  write_file(SD, "/hello.txt", "Hello ");
  append_file(SD, "/hello.txt", "World!\n");
  read_file(SD, "/hello.txt");
  delete_file(SD, "/foo.txt");
  renameFile(SD, "/hello.txt", "/foo.txt");
  read_file(SD, "/foo.txt");
  test_file_io(SD, "/test.txt");

  log_sd_info();
}
