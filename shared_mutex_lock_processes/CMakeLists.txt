file(GLOB CLIENT_SOURCES main_client.cpp shared_packet.cpp)
file(GLOB SERVER_SOURCES main_server.cpp shared_packet.cpp)
file(GLOB THIS_APP_HEADERS *.h)

project(shared_mutex_lock_processes)

include_directories(resource)

add_executable(${PROJECT_NAME}_client ${CLIENT_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_client pthread rt)

add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_server pthread rt)
