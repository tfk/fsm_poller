cmake_minimum_required(VERSION 3.21)

project(fsm_poller
  VERSION 1.0
  LANGUAGES CXX
)

add_library(fsm_poller INTERFACE)

target_include_directories(fsm_poller INTERFACE include/>)
