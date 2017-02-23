set(NET_SOURCE
        Channel.h
        Socket.cpp
        EventLoop.cpp
        CurrentThread.cpp
        Channel.cpp
        Multiplexer.cpp
        EPoller.cpp
        TimerQueue.cpp TimerQueue.h Timer.cpp Timer.h Acceptor.cpp Acceptor.h)
add_library(net ${NET_SOURCE})
add_subdirectory(tests)