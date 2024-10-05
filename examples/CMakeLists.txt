add_executable(media_player media_player.cpp)

target_link_libraries(media_player PRIVATE fsm_poller)
set_property(TARGET media_player PROPERTY CXX_STANDARD 14) # allow auto-lamba expressions