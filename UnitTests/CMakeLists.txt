project(unitTests)

set (INCLUDE_DIRS "$ENV{GTEST_ROOT}/googletest/include" "${CMAKE_SOURCE_DIR}" ..)
set (LIB_DIRS "$ENV{GTEST_ROOT}/build/lib/" "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" "${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")
set (LIBS "${GTEST_LIBS}")

add_executable(AllocatorSegmentTreeTests AllocatorSegmentTreeTests.cpp)
target_include_directories(AllocatorSegmentTreeTests PUBLIC ${INCLUDE_DIRS})
target_link_directories(AllocatorSegmentTreeTests PUBLIC ${LIB_DIRS})
target_link_libraries(AllocatorSegmentTreeTests PUBLIC ${LIBS})

add_executable(LinkedListAllocatorTests LinkedListAllocatorTests.cpp)
target_include_directories(LinkedListAllocatorTests PUBLIC ${INCLUDE_DIRS})
target_link_directories(LinkedListAllocatorTests PUBLIC ${LIB_DIRS})
target_link_libraries(LinkedListAllocatorTests PUBLIC ${LIBS})

#add_executable(LinkedListAllocatorTests LinkedListAllocatorTests.cpp)
#target_include_directories(LinkedListAllocatorTests PUBLIC ${LIB_DIRS})
#target_link_libraries(LinkedListAllocatorTests ${LIBS})