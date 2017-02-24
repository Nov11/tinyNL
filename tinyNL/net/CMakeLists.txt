set(NET_SOURCE
        Channel.h
        Socket.cpp
        EventLoop.cpp
        CurrentThread.cpp
        Channel.cpp
        Multiplexer.cpp
        EPoller.cpp
        TimerQueue.cpp TimerQueue.h Timer.cpp Timer.h Acceptor.cpp Acceptor.h TcpServer.cpp TcpServer.h TcpConnection.cpp TcpConnection.h Buffer.cpp Buffer.h EventLoopThreadPool.cpp EventLoopThreadPool.h EventLoopThread.cpp EventLoopThread.h)
add_library(net ${NET_SOURCE})
add_subdirectory(tests)