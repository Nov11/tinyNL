//
// Created by c6s on 17-2-21.
//

#include <tinyNL/net/Channel.h>
#include <tinyNL/net/EventLoop.h>
#include <sys/epoll.h>

namespace tinyNL {
    namespace net {
        Channel::Channel(int fd, EventLoop *loop)
                : fd_(fd),
                  loop_(loop),
                  events_(0),
                  revents_(0),
                  added(false),
        disabled(false)
        {}

        void Channel::handleEvent() {
            if (revents_ & (EPOLLERR | EPOLLHUP | EPOLLIN | EPOLLPRI)) {
                if (readCallBack) {
                    readCallBack();
                }
            }
            if (revents_ & (EPOLLERR | EPOLLOUT)) {
                if (writeCallBack) {
                    writeCallBack();
                }
            }
        }

        void Channel::setReadCallBack(const Channel::CallBack &cb) {
            readCallBack = cb;
        }

        void Channel::setWriteCallBack(const Channel::CallBack &cb) {
            writeCallBack = cb;
        }

        void Channel::setErrorCallBack(const Channel::CallBack &cb) {
            errorCallBack = cb;
        }

        void Channel::setRevent(uint32_t et) {
            revents_ = et;
        }

        void Channel::enableRead() {
            events_ |= EPOLLIN;
            updateChannel();
        }

        void Channel::enableWrite() {
            events_ |= EPOLLOUT;
            updateChannel();
        }

        void Channel::disableRead() {
            events_ &= ~EPOLLIN;
            updateChannel();
        }

        void Channel::disableWrite() {
            events_ &= ~EPOLLOUT;
            updateChannel();
        }

        void Channel::updateChannel() {
            loop_->updateMultiplexer(this);
        }

        void Channel::disableChannel() {
            disabled = true;
            updateChannel();
        }
    }
}