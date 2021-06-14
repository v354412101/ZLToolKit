﻿/*
 * Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
 *
 * This file is part of ZLToolKit(https://github.com/xiongziliang/ZLToolKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef NETWORK_TCPCLIENT_H
#define NETWORK_TCPCLIENT_H

#include <mutex>
#include <memory>
#include <functional>
#include "Socket.h"
#include "Util/TimeTicker.h"

using namespace std;

namespace toolkit {


//Tcp客户端，接口线程安全的
class TcpClient : public std::enable_shared_from_this<TcpClient>, public SocketHelper{
public:
    typedef std::shared_ptr<TcpClient> Ptr;
    TcpClient(const EventPoller::Ptr &poller = nullptr);
    virtual ~TcpClient();
    //开始连接服务器，strUrl可以是域名或ip
    virtual void startConnect(const string &strUrl, uint16_t iPort, float fTimeOutSec = 3);
    //主动断开服务器
    void shutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown")) override ;
    //是否与服务器连接中
    bool alive();
    //设置网卡适配器
    void setNetAdapter(const string &localIp);
    //获取对象相关信息
    string getIdentifier() const override;
protected:
    //连接服务器结果回调
    virtual void onConnect(const SockException &ex) {}
    //收到数据回调
    virtual void onRecv(const Buffer::Ptr &pBuf) {}
    //数据全部发送完毕后回调
    virtual void onFlush() {}
    //被动断开连接回调
    virtual void onErr(const SockException &ex) {}
    //tcp连接成功后每2秒触发一次该事件
    virtual void onManager() {}
private:
    void onSockConnect(const SockException &ex);
private:
    std::shared_ptr<Timer> _managerTimer;
    string _netAdapter = "0.0.0.0";
};

} /* namespace toolkit */
#endif /* NETWORK_TCPCLIENT_H */
