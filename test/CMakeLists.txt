find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests GTest::GTest GTest::Main)
