#pragma once
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <StringUtils.h>

#define ST_FS LittleFS

namespace sets {

class FSClass {
   public:
    void listDir(String& str, const char* path, bool withSize = false) {
#ifdef ESP8266
        Dir dir = ST_FS.openDir(path);
        while (dir.next()) {
            if (dir.isDirectory()) {
                String p(path);
                p += dir.fileName();
                p += '/';
                Dir sdir = ST_FS.openDir(p);
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
        File root = ST_FS.open(path);
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
        return ST_FS.remove(path.c_str());
    }

    File openRead(Text path) {
        return ST_FS.open(path.c_str(), "r");
    }

    File openWrite(Text path) {
        mkdir(path);
        return ST_FS.open(path.c_str(), "w");
    }

    void mkdir(Text tpath) {
#ifdef ESP32
        Text::Cstr path = tpath.c_str();
        if (!ST_FS.exists(path)) {
            if (strchr(path, '/')) {
                char* pathStr = strdup(path);
                if (pathStr) {
                    char* ptr = strchr(pathStr, '/');
                    while (ptr) {
                        *ptr = 0;
                        ST_FS.mkdir(pathStr);
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
                ST_FS.rmdir(pathStr);
                ptr = strrchr(pathStr, '/');
            }
            free(pathStr);
        }
#else
#endif
    }

    uint64_t freeSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        ST_FS.info64(fs_info);
        return fs_info.totalBytes - fs_info.usedBytes;
#else
        return ST_FS.totalBytes() - ST_FS.usedBytes();
#endif
    }

    uint64_t totalSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        ST_FS.info64(fs_info);
        return fs_info.totalBytes;
#else
        return ST_FS.totalBytes();
#endif
    }

    uint64_t usedSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        ST_FS.info64(fs_info);
        return fs_info.usedBytes;
#else
        return ST_FS.usedBytes();
#endif
    }
};

extern FSClass FS;

}  // namespace sets