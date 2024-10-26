cmake_minimum_required(VERSION 3.29)
project(arduino_ml C CXX)

set(CMAKE_CXX_STANDARD 20)

option(WIFI_SSID "WiFi SSID" "")
option(WIFI_PASSWD "WiFi Password" "")

if(NOT WIFI_SSID OR NOT WIFI_PASSWD)
    message(FATAL_ERROR "Please specify both WIFI_SSID and WIFI_PASSWD as CMake options, e.g., -DWIFI_SSID=your_ssid -DWIFI_PASSWD=your_password")
endif()
configure_file(${CMAKE_SOURCE_DIR}/src/wifi_conf.hpp.in ${CMAKE_SOURCE_DIR}/src/wifi_conf.hpp @ONLY)

file(GLOB ${PROJECT_NAME}_SRC "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(arduino_ml ${${PROJECT_NAME}_SRC})
target_link_arduino_libraries(arduino_ml PRIVATE
        Chirale_TensorFLowLite
        WebServer WiFi core)
target_enable_arduino_upload(arduino_ml)
