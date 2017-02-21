//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_MULTIPLEXER_H
#define TINYNL_MULTIPLEXER_H

#include <tinyNL/dir/Noncopyable.h>
#include <vector>

namespace tinyNL{
    namespace net{
        class Channel;
        class Multiplexer : Noncopyable{
        public:
            typedef std::vector<Channel*> ChannelList;
            virtual void multiplexerPolling(int timeoutMillionSecond, ChannelList& readyChannelList) = 0;
            virtual ~Multiplexer();

            //remove these two later
//            static enum ACTION{ADD, DEL, MOD};
//            virtual void multiplexerUpdate(ACTION action, Channel *channel) = 0;

            virtual void multiplexerUpdate(Channel* channel) = 0;
        };
    }
}



#endif //TINYNL_MULTIPLEXER_H
