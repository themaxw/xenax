cmake_minimum_required(VERSION 3.16)

project(xenax)

set(CMAKE_CXX_STANDARD 20)

add_executable(xenax main.cpp)

# On OSX, need to add the WebKit framework for WebView functionality
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(xenax "-framework WebKit -framework CoreServices")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules (gtk3 REQUIRED gtk+-3.0 IMPORTED_TARGET)
    pkg_check_modules (webkit2 REQUIRED webkit2gtk-4.1 IMPORTED_TARGET)
    target_link_libraries (xenax PUBLIC pthread PkgConfig::gtk3 PkgConfig::webkit2)
endif()

set(BOOST_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/modules/boost/libs)

# TODO change this to a installation dir
target_compile_definitions(xenax PRIVATE STATIC_FILE_DIR="${PROJECT_SOURCE_DIR}/static/")

target_include_directories(xenax
    SYSTEM PRIVATE
    ${BOOST_LOCATION}/align/include/
    ${BOOST_LOCATION}/asio/include/
    ${BOOST_LOCATION}/assert/include/
    ${BOOST_LOCATION}/beast/include/
    ${BOOST_LOCATION}/bind/include/
    ${BOOST_LOCATION}/config/include/
    ${BOOST_LOCATION}/core/include/
    ${BOOST_LOCATION}/date_time/include/
    ${BOOST_LOCATION}/endian/include/
    ${BOOST_LOCATION}/intrusive/include/
    ${BOOST_LOCATION}/is_placeholder/include/
    ${BOOST_LOCATION}/io/include/
    ${BOOST_LOCATION}/logic/include/
    ${BOOST_LOCATION}/numeric/conversion/include/
    ${BOOST_LOCATION}/optional/include/
    ${BOOST_LOCATION}/static_assert/include/
    ${BOOST_LOCATION}/smart_ptr/include/
    ${BOOST_LOCATION}/system/include/
    ${BOOST_LOCATION}/move/include/
    ${BOOST_LOCATION}/mp11/include/
    ${BOOST_LOCATION}/mpl/include/
    ${BOOST_LOCATION}/numeric_conversion/include/
    ${BOOST_LOCATION}/preprocessor/include/
    ${BOOST_LOCATION}/predef/include/
    ${BOOST_LOCATION}/regex/include/
    ${BOOST_LOCATION}/throw_exception/include/
    ${BOOST_LOCATION}/type_traits/include/
    ${BOOST_LOCATION}/utility/include/
    ${BOOST_LOCATION}/winapi/include/
    ${BOOST_LOCATION}/static_string/include/
)