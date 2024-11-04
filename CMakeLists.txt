cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME Lancer-bot)
project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include(FetchContent)

# Function to declare content with local fallback
function(declare_local_or_remote NAME LOCAL_DIR REPO_URL TAG)
    if(EXISTS ${LOCAL_DIR})
        FetchContent_Declare(
            ${NAME}
            SOURCE_DIR ${LOCAL_DIR}
        )
    else()
        message(STATUS "${NAME} not found locally, fetching from GitHub")
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${REPO_URL}
            GIT_TAG ${TAG}
        )
    endif()
endfunction()

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Create database directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/database)

# Configure SQL file
configure_file(
    ${CMAKE_SOURCE_DIR}/src/database/chess_openings.sql
    ${CMAKE_BINARY_DIR}/bin/database/chess_openings.sql
    COPYONLY
)

# Create and initialize database
add_custom_target(init_database ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Creating database..."
    COMMAND sqlite3 ${CMAKE_BINARY_DIR}/bin/database/chess_openings.db < ${CMAKE_BINARY_DIR}/bin/database/chess_openings.sql
    DEPENDS ${CMAKE_BINARY_DIR}/bin/database/chess_openings.sql
    COMMENT "Initializing chess database"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Make sure database is initialized before building the executable
add_dependencies(${PROJECT_NAME} init_database)

# Link SQLite3
target_link_libraries(${PROJECT_NAME} PRIVATE SQLite::SQLite3)