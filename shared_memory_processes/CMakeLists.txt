file(GLOB READ_SOURCES main_read.cpp shared_packet.cpp)
file(GLOB WRITE_SOURCES main_write.cpp shared_packet.cpp)
file(GLOB THIS_APP_HEADERS *.h)

project(shared_mutex_lock_processes)

include_directories(resource)

add_executable(${PROJECT_NAME}_read ${READ_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_read pthread rt)

add_executable(${PROJECT_NAME}_write ${WRITE_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_write pthread rt)
