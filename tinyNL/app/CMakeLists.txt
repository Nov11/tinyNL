set(APP_SOURCE
      ls.cpp
        ls.h
        md5.c
        md5.h
        rsa.c
        rsa.h
        bn_mul.h
        bignum.h
        bignum.c
        Utils.h
        Utils.cpp
        )

set(lib net base)
add_executable(ls ${APP_SOURCE})
add_executable(kmp Search.h kmptest.cpp)
target_link_libraries(ls ${lib})