add_executable(et EventLoopTest.cpp)
target_link_libraries(et net base)

add_executable(timertest TimerTest.cpp)
target_link_libraries(timertest net base pthread)
add_executable(asrv acceptorTestSrv.cpp)
target_link_libraries(asrv net base pthread)
add_executable(aclnt acceptorTestClnt.cpp)
target_link_libraries(aclnt net base pthread)