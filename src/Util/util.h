﻿/*
 * Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
 *
 * This file is part of ZLToolKit(https://github.com/xiongziliang/ZLToolKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include <ctime>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <unistd.h>
#include <sys/time.h>

#define INSTANCE_IMP(class_name, ...) \
class_name &class_name::Instance() { \
    static std::shared_ptr<class_name> s_instance(new class_name(__VA_ARGS__)); \
    static class_name &s_insteanc_ref = *s_instance; \
    return s_insteanc_ref; \
}

using namespace std;

namespace toolkit {

#define StrPrinter _StrPrinter()
class _StrPrinter : public string {
public:
    _StrPrinter() {}

    template<typename T>
    _StrPrinter& operator <<(T && data) {
        _stream << std::forward<T>(data);
        this->string::operator=(_stream.str());
        return *this;
    }

    string operator <<(std::ostream&(*f)(std::ostream&)) const {
        return *this;
    }

private:
    stringstream _stream;
};

//禁止拷贝基类
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    //禁止拷贝
    noncopyable(const noncopyable &that) = delete;
    noncopyable(noncopyable &&that) = delete;
    noncopyable &operator=(const noncopyable &that) = delete;
    noncopyable &operator=(noncopyable &&that) = delete;
};

//可以保存任意的对象
class Any{
public:
    typedef std::shared_ptr<Any> Ptr;

    Any() = default;
    ~Any() = default;

    template <typename C,typename ...ArgsType>
    void set(ArgsType &&...args){
        _data.reset(new C(std::forward<ArgsType>(args)...),[](void *ptr){
            delete (C*) ptr;
        });
    }
    template <typename C>
    C& get(){
        if(!_data){
            throw std::invalid_argument("Any is empty");
        }
        C *ptr = (C *)_data.get();
        return *ptr;
    }

    operator bool() {
        return _data.operator bool ();
    }
    bool empty(){
        return !bool();
    }
private:
    std::shared_ptr<void> _data;
};

//用于保存一些外加属性
class AnyStorage : public unordered_map<string,Any>{
public:
    AnyStorage() = default;
    ~AnyStorage() = default;
    typedef std::shared_ptr<AnyStorage> Ptr;
};

//对象安全的构建和析构
//构建后执行onCreate函数
//析构前执行onDestory函数
//在函数onCreate和onDestory中可以执行构造或析构中不能调用的方法，比如说shared_from_this或者虚函数
class Creator {
public:
    template<typename C,typename ...ArgsType>
    static std::shared_ptr<C> create(ArgsType &&...args){
        std::shared_ptr<C> ret(new C(std::forward<ArgsType>(args)...),[](C *ptr){
            ptr->onDestory();
            delete ptr;
        });
        ret->onCreate();
        return ret;
    }
private:
    Creator() = default;
    ~Creator() = default;
};

string makeRandStr(int sz, bool printable = true);
string hexdump(const void *buf, size_t len);
string exePath();
string exeDir();
string exeName();

vector<string> split(const string& s, const char *delim);
//去除前后的空格、回车符、制表符...
std::string& trim(std::string &s,const string &chars=" \r\n\t");
std::string trim(std::string &&s,const string &chars=" \r\n\t");
// string转小写
std::string &strToLower(std::string &str);
std::string strToLower(std::string &&str);
// string转大写
std::string &strToUpper(std::string &str);
std::string strToUpper(std::string &&str);
void replace(string &str, const string &old_str, const string &new_str) ;
bool isIP(const char *str);

#ifndef bzero
#define bzero(ptr,size)  memset((ptr),0,(size));
#endif //bzero

/**
 * 获取1970年至今的毫秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
uint64_t getCurrentMillisecond(bool system_time = false);

/**
 * 获取1970年至今的微秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
uint64_t getCurrentMicrosecond(bool system_time = false);

/**
 * 获取时间字符串
 * @param fmt 时间格式，譬如%Y-%m-%d %H:%M:%S
 * @return 时间字符串
 */
string getTimeStr(const char *fmt,time_t time = 0);

}  // namespace toolkit
#endif /* UTIL_UTIL_H_ */
