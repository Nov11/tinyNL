set(BASE_SOURCE base.cpp Log.cpp Log.h Mutex.cpp Mutex.h MutexLockGuard.cpp MutexLockGuard.h Cond.cpp Cond.h TimeUtilies.cpp TimeUtilies.h Thread.cpp Thread.h IgnoreSIGPIPE.cpp IgnoreSIGPIPE.h CountDownLatch.cpp CountDownLatch.h)
add_library(base ${BASE_SOURCE})
add_subdirectory(tests)
target_link_libraries(base pthread)