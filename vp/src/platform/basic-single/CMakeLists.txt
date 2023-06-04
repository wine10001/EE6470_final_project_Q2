file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-basic-single
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-basic-single PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platform-basic-single systemc)

add_executable(riscv-vp-single
        main.cpp)

target_link_libraries(riscv-vp-single rv32 platform-basic-single platform-common gdb-mc ${Boost_LIBRARIES} systemc pthread)

INSTALL(TARGETS riscv-vp-single RUNTIME DESTINATION bin)
