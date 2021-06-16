﻿/*
 * Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
 *
 * This file is part of ZLToolKit(https://github.com/xiongziliang/ZLToolKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#include <stdlib.h>
#include <mutex>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "util.h"
#include "onceToken.h"
#include "Util/File.h"
#include "Util/logger.h"
#include "Util/uv_errno.h"
#include "Network/sockutil.h"

using namespace std;

namespace toolkit {

string makeRandStr(int sz, bool printable) {
    char *tmp = new char[sz + 1];
    static const char CCH[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;
    for (i = 0; i < sz; i++) {
        srand((unsigned)time(NULL) + i);
        if (printable) {
            int x = rand() % (sizeof(CCH) - 1);
            tmp[i] = CCH[x];
        }
        else {
            tmp[i] = rand() % 0xFF;
        }
    }
    tmp[i] = 0;
    string ret = tmp;
    delete[] tmp;
    return ret;
}

bool is_safe(uint8_t b) {
    return b >= ' ' && b < 128;
}
string hexdump(const void *buf, size_t len) {
    string ret("\r\n");
    char tmp[8];
    const uint8_t *data = (const uint8_t *)buf;
    for (size_t i = 0; i < len; i += 16) {
        for (int j = 0; j < 16; ++j) {
            if (i + j < len) {
                int sz = sprintf(tmp, "%.2x ", data[i + j]);
                ret.append(tmp, sz);
            }
            else {
                int sz = sprintf(tmp, "   ");
                ret.append(tmp, sz);
            }
        }
        for (int j = 0; j < 16; ++j) {
            if (i + j < len) {
                ret += (is_safe(data[i + j]) ? data[i + j] : '.');
            }
            else {
                ret += (' ');
            }
        }
        ret += ('\n');
    }
    return ret;
}

string exePath() {
    char buffer[PATH_MAX * 2 + 1] = { 0 };
    int n = -1;
    n = readlink("/proc/self/exe", buffer, sizeof(buffer));

    string filePath;
    if (n <= 0) {
        filePath = "./";
    }
    else {
        filePath = buffer;
    }

    return filePath;
}
string exeDir() {
    auto path = exePath();
    return path.substr(0, path.rfind('/') + 1);
}
string exeName() {
    auto path = exePath();
    return path.substr(path.rfind('/') + 1);
}
// string转小写
std::string &strToLower(std::string &str) {
    transform(str.begin(), str.end(), str.begin(), towlower);
    return str;
}
// string转大写
std::string &strToUpper(std::string &str) {
    transform(str.begin(), str.end(), str.begin(), towupper);
    return str;
}

// string转小写
std::string strToLower(std::string &&str) {
    transform(str.begin(), str.end(), str.begin(), towlower);
    return std::move(str);
}
// string转大写
std::string strToUpper(std::string &&str) {
    transform(str.begin(), str.end(), str.begin(), towupper);
    return std::move(str);
}

vector<string> split(const string& s, const char *delim) {
    vector<string> ret;
    int last = 0;
    int index = s.find(delim, last);
    while (index != string::npos) {
        if (index - last > 0) {
            ret.push_back(s.substr(last, index - last));
        }
        last = index + strlen(delim);
        index = s.find(delim, last);
    }
    if (!s.size() || s.size() - last > 0) {
        ret.push_back(s.substr(last));
    }
    return ret;
}

#define TRIM(s,chars) \
do{ \
    string map(0xFF, '\0'); \
    for (auto &ch : chars) { \
        map[(unsigned char &)ch] = '\1'; \
    } \
    while( s.size() && map.at((unsigned char &)s.back())) s.pop_back(); \
    while( s.size() && map.at((unsigned char &)s.front())) s.erase(0,1); \
    return s; \
}while(0);

//去除前后的空格、回车符、制表符
std::string& trim(std::string &s, const string &chars) {
    TRIM(s, chars);
}
std::string trim(std::string &&s, const string &chars) {
    TRIM(s, chars);
}

void replace(string &str, const string &old_str, const string &new_str) {
    if (old_str.empty() || old_str == new_str) {
        return;
    }
    auto pos = str.find(old_str);
    if (pos == string::npos) {
        return;
    }
    str.replace(pos, old_str.size(), new_str);
    replace(str, old_str, new_str);
}

bool isIP(const char *str){
    return INADDR_NONE != inet_addr(str);
}

static inline uint64_t getCurrentMicrosecondOrigin() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

static atomic<uint64_t> s_currentMicrosecond(0);
static atomic<uint64_t> s_currentMillisecond(0);
static atomic<uint64_t> s_currentMicrosecond_system(getCurrentMicrosecondOrigin());
static atomic<uint64_t> s_currentMillisecond_system(getCurrentMicrosecondOrigin() / 1000);

static inline bool initMillisecondThread() {
    static std::thread s_thread([]() {
        DebugL << "Stamp thread started!";
        uint64_t last = getCurrentMicrosecondOrigin();
        uint64_t now;
        uint64_t microsecond = 0;
        while (true) {
            now = getCurrentMicrosecondOrigin();
            //记录系统时间戳，可回退
            s_currentMicrosecond_system.store(now, memory_order_release);
            s_currentMillisecond_system.store(now / 1000, memory_order_release);

            //记录流逝时间戳，不可回退
            int64_t expired = now - last;
            last = now;
            if (expired > 0 && expired < 1000 * 1000) {
                //流逝时间处于0~1000ms之间，那么是合理的，说明没有调整系统时间
                microsecond += expired;
                s_currentMicrosecond.store(microsecond, memory_order_release);
                s_currentMillisecond.store(microsecond / 1000, memory_order_release);
            } else if(expired != 0){
                WarnL << "Stamp expired is not abnormal:" << expired;
            }
            usleep(500);
        }
    });
    static onceToken s_token([]() {
        s_thread.detach();
    });
    return true;
}

uint64_t getCurrentMillisecond(bool system_time) {
    static bool flag = initMillisecondThread();
    if(system_time){
        return s_currentMillisecond_system.load(memory_order_acquire);
    }
    return s_currentMillisecond.load(memory_order_acquire);
}

uint64_t getCurrentMicrosecond(bool system_time) {
    static bool flag = initMillisecondThread();
    if(system_time){
        return s_currentMicrosecond_system.load(memory_order_acquire);
    }
    return s_currentMicrosecond.load(memory_order_acquire);
}

string getTimeStr(const char *fmt,time_t time){
    std::tm tm_snapshot;
    if(!time){
        time = ::time(NULL);
    }
#if defined(_WIN32)
    localtime_s(&tm_snapshot, &time); // thread-safe
#else
    localtime_r(&time, &tm_snapshot); // POSIX
#endif
    char buffer[1024];
    auto success = std::strftime(buffer, sizeof(buffer), fmt, &tm_snapshot);
    if (0 == success)
        return string(fmt);
    return buffer;
}

}  // namespace toolkit

