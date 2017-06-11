set(APP_SOURCE
        compiletest.cpp
        )

set(lib net base)


#find lua library
#include findlua module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "CMAKE_CURRENT_SOURCE_DIR/Modules/")
# find lua packages first
find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LUA_LIBRARIES})
#done finding lua library

add_executable(t ${APP_SOURCE})
target_link_libraries(t ${lib} ${LIBS})

add_custom_command(TARGET t PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/foo.lua ${CMAKE_CURRENT_BINARY_DIR}/scripts/foo.lua )



add_library(luacommon common.h common.cpp)
add_executable(srvwithscript srv.cpp )
target_link_libraries(srvwithscript ${lib} ${LIBS} luacommon)

add_custom_command(TARGET srvwithscript PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/calledinlua.lua ${CMAKE_CURRENT_BINARY_DIR}/scripts/calledinlua.lua )
#message(${CMAKE_CURRENT_SOURCE_DIR}/scripts/calledinlua.lua)
#message(${CMAKE_CURRENT_BINARY_DIR}/scripts/calledinlua.lua)
