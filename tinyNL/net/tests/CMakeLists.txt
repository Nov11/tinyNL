add_executable(et EventLoopTest.cpp)
target_link_libraries(et net base)

add_executable(timertest TimerTest.cpp)
target_link_libraries(timertest net base pthread)
add_executable(asrv acceptorTestSrv.cpp)
target_link_libraries(asrv net base pthread)
add_executable(aclnt acceptorTestClnt.cpp)
target_link_libraries(aclnt net base pthread)

add_executable(tcpsrv tcpSrvTest.cpp)
target_link_libraries(tcpsrv net base pthread)

add_executable(buffertest buffertest.cpp)
target_link_libraries(buffertest net base)