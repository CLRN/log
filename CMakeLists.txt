set(PROJECT_NAME lib_log)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(conversion)

set(BOOST_COMPONENTS thread)
find_package(Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
find_package(Threads REQUIRED)
find_package(Log4cplus)

if (${Log4cplus_FOUND})
    file(GLOB SOURCES "src/*.cpp" "src/*.h" "src/log4cplus/*.cpp" "src/log4cplus/*.h" "include/log/log.h")
else()
    file(GLOB SOURCES "src/*.cpp" "src/*.h" "include/log/log.h")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "common")
target_link_libraries(${PROJECT_NAME}
    lib_conversion
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if (WITH_TESTS)
    add_subdirectory(tests)
endif()
