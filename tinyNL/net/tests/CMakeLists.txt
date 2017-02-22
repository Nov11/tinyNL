add_executable(et EventLoopTest.cpp)
target_link_libraries(et net base)

add_executable(timertest TimerTest.cpp)
target_link_libraries(timertest net base pthread)