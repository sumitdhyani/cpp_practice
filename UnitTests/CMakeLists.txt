set (ALL_HEADERS ../CustomAllocatorSegmentTree.hpp)
set (ALL_SOURCES AllocatorSegmentTreeTests.cpp)


set (ALL_HEADERS_AND_SOURCES ${ALL_HEADERS} ${ALL_SOURCES})
project(unitTests)
add_executable("${PROJECT_NAME}" "${ALL_HEADERS_AND_SOURCES}")
target_include_directories("${PROJECT_NAME}" PUBLIC  "$ENV{GTEST_ROOT}/googletest/include" "${CMAKE_SOURCE_DIR}" ..)
target_link_directories("${PROJECT_NAME}" PUBLIC "$ENV{GTEST_ROOT}/build/lib/" "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" "${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")
target_link_libraries("${PROJECT_NAME}" "${GTEST_LIBS}" )
