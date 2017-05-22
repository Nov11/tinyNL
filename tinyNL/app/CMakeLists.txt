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
        )

set(lib net base)
add_executable(ls ${APP_SOURCE})
target_link_libraries(ls ${lib})