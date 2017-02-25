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
add_executable(elt EventLoopThreadTest.cpp)
target_link_libraries(elt net base)

add_executable(elstop eventloopstop.cpp)
target_link_libraries(elstop net base)

add_executable(echosrv echosrv.cpp)
target_link_libraries(echosrv net base)
add_executable(connectortest connectortest.cpp)
target_link_libraries(connectortest net base)

add_executable(tcpcltTest tcpclientTest.cpp)
target_link_libraries(tcpcltTest ${lib})