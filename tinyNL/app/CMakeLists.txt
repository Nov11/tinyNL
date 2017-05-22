set(APP_SOURCE
      ls.cpp
        )

set(lib net base)
add_executable(ls ${APP_SOURCE})
target_link_libraries(ls ${lib})