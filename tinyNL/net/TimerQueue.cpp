//
// Created by c6s on 17-2-21.
//

#include "TimerQueue.h"
#include <sys/timerfd.h>
#include <tinyNL/base/Log.h>
#include <tinyNL/net/Channel.h>

namespace tinyNL {
    namespace net {
        TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop) {
            int ret = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
            if (ret == -1) { tinyNL::base::LOG.logErrorAndExit(); }
            timerfd_ = ret;
            channelPtr_ = std::shared_ptr<Channel>(new Channel(timerfd_, loop_));
            //set call back
            //deal with life time range later
            rcb = [this](){readCallBack();};
                    //std::bind(TimerQueue::readCallBack, this, std::placeholders::_1);
            channelPtr_->setReadCallBack(rcb);
            channelPtr_->enableRead();
        }


        TimerQueue::~TimerQueue() {
            channelPtr_->disableChannel();
            close(timerfd_);
        }

        void TimerQueue::readCallBack() {
            //read timerfd
            //get expired tasks
            //queue them
            //find the next timer
            //set up timer
        }

    }
}

