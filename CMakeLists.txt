file(GLOB SOURCES "src/*.cpp" "src/*.h" "log.h")

IF(BIICODE)
    INIT_BIICODE_BLOCK()

    #Include biicode Boost setup
    include(biicode/boost/setup)
    
    # Including tools.cmake from biicode/cmake user block
    include(biicode/cmake/tools)    
    
    list(APPEND BII_LIB_SRC ${SOURCES})

    ADD_BIICODE_TARGETS()
    
    # Calling specific macro to activate c++11 flags
    ACTIVATE_CPP11(INTERFACE ${BII_BLOCK_TARGET})    

    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)

    bii_setup_boost(COMPONENTS filesystem system thread REQUIRED)
    find_package(Boost COMPONENTS filesystem system thread REQUIRED)

    target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})
    
    foreach(target ${BII_BLOCK_TARGETS})
        set_target_properties(${target} PROPERTIES FOLDER "clrn/common")
    endforeach()
    
else()
    set(PROJECT_NAME lib_log)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "clrn/common")
    add_subdirectory(tests)
endif()
