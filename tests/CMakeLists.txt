add_executable(mathbits_test
    test_main.c
    test_num.c
    test_vec.c
    test_mat.c
    test_linalg.c
    test_tensor.c
    test_nn.c
    test_geometry.c
    test_optim.c
    test_sparse.c
)
target_link_libraries(mathbits_test PRIVATE mathbits_internal)
add_test(NAME mathbits_unit COMMAND mathbits_test)

add_executable(mathbits_vec_only test_vec_only.c)
target_link_libraries(mathbits_vec_only PRIVATE mathbits_internal)
add_test(NAME mathbits_vec_only COMMAND mathbits_vec_only)
