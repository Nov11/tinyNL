add_library(testlib commoncb.cpp)
set(lib net base testlib)
add_executable(et EventLoopTest.cpp)
target_link_libraries(et net base)

add_executable(timertest TimerTest.cpp)
target_link_libraries(timertest net base pthread)
add_executable(asrv acceptorTestSrv.cpp)
target_link_libraries(asrv net base pthread)
add_executable(aclnt acceptorTestClnt.cpp)
target_link_libraries(aclnt  base net  pthread)

add_executable(tcpsrv tcpSrvTest.cpp)
target_link_libraries(tcpsrv net base pthread)

add_executable(buffertest buffertest.cpp)
target_link_libraries(buffertest net base)
add_executable(eltt EventLoopThreadTest.cpp)
target_link_libraries(eltt net base)

add_executable(elstop eventloopstop.cpp)
target_link_libraries(elstop net base)

add_executable(echosrv echosrv.cpp)
target_link_libraries(echosrv net base)
add_executable(connectortest connectortest.cpp)
target_link_libraries(connectortest net base)

add_executable(tcpcltTest tcpclientTest.cpp)
target_link_libraries(tcpcltTest ${lib})

add_executable(ppclt pingpongclt.cpp)
target_link_libraries(ppclt ${lib})

add_executable(ppsrv pingpongsrv.cpp)
target_link_libraries(ppsrv ${lib})

add_executable(discardsrv discardsrv.cpp)
target_link_libraries(discardsrv net base)

add_executable(daytimesrv daytimesrv.cpp)
target_link_libraries(daytimesrv ${lib})

add_executable(timesrv timesrv.cpp)
target_link_libraries(timesrv net base)

add_executable(timeclt timeclt.cpp)
target_link_libraries(timeclt ${lib})

add_executable(chargen chargen.cpp)
target_link_libraries(chargen ${lib})

add_executable(hy singlethreadhybridsrv.cpp)
target_link_libraries(hy ${lib})

add_executable(filesrv fileSrv.cpp)
target_link_libraries(filesrv ${lib})

add_executable(srvheader ServerWithPacketHeader.cpp)
target_link_libraries(srvheader ${lib})

add_executable(cltheader ClientWithPacketHeader.cpp)
target_link_libraries(cltheader ${lib})