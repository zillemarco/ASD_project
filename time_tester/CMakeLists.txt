project(time_tester C CXX)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/../common/include")

# Common files
file(GLOB COMMON_INC "${PROJECT_SOURCE_DIR}/../common/include/*.h")
file(GLOB COMMON_SRC "${PROJECT_SOURCE_DIR}/../common/src/*.cpp")

file(GLOB TIME_TESTER_INC "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB TIME_TESTER_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Add project filters for the IDE
source_group(common\\include        FILES ${COMMON_INC})
source_group(common\\src            FILES ${COMMON_SRC})
source_group(time_tester\\include   FILES ${TIME_TESTER_INC})
source_group(time_tester\\src       FILES ${TIME_TESTER_SRC})

add_executable(${PROJECT_NAME} ${COMMON_INC} ${COMMON_SRC} ${TIME_TESTER_INC} ${TIME_TESTER_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX        "${RELEASE_SUFFIX}")
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX          "${DEBUG_SUFFIX}")
set_target_properties(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX     "${MIN_SIZE_RELEASE_SUFFIX}")
set_target_properties(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "${RELEASE_WITH_DEBUG_INFO_SUFFIX}")