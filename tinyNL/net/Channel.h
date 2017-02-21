//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_CHANNEL_H
#define TINYNL_CHANNEL_H

#include <functional>

namespace tinyNL {
    namespace net {
        class EventLoop;

        class Channel {
        public:
            typedef std::function<void()> CallBack;

            Channel(int fd, tinyNL::net::EventLoop *loop);

            void handleEvent();

            void setReadCallBack(CallBack &cb);

            void setWriteCallBack(CallBack &cb);

            void setErrorCallBack(CallBack &cb);

            void setRevent(uint32_t et);

            void enableRead();

            void enableWrite();

            void disableRead();

            void disableWrite();

            void disableChannel();

            void setAddedIntoMultiplexer(bool on) {
                added = on;
            }

            int fd() const { return fd_; }

            uint32_t events() const { return events_; }

            bool isDisabled() const { return disabled; }

            bool isAdded() const { return added; }

//            static enum WAITFOREVENT{};
            EventLoop *getEventLoop() const { return loop_; }

        private:


            int fd_;
            EventLoop *loop_;
            uint32_t events_;
            uint32_t revents_;
            //if set, this channel is added to multiplexer
            bool added;
            //if disabled, remove relate fd from multiplexer
            bool disabled;
            CallBack readCallBack;
            CallBack writeCallBack;
            CallBack errorCallBack;

            void updateChannel();
        };
    }
}
#endif //TINYNL_CHANNEL_H
