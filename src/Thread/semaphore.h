/*
 * Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
 *
 * This file is part of ZLToolKit(https://github.com/xiongziliang/ZLToolKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <atomic>
#include <mutex>
#include <condition_variable>
using namespace std;

namespace toolkit {

class semaphore {
public:
    explicit semaphore(unsigned int initial = 0) {
        _count = 0;
    }
    
    ~semaphore() {
    }
    
    void post(unsigned int n = 1) {
        unique_lock<mutex> lock(_mutex);
        _count += n;
        if(n == 1){
            _condition.notify_one();
        }else{
            _condition.notify_all();
        }
    }
    void wait() {
        unique_lock<mutex> lock(_mutex);
        while (_count == 0) {
            _condition.wait(lock);
        }
        --_count;
    }
private:
    int _count;
    mutex _mutex;
    condition_variable_any _condition;
};

} /* namespace toolkit */
#endif /* SEMAPHORE_H_ */
