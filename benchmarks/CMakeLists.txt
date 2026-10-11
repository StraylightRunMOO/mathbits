add_executable(mathbits_bench bench_main.c)
target_link_libraries(mathbits_bench PRIVATE mathbits_internal)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(mathbits_bench PRIVATE -O3)
endif()
