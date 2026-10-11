add_executable(ex_basic ex_basic.c)
target_link_libraries(ex_basic PRIVATE mathbits_internal)

add_executable(ex_mlp ex_mlp.c)
target_link_libraries(ex_mlp PRIVATE mathbits_internal)

if(MATHBITS_USE_MEMENTO)
    add_executable(ex_memento ex_memento.c)
    target_link_libraries(ex_memento PRIVATE mathbits_internal)
endif()

if(MATHBITS_SUSPENDERS_SOURCE AND EXISTS "${MATHBITS_SUSPENDERS_SOURCE}/suspenders.h")
    add_executable(ex_suspenders ex_suspenders.c)
    target_link_libraries(ex_suspenders PRIVATE mathbits_internal)
    target_include_directories(ex_suspenders PRIVATE "${MATHBITS_SUSPENDERS_SOURCE}")
endif()
