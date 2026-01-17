
cmake_minimum_required(VERSION 3.28.3)
project(CopyKnights LANGUAGES C)

file(GLOB_RECURSE sources src/*.c)
file(GLOB_RECURSE data src/data/*)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(ck ${sources})

target_compile_options(ck PUBLIC -Wall -fsanitize=undefined -g -Werror -std=c99 -pedantic)
target_include_directories(ck PUBLIC include/)
file(COPY ${data} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)


find_package(Curses REQUIRED)
target_include_directories(ck PUBLIC ${Curses_INCLUDE_DIR})
target_link_libraries(ck PUBLIC m ncurses ${Curses_LIBRARIES} ubsan)

# SDL2
find_package(SDL2 REQUIRED)
target_include_directories(ck PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(ck PUBLIC ${SDL2_CFLAGS_OTHER})

# SDL2_ttf
target_include_directories(ck PUBLIC /usr/include/SDL2)
target_link_libraries(ck PUBLIC /usr/lib/x86_64-linux-gnu/libSDL2_image.so /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so ${SDL2_LIBRARIES} m)
# target_link_libraries(ck PUBLIC ${SDL2_TTF_LIBRARIES})

add_custom_target(run
    COMMAND valgrind --track-origins=yes --leak-check=full --log-file=log ./ck
    DEPENDS ck
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building and running ck under Valgrind"
)

#install(TARGETS ck DESTINATION game/)
#install(DIRECTORY resources DESTINATION example_destination)

#set(CPACK_PACKAGE_NAME "CopyKnights")
#set(CPACK_PACKAGE_VERSION "0.4.3")

# we don't want to split our program up into several things
#set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
#include(CPack)
