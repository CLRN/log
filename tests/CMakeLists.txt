set(PROJECT_NAME net_tests)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${CLIENT_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "common/tests")
target_link_libraries(${PROJECT_NAME}
	lib_common
	lib_net
	gtest
	gtest_main
)

include(CreateLaunchers)
create_target_launcher(
    ${PROJECT_NAME}
    ARGS "--gtest_filter=* --gtest_break_on_failure"
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}
)

add_test(${PROJECT_NAME} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe")