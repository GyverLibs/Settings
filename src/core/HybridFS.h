#pragma once
#include <Arduino.h>
#include <FS.h>

#include <functional>
#include <type_traits>

#define FSW_MAX_SPACE 512000000000

// FSWrapper
class FSWrapper {
   public:
    FSWrapper() {}

    template <typename fs_t>
    FSWrapper(fs_t& fs) { setFS(fs); }

    // указатель на fs
    FS* fs() {
        return _fs;
    }

    // fs подключена
    bool valid() {
        return _fs;
    }
    operator bool() {
        return _fs;
    }

    // подлючить fs
    template <typename fs_t>
    void setFS(fs_t& fs) {
        _fs = &fs;

#ifdef ESP8266
        _totalSpace = [&]() -> uint64_t {
            fs::FSInfo64 info;
            fs.info64(info);
            return info.totalBytes;
        };
        _usedSpace = [&]() -> uint64_t {
            fs::FSInfo64 info;
            fs.info64(info);
            return info.usedBytes;
        };
#else  // ESP32
        _totalSpace = [&]() -> uint64_t {
            return fs.totalBytes();
        };
        _usedSpace = [&]() -> uint64_t {
            return fs.usedBytes();
        };
#endif
    }

    // отключить fs
    void reset() {
        _fs = nullptr;
        _totalSpace = nullptr;
        _usedSpace = nullptr;
    }

    // удалить файл
    bool remove(const char* path) {
        return _fs ? _fs->remove(path) : false;
    }

    // открыть файл
    File open(const char* path, const char* mode) {
        if (!_fs) return File();
#ifdef ESP8266
        return _fs->open(path, mode);
#else
        return _fs->open(path, mode, mode[0] != 'r');
#endif
    }

    // открыть для чтения
    File openRead(const char* path) {
        return open(path, "r");
    }

    // открыть для записи
    File openWrite(const char* path) {
        return open(path, "w");
    }

    String listDir(const char* path = "/") {
        String str;
        listDir(str, path);
        return str;
    }

    // вывести список файлов. Разделитель файлов - ';', через ':' указан размер в байтах
    void listDir(String& str, const char* path = "/", bool withSize = false, const char* prefix = "") {
        if (!_fs) return;
#ifdef ESP8266
        Dir dir = _fs->openDir(path);
        while (dir.next()) {
            if (dir.isDirectory()) {
                String p(path);
                p += dir.fileName();
                p += '/';
                Dir sdir = _fs->openDir(p);
                listDir(str, p.c_str(), withSize);
            }
            if (dir.isFile() && dir.fileName().length()) {
                str += prefix;
                str += path;
                str += dir.fileName();
                if (withSize) {
                    str += ':';
                    str += dir.fileSize();
                }
                str += ';';
            }
        }

#else  // ESP32
        File root = _fs->open(path);
        if (!root || !root.isDirectory()) return;
        File file;
        while (file = root.openNextFile()) {
            if (file.isDirectory()) {
                listDir(str, file.path(), withSize);
            } else {
                str += prefix;
                if (strlen(path) > 1) str += path;
                str += '/';
                str += file.name();
                if (withSize) {
                    str += ':';
                    str += file.size();
                }
                str += ';';
            }
        }
#endif
    }

    // создать директорию
    void mkdir(const char* path) {
        if (!_fs) return;
#ifdef ESP8266
        _fs->mkdir(path);
#else
        if (!_fs->exists(path)) {
            if (strchr(path, '/')) {
                char* pathD = strdup(path);
                if (pathD) {
                    char* ptr = strchr(pathD, '/');
                    while (ptr) {
                        *ptr = 0;
                        _fs->mkdir(pathD);
                        *ptr = '/';
                        ptr = strchr(ptr + 1, '/');
                    }
                }
                free(pathD);
            }
        }
#endif
    }

    // удалить директорию
    void rmdir(const char* path) {
        if (!_fs) return;
#ifdef ESP8266
        _fs->rmdir(path);
#else
        char* pathD = strdup(path);
        if (pathD) {
            char* ptr = strrchr(pathD, '/');
            while (ptr) {
                *ptr = 0;
                _fs->rmdir(pathD);
                ptr = strrchr(pathD, '/');
            }
            free(pathD);
        }
#endif
    }

    // всего памяти, байт
    uint64_t totalSpace() {
        if (!_totalSpace) return 0;
        uint64_t v = _totalSpace();
        return v < FSW_MAX_SPACE ? v : 0;
    }

    // свободно, байт
    uint64_t usedSpace() {
        if (!_usedSpace) return 0;
        uint64_t v = _usedSpace();
        return v < FSW_MAX_SPACE ? v : 0;
    }

    // занято, байт
    uint64_t freeSpace() {
        return totalSpace() - usedSpace();
    }

   private:
    FS* _fs = nullptr;
    std::function<uint64_t()> _totalSpace = nullptr;
    std::function<uint64_t()> _usedSpace = nullptr;
};

#define HFS_SD_PREFIX "/sd/"
#define HFS_SD_PREFIX_LEN (sizeof(HFS_SD_PREFIX) - 1)
#define HFS_SD_SHIFT (sizeof(HFS_SD_PREFIX) - 2)

// HybridFS
class HybridFS {
   public:
    FSWrapper flash, sd;

    HybridFS() {}

    // подлючить fs (flash)
    template <typename flash_t>
    HybridFS(flash_t& flash_fs) { setFS(flash_fs); }

    // подлючить fs (flash + sd)
    template <typename flash_t, typename sd_t>
    HybridFS(flash_t& flash_fs, sd_t& sd_fs) { setFS(flash_fs, sd_fs); }

    // подлючить fs (flash)
    template <typename flash_t>
    void setFS(flash_t& flash_fs) {
        flash.setFS(flash_fs);
        sd.reset();
    }

    // подлючить fs (flash + sd)
    template <typename flash_t, typename sd_t>
    void setFS(flash_t& flash_fs, sd_t& sd_fs) {
        flash.setFS(flash_fs);
        sd.setFS(sd_fs);
    }

    // вывести список файлов. Разделитель файлов - ';', через ':' указан размер в байтах
    void listDir(String& str, const char* path = "/", bool withSize = false) {
        flash.listDir(str, path, withSize);

        if (sd) {
            char* pref = strdup(HFS_SD_PREFIX);
            pref[HFS_SD_PREFIX_LEN - 1] = 0;
            sd.listDir(str, path, withSize, pref);
            free(pref);
        }
    }

    // вывести список файлов. Разделитель файлов - ';'
    String listDir(const char* path = "/") {
        String str;
        listDir(str, path);
        return str;
    }

    // удалить файл
    bool remove(const char* path) {
        return _isSD(path) ? sd.remove(path + HFS_SD_SHIFT) : flash.remove(path);
    }

    // открыть файл
    File open(const char* path, const char* mode) {
        return _isSD(path) ? sd.open(path + HFS_SD_SHIFT, mode) : flash.open(path, mode);
    }

    // открыть для чтения
    File openRead(const char* path) {
        return open(path, "r");
    }

    // открыть для записи
    File openWrite(const char* path) {
        return open(path, "w");
    }

    // создать директорию
    void mkdir(const char* path) {
        _isSD(path) ? sd.mkdir(path + HFS_SD_SHIFT) : flash.mkdir(path);
    }

    // удалить директорию
    void rmdir(const char* path) {
        _isSD(path) ? sd.rmdir(path + HFS_SD_SHIFT) : flash.rmdir(path);
    }

    // указатель на fs
    FS* fs(const char* path) {
        return _isSD(path) ? sd.fs() : flash.fs();
    }

   private:
    inline bool _isSD(const char* path) {
        return !strncmp(path, HFS_SD_PREFIX, HFS_SD_PREFIX_LEN);
    }
};