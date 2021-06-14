/*
 * Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
 *
 * This file is part of ZLToolKit(https://github.com/xiongziliang/ZLToolKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SERVER_SESSION_H_
#define SERVER_SESSION_H_

#include <memory>
#include <sstream>
#include "Socket.h"
#include "Util/logger.h"
#include "Util/mini.h"
#include "Thread/ThreadPool.h"
using namespace std;

namespace toolkit {

class TcpServer;
class TcpSession : public std::enable_shared_from_this<TcpSession>, public SocketHelper{
public:
    typedef std::shared_ptr<TcpSession> Ptr;

    TcpSession(const Socket::Ptr &pSock);
    virtual ~TcpSession();
    //接收数据入口
    virtual void onRecv(const Buffer::Ptr &) = 0;
    //收到eof或其他导致脱离TcpServer事件的回调
    virtual void onError(const SockException &err) = 0;
    //每隔一段时间触发，用来做超时管理
    virtual void onManager() = 0;
    //在创建TcpSession后，TcpServer会把自身的配置参数通过该函数传递给TcpSession
    virtual void attachServer(const TcpServer &server){};
    //作为该TcpSession的唯一标识符
    string getIdentifier() const override;
    //安全的脱离TcpServer并触发onError事件
    void safeShutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown"));
};

} /* namespace toolkit */
#endif /* SERVER_SESSION_H_ */
