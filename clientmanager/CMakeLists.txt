include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME clientmanager)
pr_add_library(${PROJ_NAME} STATIC)

pr_add_dependency(${PROJ_NAME} networkmanager TARGET PUBLIC)
pr_add_external_dependency(${PROJ_NAME} boost LIBRARY)

pr_init_module(${PROJ_NAME})

pr_add_compile_definitions(${PROJ_NAME} -DBOOST_ALL_DYN_LINK)

pr_finalize(${PROJ_NAME})
