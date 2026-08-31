cmake_minimum_required(VERSION 3.22)

project(CommunityFunctionsSE VERSION 0.2.0 LANGUAGES CXX)

include(cmake/headerlist.cmake)
include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE ${headers})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(cmake/config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
