file(GLOB THIS_APP_SOURCES *.cpp)
file(GLOB THIS_APP_HEADERS *.h)

project(shared_mutex_lock_server)

include_directories(resource)

add_executable(${PROJECT_NAME} ${THIS_APP_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME} pthread rt)
