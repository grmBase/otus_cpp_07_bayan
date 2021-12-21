cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})


# имя проекта
project(bayan VERSION ${PROJECT_VESRION})


# поищем, потом может будем использовать
# find_package(GTest REQUIRED)




# поищем - нужно для генерации документации
#find_package(Doxygen QUIET)
#
#if (DOXYGEN_FOUND)
#
#    # set input and output files
#    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile_config)
#    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

#    message("debug start")
#    message(DOXYGEN_IN)
#    message(DOXYGEN_OUT)
#    message("debug finish")

#    # request to configure the file
#    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
#    add_custom_target( doc_doxygen ALL
#        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating API documentation with Doxygen"
#        VERBATIM )
#else (DOXYGEN_FOUND)
#  message("Doxygen need to be installed to generate the doxygen documentation")
#endif (DOXYGEN_FOUND)





# имя основной утилиты будет таким же как и имя проекта?
add_executable(bayan
src/main.cpp
src/md5.cpp
src/work_obj.cpp
)


target_include_directories(bayan PUBLIC
${PROJECT_SOURCE_DIR}/./
)



# пока не буду делать библиотеку, но по идее потом может и надо буует
#add_library(helloworld lib.cpp)
#add_executable(test_version test_version.cpp)

set_target_properties(bayan PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bayan
    PRIVATE "${CMAKE_BINARY_DIR}"
)

#  Пока не надо - тестов не добавляю
#set_target_properties(test_version PROPERTIES
#    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
#    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
#)


# Пока всё без доп библиотек
#target_link_libraries(helloworld_cli PRIVATE
#    helloworld
#)
#target_link_libraries(test_version
#    ${Boost_LIBRARIES}
#    helloworld
#)


if (MSVC)
    target_compile_options(bayan PRIVATE
        /W4
    )
else ()
    target_compile_options(bayan PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

# Тестов пока нет
#enable_testing()
#add_test(test_version test_version)
