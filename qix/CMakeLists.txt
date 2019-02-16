project(qix)

add_definitions(-DASSETS_FOLDER="${qix_SOURCE_DIR}/assets/" )

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(qix src/)

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(qix ${SourceFiles})

target_link_libraries(qix ${SDL2_LIBRARY})
target_link_libraries(qix ${SDL2_TTF_LIBRARIES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(qix -lc++)

  if (UNIX)
    target_link_libraries(qix -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(qix -lstdc++)
  target_link_libraries(qix -lm)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET qix PROPERTY CXX_STANDARD 17)
endif()

# Build the test
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

file(GLOB_RECURSE SourceFiles src/game/* src/utility/*.cpp src/network/*.cpp test/*.cpp)

add_executable(qix_test ${SourceFiles})
add_dependencies(qix_test catch)

target_link_libraries(qix_test ${SDL2_LIBRARY})
target_link_libraries(qix_test ${SDL2_TTF_LIBRARIES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(qix_test -lc++)
  if (UNIX)
    target_link_libraries(qix_test -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(qix_test -lstdc++)
  target_link_libraries(qix_test -lm)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET qix_test PROPERTY CXX_STANDARD 17)
endif()
