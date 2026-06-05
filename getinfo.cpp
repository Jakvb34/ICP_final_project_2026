#include "getinfo.hpp"

#include <iostream>
#include <stdexcept>

namespace {
const char* safe_gl_string(GLenum name) {
    const auto* value = reinterpret_cast<const char*>(glGetString(name));
    return value != nullptr ? value : "<Unknown>";
}
}

void print_gl_context_info() {
    std::cout << "GL_VENDOR: " << safe_gl_string(GL_VENDOR) << '\n';
    std::cout << "GL_RENDERER: " << safe_gl_string(GL_RENDERER) << '\n';
    std::cout << "GL_VERSION: " << safe_gl_string(GL_VERSION) << '\n';
    std::cout << "GLSL_VERSION: " << safe_gl_string(GL_SHADING_LANGUAGE_VERSION) << '\n';

    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "Numeric version: " << major << '.' << minor << '\n';

    if (major < 4 || (major == 4 && minor < 1)) {
    throw std::runtime_error("OpenGL 4.1 or newer is required.");
  }

    GLint profile_mask = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);
    if ((profile_mask & GL_CONTEXT_CORE_PROFILE_BIT) != 0) {
        std::cout << "Profile: CORE\n";
    } else if ((profile_mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0) {
        std::cout << "Profile: COMPATIBILITY\n";
    } else {
        std::cout << "Profile: <Unknown>\n";
    }

    GLint context_flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    std::cout << "Context flags:";
    if ((context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) != 0) {
        std::cout << " FORWARD_COMPATIBLE";
    }
    if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
        std::cout << " DEBUG";
    }
#ifdef GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
    if ((context_flags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT) != 0) {
        std::cout << " ROBUST_ACCESS";
    }
#endif
#ifdef GL_CONTEXT_FLAG_NO_ERROR_BIT
    if ((context_flags & GL_CONTEXT_FLAG_NO_ERROR_BIT) != 0) {
        std::cout << " NO_ERROR";
    }
#endif
    if (context_flags == 0) {
        std::cout << " <none>";
    }
    std::cout << '\n';
}
