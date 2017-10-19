//
// Created by c6s on 17-2-23.
//

#include <unistd.h>
#include "Buffer.h"
#include <tinyNL/net/Socket.h>
#include <iostream>

namespace tinyNL{
    namespace net{
        bool Buffer::append(const char *src, size_t len) {
            Pos blank = readIdx;
            if(upperLimit - readableSize() < len){
                //make sure single buffer will not consume too much memory
                return false;
            }

            //just this size is arbitrarily chosen
            if(needShrink(blank)){
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
            return true;
        }

        void Buffer::erase(size_t len) {
            readIdx += len;
            if(needShrink(readIdx)){
                std::vector<char> nvec;
                std::copy(&innerbuf_[readIdx], &innerbuf_[writeIdx], std::back_inserter(nvec));
                swap(innerbuf_, nvec);
                readIdx = 0;
                writeIdx = innerbuf_.size();
            }
        }

        std::vector<char> Buffer::peek(size_t bytes) {
            size_t cnt = std::min(readableSize(), bytes);
            std::vector<char> result;
            for(size_t i = 0; i < cnt; i++){
                result.push_back(innerbuf_[readIdx + i]);
            }
            return result;
        }

    }
}