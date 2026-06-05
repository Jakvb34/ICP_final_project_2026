#pragma once

#include "fps_meter.hpp"
#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include <opencv2/opencv.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <chrono>
#include <memory>
#include <string>
#include "AudioManager.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_error_callback(int error, const char* description);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

class App {
public:
    App();
    ~App();

    bool init();
    void run();

    static App* from_window(GLFWwindow* window);

    void toggle_vsync();
    void set_vsync(bool enabled);

    void toggle_fullscreen();
    void capture_cursor();
    void release_cursor();

    void update_title();
    void render_frame();

    void update_projection_matrix();

    GLFWwindow* window() const { return window_; }

    bool cursor_captured() const { return cursor_captured_; }
    bool fullscreen() const { return fullscreen_; }

    void set_clear_color(float r, float g, float b, float a = 1.0f) {
        clear_color_ = {r, g, b, a};
    }

    void change_clear_brightness(float delta);

    void save_screenshot();

    void toggle_pause();
    bool paused() const { return paused_; }

    void process_camera_input(float delta_time);
    void handle_mouse_motion(double xpos, double ypos);

    void toggle_antialiasing();

private:
    struct Config {
        int width = 1280;
        int height = 720;
        std::string title = "ICP Final OpenGL";
        bool vsync_on = true;
        double fps_update_interval = 0.25;
    };

    struct Color4 {
        float r = 0.1f;
        float g = 0.12f;
        float b = 0.16f;
        float a = 1.0f;
    };

    bool load_config();
    void setup_debug_callback() const;
    void init_assets();
    void init_imgui();
    void shutdown_imgui();
    bool collides_with_scene(const glm::vec3& pos) const;
    

private:
    GLFWwindow* window_ = nullptr;

    Config config_{};

    std::unique_ptr<ShaderProgram> shader_program_;

    std::unique_ptr<Model> box_model_;
    std::unique_ptr<Model> bunny_model_;
    std::unique_ptr<Model> teapot_model_;
    std::unique_ptr<Model> plane_model_;

    std::unique_ptr<Texture> box_texture_;
    std::unique_ptr<Texture> bunny_texture_;
    std::unique_ptr<Texture> teapot_texture_;
    std::unique_ptr<Texture> plane_texture_;

    std::string gl_version_text_ = "unknown";
    std::string gl_profile_text_ = "unknown";

    cv::VideoCapture camera_;
    std::unique_ptr<Texture> camera_texture_;
    bool camera_ready_ = false;

    glm::mat4 projection_matrix_{1.0f};
    float fov_degrees_ = 45.0f;
    float near_plane_ = 0.1f;
    float far_plane_ = 100.0f;

    fps_meter fps_{};
    bool vsync_on_ = true;

    Color4 clear_color_{};
    Color4 object_color_{1.0f, 0.4f, 0.2f, 1.0f};

    bool imgui_initialized_ = false;

    bool cursor_captured_ = false;
    bool fullscreen_ = false;
    bool esc_releases_cursor_armed_ = true;

    int windowed_x_ = 100;
    int windowed_y_ = 100;
    int windowed_w_ = 1280;
    int windowed_h_ = 720;

    bool paused_ = false;
    float virtual_time_ = 0.0f;
    float last_frame_time_ = 0.0f;

    bool microphone_ready_ = false;

    glm::vec3 camera_position_{0.0f, 0.2f, 4.0f};
    glm::vec3 camera_front_{0.0f, -0.1f, -1.0f};
    glm::vec3 camera_up_{0.0f, 1.0f, 0.0f};

    float camera_yaw_ = -90.0f;
    float camera_pitch_ = -5.0f;
    float camera_speed_ = 3.0f;
    float mouse_sensitivity_ = 0.1f;

    bool first_mouse_ = true;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;

    float last_box_sound_time_ = 0.0f;
    float last_bunny_sound_time_ = 0.0f;
    float last_teapot_sound_time_ = 0.0f;
    float last_camera_update_time_ = 0.0f;

    bool antialiasing_on_ = true;
};