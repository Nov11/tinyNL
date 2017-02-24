add_executable(logtest logtest.cpp)
target_link_libraries(logtest base net)
add_executable(threadTest threadTest.cpp)
target_link_libraries(threadTest base net)