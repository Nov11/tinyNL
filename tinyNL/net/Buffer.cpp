//
// Created by c6s on 17-2-23.
//

#include <unistd.h>
#include "Buffer.h"
#include <tinyNL/net/Socket.h>
#include <iostream>

namespace tinyNL{
    namespace net{
        void Buffer::append(const char *src, size_t len) {
            Pos blank = readIdx;
            //just this size is arbitrarily chosen
            if(needShrimp(blank)){
                std::vector<char> nvec;
                std::copy(&innerbuf_[readIdx], &innerbuf_[writeIdx], std::back_inserter(nvec));
                std::copy(src, src + len, std::back_inserter(nvec));
                swap(nvec, innerbuf_);
                readIdx=0;
                writeIdx=innerbuf_.size();
            }else{
                std::copy(src, src + len, std::back_inserter(innerbuf_));
                writeIdx=innerbuf_.size();
            }
        }

        void Buffer::erase(size_t len) {
            readIdx += len;
            if(needShrimp(readIdx)){
                std::vector<char> nvec;
                std::copy(&innerbuf_[readIdx], &innerbuf_[writeIdx], std::back_inserter(nvec));
                swap(innerbuf_, nvec);
                readIdx = 0;
                writeIdx = innerbuf_.size();
            }
        }

    }
}