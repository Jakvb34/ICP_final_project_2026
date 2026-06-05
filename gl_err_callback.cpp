#include "gl_err_callback.h"

#include <iostream>

void GLAPIENTRY MessageCallback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei /*length*/,
                               const GLchar* message,
                               const void* /*userParam*/) {
    const char* src_str = "Unknown";
    switch (source) {
        case GL_DEBUG_SOURCE_API: src_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src_str = "WINDOW SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: src_str = "SHADER COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: src_str = "THIRD PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION: src_str = "APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER: src_str = "OTHER"; break;
        default: break;
    }

    const char* type_str = "Unknown";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: type_str = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "UNDEFINED BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY: type_str = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: type_str = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_MARKER: type_str = "MARKER"; break;
        case GL_DEBUG_TYPE_OTHER: type_str = "OTHER"; break;
        default: break;
    }

    const char* severity_str = "Unknown";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: severity_str = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW: severity_str = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "NOTIFICATION"; break;
        default: break;
    }

    std::cerr << "[GL CALLBACK] source=" << src_str
              << ", type=" << type_str
              << ", severity=" << severity_str
              << ", id=" << id
              << ", message=" << message << '\n';
}
