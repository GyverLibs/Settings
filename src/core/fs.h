#pragma once
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <StringUtils.h>
#include <functional>
#include <type_traits>

namespace sets {

class FSClass {
   public:
    FSClass() {
        _fs = &LittleFS;

        _totalBytes = []() -> size_t {
#ifdef ESP8266
            FSInfo64 fs_info{};
            LittleFS.info64(fs_info);
            return fs_info.totalBytes;
#else
            return LittleFS.totalBytes();
#endif
        };

        _usedBytes = []() -> size_t {
#ifdef ESP8266
            FSInfo64 fs_info{};
            LittleFS.info64(fs_info);
            return fs_info.usedBytes;
#else
            return LittleFS.usedBytes();
#endif
        };
    }

    template <typename T, typename = typename std::enable_if<std::is_base_of<FS, T>::value>::type>
    void setFS(
        T& fs,
        const std::function<size_t()>& totalBytes,
        const std::function<size_t()>& usedBytes
    ) {
        _fs = &fs;
        _totalBytes = totalBytes;
        _usedBytes = usedBytes;
    }

    void listDir(String& str, const char* path, bool withSize = false) {
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

    bool remove(Text path) {
        return _fs->remove(path.c_str());
    }

    File openRead(Text path) {
        return _fs->open(path.c_str(), "r");
    }

    File openWrite(Text path) {
        mkdir(path);
        return _fs->open(path.c_str(), "w");
    }

    void mkdir(Text tpath) {
#ifdef ESP32
        Text::Cstr path = tpath.c_str();
        if (!_fs->exists(path)) {
            if (strchr(path, '/')) {
                char* pathStr = strdup(path);
                if (pathStr) {
                    char* ptr = strchr(pathStr, '/');
                    while (ptr) {
                        *ptr = 0;
                        _fs->mkdir(pathStr);
                        *ptr = '/';
                        ptr = strchr(ptr + 1, '/');
                    }
                }
                free(pathStr);
            }
        }
#else
#endif
    }

    void rmdir(Text path) {
#ifdef ESP32
        char* pathStr = strdup(path.c_str());
        if (pathStr) {
            char* ptr = strrchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                _fs->rmdir(pathStr);
                ptr = strrchr(pathStr, '/');
            }
            free(pathStr);
        }
#else
#endif
    }

    uint64_t freeSpace() {
        return _totalBytes() - _usedBytes();
    }

    uint64_t totalSpace() {
        return _totalBytes();
    }

    uint64_t usedSpace() {
        return _usedBytes();
    }

   private:
    FS* _fs;

    std::function<size_t()> _totalBytes;
    std::function<size_t()> _usedBytes;
};

extern FSClass FS;

}  // namespace sets