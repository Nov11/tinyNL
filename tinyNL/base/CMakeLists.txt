set(BASE_SOURCE base.cpp Log.cpp Log.h Mutex.cpp Mutex.h MutexLockGuard.cpp MutexLockGuard.h Cond.cpp Cond.h TimeUtilies.cpp TimeUtilies.h)
add_library(base ${BASE_SOURCE})
add_subdirectory(tests)