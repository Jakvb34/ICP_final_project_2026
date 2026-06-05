#include "app.hpp"

#include <cstdlib>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    if (App* app = App::from_window(window)) {
        app->change_clear_brightness(static_cast<float>(yoffset) * 0.03f);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;

    if (App* app = App::from_window(window)) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            app->capture_cursor();
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            app->release_cursor();
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (App* app = App::from_window(window)) {
        if (app->cursor_captured()) {
            app->handle_mouse_motion(xpos, ypos);
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    App* app = App::from_window(window);
    if (app == nullptr) {
        return;
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (app->cursor_captured()) {
                app->release_cursor();
            } else {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;

        case GLFW_KEY_TAB:
            app->release_cursor();
            break;

        case GLFW_KEY_V:
            if (action == GLFW_PRESS) {
                app->toggle_vsync();
            }
            break;

        case GLFW_KEY_M:
            if (action == GLFW_PRESS) {
                app->toggle_antialiasing();
            }
            break;

        case GLFW_KEY_P:
             if (action == GLFW_PRESS) {
                app->save_screenshot();
            }
            break;

        case GLFW_KEY_SPACE:
             if (action == GLFW_PRESS) {
                app->toggle_pause();
            }
            break;

        case GLFW_KEY_L:
            if (action == GLFW_PRESS) {
                app->toggle_fullscreen();
            }
            break;

        case GLFW_KEY_R:
            app->set_clear_color(0.7f, 0.15f, 0.15f, 1.0f);
            break;

        case GLFW_KEY_G:
            app->set_clear_color(0.15f, 0.6f, 0.2f, 1.0f);
            break;

        case GLFW_KEY_B:
            app->set_clear_color(0.15f, 0.25f, 0.7f, 1.0f);
            break;

        case GLFW_KEY_C:
            app->set_clear_color(0.1f, 0.12f, 0.16f, 1.0f);
            break;

        default:
            break;
    }
}

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}