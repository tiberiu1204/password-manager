cmake_minimum_required(VERSION 3.27)
project(password_manager)

add_subdirectory(deps/SQLiteCpp)
include_directories(include deps/SQLiteCpp/include deps/SQLiteCpp/include/SQLiteCpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(password_manager main.cpp
        src/bcrypt.cpp
        src/user.cpp
        include/user.h
        src/user_data.cpp
        src/program.cpp
        include/program.h
        include/commands.h
        src/commands.cpp
        include/data_encryption.h
        src/data_encryption.cpp
)

target_link_libraries(password_manager SQLiteCpp)
