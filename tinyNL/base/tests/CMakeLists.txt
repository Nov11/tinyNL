add_executable(logtest logtest.cpp)
target_link_libraries(logtest net base)
add_executable(threadTest threadTest.cpp)
target_link_libraries(threadTest net base)