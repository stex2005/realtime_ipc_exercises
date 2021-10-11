cmake_minimum_required(VERSION 2.8)
file(GLOB MUTEX_SOURCES main.cpp)
file(GLOB EX_SOURCES main_ex.cpp)
file(GLOB THIS_APP_HEADERS *.h)

project(mutex_lock_threads)

include_directories(resource)

add_executable(${PROJECT_NAME} ${MUTEX_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME} pthread rt)

add_executable(${PROJECT_NAME}_ex ${EX_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_ex pthread rt)
