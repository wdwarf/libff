#include "minizip/unzip.h"

#include <ff/Exception.h>
#include <ff/File.h>
#include <ff/Unzip.h>

#include <cstring>
#include <fstream>

using namespace std;

NS_FF_BEG

Unzip::Unzip(const std::string& filePath) : m_filePath(filePath) {}

Unzip::~Unzip() {}

const std::string& Unzip::getFilePath() const { return this->m_filePath; }

std::vector<ZipFileInfo> Unzip::list() const {
  std::vector<ZipFileInfo> files;

  unzFile uf = nullptr;

  try {
    uf = unzOpen64(this->m_filePath.c_str());
    if (nullptr == uf) {
      uf = unzOpen64((this->m_filePath + ".zip").c_str());
    }

    if (nullptr == uf) THROW_EXCEPTION(Exception, "Open zip file failed.", -1);

    unz_global_info64 gi;
    int err = 0;

    err = unzGetGlobalInfo64(uf, &gi);
    if (UNZ_OK != err)
      THROW_EXCEPTION(Exception, "Get zip file info failed.", -1);

    for (uint64_t i = 0; i < gi.number_entry; i++) {
      ZipFileInfo info;
      char filename_inzip[256];
      unz_file_info64 file_info;
      uLong ratio = 0;
      const char* string_method;
      char charCrypt = ' ';

      err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
                                    sizeof(filename_inzip), NULL, 0, NULL, 0);
      info.name = filename_inzip;

      if (UNZ_OK != err)
        THROW_EXCEPTION(Exception, "Get zip file info failed.", -1);

      if (file_info.uncompressed_size > 0)
        ratio = (uLong)((file_info.compressed_size * 100) /
                        file_info.uncompressed_size);

      info.ratio = ratio;

      /* display a '*' if the file is crypted */
      if ((file_info.flag & 1) != 0) charCrypt = '*';

      if (file_info.compression_method == 0)
        string_method = "Stored";
      else if (file_info.compression_method == Z_DEFLATED) {
        uInt iLevel = (uInt)((file_info.flag & 0x6) / 2);
        if (iLevel == 0)
          string_method = "Defl:N";
        else if (iLevel == 1)
          string_method = "Defl:X";
        else if ((iLevel == 2) || (iLevel == 3))
          string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
      } else if (file_info.compression_method == Z_BZIP2ED) {
        string_method = "BZip2";
      } else
        string_method = "Unkn.";

      info.method = string_method;
      info.uncompressedSize = file_info.uncompressed_size;
      info.compressedSize = file_info.compressed_size;
      info.crc = file_info.crc;
      info.date.sec = file_info.tmu_date.tm_sec;
      info.date.min = file_info.tmu_date.tm_min;
      info.date.hour = file_info.tmu_date.tm_hour;
      info.date.mday = file_info.tmu_date.tm_mday;
      info.date.mon = file_info.tmu_date.tm_mon;
      info.date.year = file_info.tmu_date.tm_year;

      files.push_back(info);
      if ((i + 1) < gi.number_entry) {
        err = unzGoToNextFile(uf);

        if (UNZ_OK != err)
          THROW_EXCEPTION(Exception, "Get zip file info failed.", -1);
      }
    }
  } catch (const std::exception& e) {
    cout << e.what() << endl;
    if (nullptr != uf) unzClose(uf);
    throw e;
  }

  if (nullptr != uf) unzClose(uf);
  return files;
}

bool Unzip::list(std::vector<ZipFileInfo>& v) const {
  try {
    v = this->list();
    return true;
  } catch (...) {
  }
  return false;
}

static void change_file_date(const char* filename, uLong dosdate,
                             tm_unz tmu_date) {
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

  hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, 0, NULL);
  GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
  LocalFileTimeToFileTime(&ftLocal, &ftm);
  SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
  CloseHandle(hFile);
#else
#if defined(unix) || defined(__APPLE__)
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min = tmu_date.tm_min;
  newdate.tm_hour = tmu_date.tm_hour;
  newdate.tm_mday = tmu_date.tm_mday;
  newdate.tm_mon = tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
    newdate.tm_year = tmu_date.tm_year - 1900;
  else
    newdate.tm_year = tmu_date.tm_year;
  newdate.tm_isdst = -1;

  ut.actime = ut.modtime = mktime(&newdate);
  utime(filename, &ut);
#endif
#endif
}

static int DoExtractFile(unzFile uf, bool overwrite = false,
                         const std::string& dir = "",
                         const string& password = "") {
  bool popt_extract_without_path = false;
  char filename_inzip[256];
  char* filename_withoutpath;
  char* p;
  int err = UNZ_OK;
  void* buf;
  uInt size_buf;

  File(dir).mkdirs();

  unz_file_info64 file_info;
  uLong ratio = 0;
  err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
                                sizeof(filename_inzip), NULL, 0, NULL, 0);

  if (err != UNZ_OK) {
    printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
    return err;
  }

  size_buf = 2048;
  buf = (void*)malloc(size_buf);
  if (buf == NULL) {
    printf("Error allocating memory\n");
    return UNZ_INTERNALERROR;
  }

  p = filename_withoutpath = filename_inzip;
  while ((*p) != '\0') {
    if (((*p) == '/') || ((*p) == '\\')) filename_withoutpath = p + 1;
    p++;
  }

  if ((*filename_withoutpath) == '\0') {
    if (!popt_extract_without_path) {
      printf("creating directory: %s\n", filename_inzip);
      File(filename_inzip).mkdirs();
    }
  } else {
    const char* write_filename;
    int skip = 0;

    if (!popt_extract_without_path)
      write_filename = filename_inzip;
    else
      write_filename = filename_withoutpath;

    err = unzOpenCurrentFilePassword(
        uf, (password.empty() ? nullptr : password.c_str()));
    if (err != UNZ_OK) {
      printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
    }

    if (!overwrite && (err == UNZ_OK)) {
      if (File(dir, write_filename).isExists()) {
        skip = 1;
      }
    }

    fstream fout;
    if ((skip == 0) && (err == UNZ_OK)) {
      fout.open(File(dir, write_filename), ios::out | ios::trunc | ios::binary);
      /* some zipfile don't contain directory alone before file */
      if ((!fout.is_open()) && (!popt_extract_without_path) &&
          (filename_withoutpath != (char*)filename_inzip)) {
        char c = *(filename_withoutpath - 1);
        *(filename_withoutpath - 1) = '\0';
        File(dir, write_filename).mkdirs();
        *(filename_withoutpath - 1) = c;
        fout.open(File(dir, write_filename),
                  ios::out | ios::trunc | ios::binary);
      }

      if (!fout.is_open()) {
        printf("error opening %s\n", write_filename);
      }
    }

    if (fout.is_open()) {
      // printf(" extracting: %s\n", write_filename);

      do {
        err = unzReadCurrentFile(uf, buf, size_buf);
        if (err < 0) {
          printf("error %d with zipfile in unzReadCurrentFile\n", err);
          break;
        }
        if (err > 0) {
          fout.write(static_cast<const char*>(buf), err);
          if (fout.tellp() <= 0) {
            printf("error in writing extracted file\n");
            err = UNZ_ERRNO;
            break;
          }
        }

      } while (err > 0);
      fout.close();

      if (err == 0)
        change_file_date(write_filename, file_info.dosDate, file_info.tmu_date);
    }

    if (err == UNZ_OK) {
      err = unzCloseCurrentFile(uf);
      if (err != UNZ_OK) {
        printf("error %d with zipfile in unzCloseCurrentFile\n", err);
      }
    } else
      unzCloseCurrentFile(uf); /* don't lose the error */
  }

  free(buf);
  return err;
}

bool Unzip::unzipTo(const std::string& dir, bool overwrite,
                    const std::string& password) {
  unzFile uf = nullptr;

  try {
    uf = unzOpen64(this->m_filePath.c_str());
    if (nullptr == uf) {
      uf = unzOpen64((this->m_filePath + ".zip").c_str());
    }

    if (nullptr == uf) THROW_EXCEPTION(Exception, "Open zip file failed.", -1);

    unz_global_info64 gi;
    int err = 0;

    err = unzGetGlobalInfo64(uf, &gi);
    if (UNZ_OK != err)
      THROW_EXCEPTION(Exception, "Get zip file info failed.", -1);

    for (int i = 0; i < gi.number_entry; i++) {
      if (DoExtractFile(uf, overwrite, dir, password.c_str()) != UNZ_OK) break;

      if ((i + 1) < gi.number_entry) {
        err = unzGoToNextFile(uf);
        if (err != UNZ_OK) {
          printf("error %d with zipfile in unzGoToNextFile\n", err);
          break;
        }
      }
    }

  } catch (const std::exception& e) {
    cout << e.what() << endl;
    if (nullptr != uf) unzClose(uf);
    return false;
  }

  if (nullptr != uf) unzClose(uf);
  return true;
}

bool Unzip::unzipTo(const std::vector<std::string>& files,
                    const std::string& dir, bool overwrite,
                    const std::string& password) {
  unzFile uf = nullptr;

  try {
    uf = unzOpen64(this->m_filePath.c_str());
    if (nullptr == uf) {
      uf = unzOpen64((this->m_filePath + ".zip").c_str());
    }

    if (nullptr == uf) THROW_EXCEPTION(Exception, "Open zip file failed.", -1);

    for (auto& file : files) {
      int err = UNZ_OK;
      if (unzLocateFile(uf, File(file).getPath().c_str(), 0) != UNZ_OK) {
        THROW_EXCEPTION(Exception, "File[" + file + "] not found.", -1);
        continue;
      }

      DoExtractFile(uf, overwrite, dir, password);
    }

  } catch (const std::exception& e) {
    cout << e.what() << endl;
    if (nullptr != uf) unzClose(uf);
    return false;
  }

  if (nullptr != uf) unzClose(uf);
  return true;
}

NS_FF_END
