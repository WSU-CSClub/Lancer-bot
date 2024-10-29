cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME Lancer-bot)
project(${PROJECT_NAME} LANGUAGES CXX )

set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(GLOB_RECURSE SOURCES "src/*.cpp" )


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


FetchContent_MakeAvailable()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE)
