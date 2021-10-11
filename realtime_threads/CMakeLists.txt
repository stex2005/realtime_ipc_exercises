file(GLOB RT_SOURCES main_rt.cpp loop_time_stats.cpp)
file(GLOB NRT_SOURCES main_nrt.cpp loop_time_stats.cpp)
file(GLOB EX_SOURCES main.cpp loop_time_stats.cpp)
file(GLOB HEADERS *.h)

project(realtime_threads)

include_directories(resource)

add_executable(${PROJECT_NAME}_rt ${RT_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_rt pthread rt)

add_executable(${PROJECT_NAME}_nrt ${NRT_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_nrt pthread rt)

add_executable(${PROJECT_NAME}_ex ${EX_SOURCES} ${THIS_APP_HEADERS})
target_link_libraries(${PROJECT_NAME}_ex pthread rt)
