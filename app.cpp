#include "app.hpp"

#include "getinfo.hpp"
#include "gl_err_callback.h"
#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <filesystem>
#include <vector>
#include <opencv2/opencv.hpp>

App::App() = default;

App::~App() {
    shutdown_imgui();

    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    glfwTerminate();
}

bool App::load_config() {
    std::ifstream file("config.json");
    if (!file) {
        std::cout << "config.json not found, using default settings.\n";
        return true;
    }

    nlohmann::json json;
    file >> json;

    config_.width = json.value("width", config_.width);
    config_.height = json.value("height", config_.height);
    config_.title = json.value("title", config_.title);
    config_.vsync_on = json.value("vsync_on", config_.vsync_on);
    config_.fps_update_interval = json.value("fps_update_interval", config_.fps_update_interval);

    return true;
}

bool App::init() {
    load_config();

    fps_.set_interval(std::chrono::duration<double>(config_.fps_update_interval));
    fps_.reset();

    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("GLFW initialization failed.");
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    #endif

    window_ = glfwCreateWindow(
        config_.width,
        config_.height,
        config_.title.c_str(),
        nullptr,
        nullptr
    );

    if (window_ == nullptr) {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    glewExperimental = GL_TRUE;
    const GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        throw std::runtime_error(
            std::string("GLEW initialization failed: ") +
            reinterpret_cast<const char*>(glewGetErrorString(glew_status))
        );
    }

    glGetError();

    print_gl_context_info();

    gl_version_text_ = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    GLint profile = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        gl_profile_text_ = "Core profile";
    } else if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
        gl_profile_text_ = "Compatibility profile";
    } else {
        gl_profile_text_ = "Unknown profile";
    }

    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if (major < 4 || (major == 4 && minor < 1)) {
        throw std::runtime_error("OpenGL 4.1 or newer is required.");
    }

    setup_debug_callback();

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    glfwSetKeyCallback(window_, key_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetCursorPosCallback(window_, cursor_position_callback);

    set_vsync(config_.vsync_on);
    init_assets();
    init_imgui();

    glViewport(0, 0, config_.width, config_.height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    update_projection_matrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    windowed_w_ = config_.width;
    windowed_h_ = config_.height;
    glfwGetWindowPos(window_, &windowed_x_, &windowed_y_);

    update_title();

    glfwShowWindow(window_);
    return true;
}

void App::setup_debug_callback() const {
    if (GLEW_KHR_debug || GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);
        std::cout << "OpenGL debug callback enabled.\n";
    } else {
        std::cout << "OpenGL debug callback not supported on this driver/context.\n";
    }
}

void App::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window_, true)) {
        throw std::runtime_error("ImGui GLFW backend init failed.");
    }

    if (!ImGui_ImplOpenGL3_Init("#version 410")) {
        throw std::runtime_error("ImGui OpenGL3 backend init failed.");
    }

    imgui_initialized_ = true;
}

void App::shutdown_imgui() {
    if (!imgui_initialized_) {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    imgui_initialized_ = false;
}

void App::init_assets() {
    shader_program_ = std::make_unique<ShaderProgram>(
        std::filesystem::path("shaders/directional.vert"),
        std::filesystem::path("shaders/point_or_directional.frag")
    );

    box_texture_ = std::make_unique<Texture>(
    std::filesystem::path("textures/box.jpg")
    );

    bunny_texture_ = std::make_unique<Texture>(
    std::filesystem::path("textures/metal.jpg")
    );

    teapot_texture_ = std::make_unique<Texture>(
    std::filesystem::path("textures/lava.jpg")
    );

    plane_texture_ = std::make_unique<Texture>(
    std::filesystem::path("textures/grass.jpg")
    );


    box_model_ = std::make_unique<Model>(
    "assets/cube_triangles_vnt.obj"
    );

    bunny_model_ = std::make_unique<Model>(
    "assets/bunny_tri_vnt.obj"
    );

    teapot_model_ = std::make_unique<Model>(
    "assets/teapot_tri_vnt.obj"
    );

    plane_model_ = std::make_unique<Model>(
    "assets/plane_tri_vnt.obj"
    );
    camera_.open(1);

    if (camera_.isOpened()) {
       cv::Mat frame;
       camera_ >> frame;

    if (!frame.empty()) {
        camera_texture_ = std::make_unique<Texture>(frame);
        camera_ready_ = true;
        std::cout << "Camera texture initialized.\n";
    }
    } else {
    std::cout << "Camera not available.\n";
    }

    microphone_ready_ = AudioManager::getInstance().initMicrophone();

     if (microphone_ready_) {
      std::cout << "Microphone initialized.\n";
      } else {
      std::cout << "Microphone not available.\n";
    }

    AudioManager& audio = AudioManager::getInstance();

    audio.load("ambient", "sounds/ambient.wav");
    audio.load("box", "sounds/box.wav");
    audio.load("bunny", "sounds/bunny.wav");
    audio.load("teapot", "sounds/teapot.wav");

    audio.playBGM("ambient", 0.08f);
}

void App::set_vsync(bool enabled) {
    vsync_on_ = enabled;
    glfwSwapInterval(vsync_on_ ? 1 : 0);
}

void App::toggle_vsync() {
    set_vsync(!vsync_on_);
    update_title();
    std::cout << "VSync: " << (vsync_on_ ? "ON" : "OFF") << '\n';
}

void App::toggle_antialiasing() {
    antialiasing_on_ = !antialiasing_on_;

    if (antialiasing_on_) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }

    std::cout << "Antialiasing: "
              << (antialiasing_on_ ? "ON" : "OFF")
              << '\n';
}

void App::toggle_pause() {
    paused_ = !paused_;
    std::cout << "Animation: " << (paused_ ? "PAUSED" : "RUNNING") << '\n';
}

void App::capture_cursor() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    cursor_captured_ = true;
    esc_releases_cursor_armed_ = true;
}

void App::release_cursor() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    cursor_captured_ = false;
}

void App::toggle_fullscreen() {
    fullscreen_ = !fullscreen_;

    if (fullscreen_) {
        glfwGetWindowPos(window_, &windowed_x_, &windowed_y_);
        glfwGetWindowSize(window_, &windowed_w_, &windowed_h_);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor == nullptr) {
            fullscreen_ = false;
            return;
        }

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (mode == nullptr) {
            fullscreen_ = false;
            return;
        }

        glfwSetWindowMonitor(
            window_,
            monitor,
            0,
            0,
            mode->width,
            mode->height,
            mode->refreshRate
        );
    } else {
        glfwSetWindowMonitor(
            window_,
            nullptr,
            windowed_x_,
            windowed_y_,
            windowed_w_,
            windowed_h_,
            0
        );
    }

    update_title();
}

void App::change_clear_brightness(float delta) {
    clear_color_.r = std::clamp(clear_color_.r + delta, 0.0f, 1.0f);
    clear_color_.g = std::clamp(clear_color_.g + delta, 0.0f, 1.0f);
    clear_color_.b = std::clamp(clear_color_.b + delta, 0.0f, 1.0f);
}

void App::update_title() {
    std::ostringstream title;
    title << config_.title
          << " | FPS: " << static_cast<int>(std::round(fps_.get()))
          << " | VSync: " << (vsync_on_ ? "ON" : "OFF")
          << " | " << (fullscreen_ ? "Fullscreen" : "Windowed");

    glfwSetWindowTitle(window_, title.str().c_str());
}

void App::save_screenshot() {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);

    std::vector<unsigned char> pixels(width * height * 3);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(
        0,
        0,
        width,
        height,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );

    std::ofstream file("screenshot.ppm", std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";

    for (int y = height - 1; y >= 0; --y) {
        file.write(
            reinterpret_cast<char*>(pixels.data() + y * width * 3),
            width * 3
        );
    }

    std::cout << "Screenshot saved as screenshot.ppm\n";
}

void App::update_projection_matrix() {
    const float aspect =
        static_cast<float>(config_.width) /
        static_cast<float>(config_.height);

    projection_matrix_ = glm::perspective(
        glm::radians(fov_degrees_),
        aspect,
        near_plane_,
        far_plane_
    );
}

void App::render_frame() {
    const float current_time = static_cast<float>(glfwGetTime());
    const float delta_time = current_time - last_frame_time_;
    last_frame_time_ = current_time;

    if (!paused_) {
    virtual_time_ += delta_time;
    }

    const float t = virtual_time_;

    object_color_.r = 0.5f + 0.5f * std::sin(t);
    object_color_.g = 0.5f + 0.5f * std::sin(t + 2.0943951f);
    object_color_.b = 0.5f + 0.5f * std::sin(t + 4.1887902f);

    glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_program_->use();

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::rotate(
    model_matrix,
    t,
    glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    model_matrix = glm::scale(
    model_matrix,
    glm::vec3(1.0f)
    );

    process_camera_input(delta_time);

    AudioManager::getInstance().setListenerPosition(
    camera_position_.x,
    camera_position_.y,
    camera_position_.z,

    camera_front_.x,
    camera_front_.y,
    camera_front_.z
    );

    glm::mat4 view_matrix = glm::lookAt(
    camera_position_,
    camera_position_ + camera_front_,
    camera_up_
    );

   shader_program_->setUniform("uM_m", model_matrix);
   shader_program_->setUniform("uV_m", view_matrix);
   shader_program_->setUniform("uP_m", projection_matrix_);

   std::vector<glm::vec3> point_light_positions = {
    glm::vec3(2.0f * std::sin(t), 1.2f, 2.0f * std::cos(t)),
    glm::vec3(2.0f * std::sin(t + 2.094f), -0.4f, 2.0f * std::cos(t + 2.094f)),
    glm::vec3(2.0f * std::sin(t + 4.188f), 0.8f, 2.0f * std::cos(t + 4.188f))
    };

   std::vector<glm::vec3> point_light_colors = {
    glm::vec3(1.0f, 0.2f, 0.2f),
    glm::vec3(0.2f, 1.0f, 0.2f),
    glm::vec3(0.2f, 0.2f, 1.0f)
    };

   shader_program_->setUniform("point_light_positions", point_light_positions);
   shader_program_->setUniform("point_light_colors", point_light_colors);

    glm::vec3 camera_position = glm::vec3(0.0f, 1.0f, 5.0f);
    glm::vec3 camera_target = glm::vec3(0.0f, -0.25f, 0.0f);

    glm::vec3 spot_position = glm::vec3(view_matrix * glm::vec4(camera_position, 1.0f));
    glm::vec3 spot_direction = glm::normalize(
    glm::vec3(view_matrix * glm::vec4(camera_target - camera_position, 0.0f))
    );

   shader_program_->setUniform("spot_position", spot_position);
   shader_program_->setUniform("spot_direction", spot_direction);
   shader_program_->setUniform("spot_color", glm::vec3(1.0f, 1.0f, 0.85f));
   shader_program_->setUniform("spot_cutoff", 0.75f);

   shader_program_->setUniform("ambient_intensity", glm::vec3(0.15f, 0.15f, 0.15f));
   shader_program_->setUniform("specular_material", glm::vec3(1.0f, 1.0f, 1.0f));
   shader_program_->setUniform("specular_shinines", 32.0f);


    auto draw_model = [&](Model* model, Texture* texture, const glm::mat4& model_matrix) {
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    shader_program_->setUniform("tex0", 0);

    shader_program_->setUniform("uM_m", model_matrix);
    shader_program_->setUniform("uV_m", view_matrix);
    shader_program_->setUniform("uP_m", projection_matrix_);

    model->draw();
    };

    glm::mat4 plane_matrix = glm::mat4(1.0f);
    plane_matrix = glm::translate(plane_matrix, glm::vec3(0.0f, -0.75f, 0.0f));
    plane_matrix = glm::scale(plane_matrix, glm::vec3(4.0f, 1.0f, 4.0f));

    glm::mat4 box_matrix = glm::mat4(1.0f);
    box_matrix = glm::translate(box_matrix, glm::vec3(0.0f, -0.25f, 0.0f));
    box_matrix = glm::rotate(box_matrix, t, glm::vec3(0.0f, 1.0f, 0.0f));
    box_matrix = glm::scale(box_matrix, glm::vec3(0.35f));

    glm::mat4 bunny_matrix = glm::mat4(1.0f);

    float bunny_jump = std::abs(std::sin(t * 3.0f)) * 0.25f;

    bunny_matrix = glm::translate(
    bunny_matrix,
    glm::vec3(-1.2f, -0.55f + bunny_jump, 0.0f)
    );

    bunny_matrix = glm::rotate(
    bunny_matrix,
    0.15f * std::sin(t * 6.0f),
    glm::vec3(1.0f, 0.0f, 0.0f)
    );

    bunny_matrix = glm::scale(bunny_matrix, glm::vec3(0.065f));

    glm::mat4 teapot_matrix = glm::mat4(1.0f);
    teapot_matrix = glm::translate(
    teapot_matrix,
    glm::vec3(1.3f, -0.35f + 0.12f * std::sin(t * 1.5f), 0.0f)
    );
    teapot_matrix = glm::rotate(teapot_matrix, t * 1.2f, glm::vec3(0.0f, 1.0f, 0.0f));
    teapot_matrix = glm::scale(teapot_matrix, glm::vec3(0.075f));

    glm::vec3 box_pos = glm::vec3(0.0f, -0.25f, 0.0f);
    glm::vec3 bunny_pos = glm::vec3(-1.2f, -0.55f + bunny_jump, 0.0f);
    glm::vec3 teapot_pos = glm::vec3(
    1.3f,
    -0.35f + 0.12f * std::sin(t * 1.5f),
    0.0f
    );

    AudioManager& audio = AudioManager::getInstance();

    if (t - last_box_sound_time_ > 4.0f) {
    audio.play3D("box", box_pos.x, box_pos.y, box_pos.z, 0.7f);
    last_box_sound_time_ = t;
    }

    if (t - last_bunny_sound_time_ > 2.0f) {
    audio.play3D("bunny", bunny_pos.x, bunny_pos.y, bunny_pos.z, 0.8f);
    last_bunny_sound_time_ = t;
    }

    if (t - last_teapot_sound_time_ > 3.0f) {
    audio.play3D("teapot", teapot_pos.x, teapot_pos.y, teapot_pos.z, 0.8f);
    last_teapot_sound_time_ = t;
    }

    draw_model(plane_model_.get(), plane_texture_.get(), plane_matrix);
    draw_model(box_model_.get(), box_texture_.get(), box_matrix);
    draw_model(bunny_model_.get(), bunny_texture_.get(), bunny_matrix);
    draw_model(teapot_model_.get(), teapot_texture_.get(), teapot_matrix);
}

void App::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        fps_.update();
        if (fps_.is_updated()) {
            update_title();
        }

        render_frame();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("App status");
        ImGui::Text("FPS: %.1f", fps_.get());
        ImGui::Text("GL version: %s", gl_version_text_.c_str());
        ImGui::Text("GL profile: %s", gl_profile_text_.c_str());

        if (microphone_ready_) {
            float mic_loudness = AudioManager::getInstance().getMicLoudness();
            ImGui::Text("Mic loudness: %.3f", mic_loudness);
            ImGui::ProgressBar(mic_loudness, ImVec2(0.0f, 0.0f));
        } else {
            ImGui::Text("Mic: not available");
        }

        ImGui::Text("VSync: %s", vsync_on_ ? "ON" : "OFF");
        ImGui::Text("AA: %s", antialiasing_on_ ? "ON" : "OFF");
        ImGui::Text("Mode: %s", fullscreen_ ? "Fullscreen" : "Windowed");
        ImGui::Text("Cursor: %s", cursor_captured_ ? "Captured" : "Released");

        ImGui::Separator();
        ImGui::Text("Camera position:");
        ImGui::Text("X: %.2f", camera_position_.x);
        ImGui::Text("Y: %.2f", camera_position_.y);
        ImGui::Text("Z: %.2f", camera_position_.z);

        ImGui::Separator();
        ImGui::Text("Scene:");
        ImGui::Text("Models: 4");
        ImGui::Text("Textures: 4");
        ImGui::Text("Lights: directional + 3 point + spotlight");
        ImGui::Text("Collisions: ON");
        ImGui::Text("3D Audio sources: box, bunny, teapot");
        ImGui::Text("Background audio: ON");

        ImGui::Separator();
        ImGui::Text("Controls:");
        ImGui::BulletText("W / A / S / D - move camera");
        ImGui::BulletText("Mouse - look around");
        ImGui::BulletText("V - toggle VSync");
        ImGui::BulletText("M - toggle antialiasing");
        ImGui::BulletText("L - toggle fullscreen");
        ImGui::BulletText("P - screenshot");
        ImGui::BulletText("SPACE - pause animation");
        ImGui::BulletText("Left mouse - capture cursor");
        ImGui::BulletText("Right mouse / TAB - release cursor");
        ImGui::BulletText("ESC - release first, close second");
        ImGui::BulletText("R / G / B / C - background presets");
        ImGui::BulletText("Mouse wheel - change background brightness");
        ImGui::End();

        if (camera_ready_) {
            float now = static_cast<float>(glfwGetTime());

            if (now - last_camera_update_time_ > 0.10f) {
                cv::Mat frame;
                camera_ >> frame;

                if (!frame.empty()) {
                    cv::flip(frame, frame, 0);
                    camera_texture_->replace_image(frame);
                }

                last_camera_update_time_ = now;
            }

            ImGui::Begin("Camera preview");
            ImGui::Image(
                (ImTextureID)(intptr_t)camera_texture_->get_name(),
                ImVec2(
                    static_cast<float>(camera_texture_->get_width()) * 0.35f,
                    static_cast<float>(camera_texture_->get_height()) * 0.35f
                )
            );
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }
}

bool App::collides_with_scene(const glm::vec3& pos) const {
    struct SphereCollider {
        glm::vec3 center;
        float radius;
    };

    const SphereCollider colliders[] = {
        { glm::vec3(0.0f, -0.25f, 0.0f), 0.75f },   // box
        { glm::vec3(-1.2f, -0.55f, 0.0f), 0.55f },  // bunny
        { glm::vec3(1.3f, -0.35f, 0.0f), 0.55f }    // teapot
    };

    for (const auto& c : colliders) {
        glm::vec2 playerXZ(pos.x, pos.z);
        glm::vec2 objectXZ(c.center.x, c.center.z);

        if (glm::length(playerXZ - objectXZ) < c.radius) {
            return true;
        }
    }

    return false;
}

void App::process_camera_input(float delta_time) {
    const float velocity = camera_speed_ * delta_time;
    glm::vec3 old_position = camera_position_;

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        camera_position_ += camera_front_ * velocity;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        camera_position_ -= camera_front_ * velocity;
    }

    glm::vec3 right = glm::normalize(glm::cross(camera_front_, camera_up_));

    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        camera_position_ -= right * velocity;
    }

    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        camera_position_ += right * velocity;
    }

    camera_position_.x = std::clamp(camera_position_.x, -4.0f, 4.0f);
    camera_position_.z = std::clamp(camera_position_.z, -4.0f, 4.0f);

    if (collides_with_scene(camera_position_)) {
    camera_position_ = old_position;
    }
}

void App::handle_mouse_motion(double xpos, double ypos) {
    if (first_mouse_) {
        last_mouse_x_ = xpos;
        last_mouse_y_ = ypos;
        first_mouse_ = false;
    }

    float xoffset = static_cast<float>(xpos - last_mouse_x_);
    float yoffset = static_cast<float>(last_mouse_y_ - ypos);

    last_mouse_x_ = xpos;
    last_mouse_y_ = ypos;

    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    camera_yaw_ += xoffset;
    camera_pitch_ += yoffset;

    camera_pitch_ = std::clamp(camera_pitch_, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = std::cos(glm::radians(camera_yaw_)) * std::cos(glm::radians(camera_pitch_));
    front.y = std::sin(glm::radians(camera_pitch_));
    front.z = std::sin(glm::radians(camera_yaw_)) * std::cos(glm::radians(camera_pitch_));

    camera_front_ = glm::normalize(front);
}

App* App::from_window(GLFWwindow* window) {
    return static_cast<App*>(glfwGetWindowUserPointer(window));
}