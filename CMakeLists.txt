cmake_minimum_required(VERSION 3.20)
project(icp_final_opengl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(OpenCV_ROOT "${VCPKG_INSTALLED_DIR}/arm64-osx/share/opencv4")
find_package(OpenCV REQUIRED)

add_executable(icp_final_opengl
    main.cpp
    app.cpp
    callbacks.cpp
    gl_err_callback.cpp
    getinfo.cpp
    OBJloader.cpp
    ShaderProgram.cpp
    Texture.cpp
    AudioManager.cpp
    miniaudio.c
)

target_include_directories(icp_final_opengl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(icp_final_opengl PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
    imgui::imgui
    nlohmann_json::nlohmann_json
    ${OpenCV_LIBS}
    
)