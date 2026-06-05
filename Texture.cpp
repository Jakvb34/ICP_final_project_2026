#include "Texture.hpp"
#include <opencv2/imgproc.hpp>

cv::Mat Texture::load_image(const std::filesystem::path& path) {
    cv::Mat image = cv::imread(path.string(), cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        throw std::runtime_error{
            std::string("no texture in file: ").append(path.string())
        };
    }

    if (image.depth() != CV_8U) {
        image.convertTo(image, CV_8U);
    }

    if (image.channels() == 1) {
        return image;
    }

    if (image.channels() == 3) {
        return image;
    }

    if (image.channels() == 4) {
        return image;
    }

    cv::Mat converted;
    cv::cvtColor(image, converted, cv::COLOR_BGR2RGB);
    return converted;
}

Texture::Texture(const std::filesystem::path& path, Interpolation interpolation)
    : Texture{load_image(path), interpolation} {}

Texture::Texture(const glm::vec3& vec)
    : Texture{
          cv::Mat{1, 1, CV_8UC3, cv::Scalar{vec.b, vec.g, vec.r}},
          Interpolation::nearest
      } {}

Texture::Texture(const glm::vec4& vec)
    : Texture{
          cv::Mat{1, 1, CV_8UC4, cv::Scalar{vec.b, vec.g, vec.r, vec.a}},
          Interpolation::nearest
      } {}

Texture::Texture(cv::Mat const& input_image, Interpolation interpolation) {
    if (input_image.empty()) {
        throw std::runtime_error{"the input image is empty"};
    }

    cv::Mat image = input_image.clone();
    cv::flip(image, image, 0);

    glGenTextures(1, &name_);
    glBindTexture(GL_TEXTURE_2D, name_);

    switch (image.type()) {
        case CV_8UC1:
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R8,
                image.cols,
                image.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                image.data
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
            break;

        case CV_8UC3:
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB8,
                image.cols,
                image.rows,
                0,
                GL_BGR,
                GL_UNSIGNED_BYTE,
                image.data
            );
            break;

        case CV_8UC4:
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                image.cols,
                image.rows,
                0,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                image.data
            );
            break;

        default:
            throw std::runtime_error{
                "unsupported number of channels or channel depth in texture"
            };
    }

    set_interpolation(interpolation);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    if (name_ != 0) {
        glDeleteTextures(1, &name_);
        name_ = 0;
    }
}

GLuint Texture::get_name() const {
    return name_;
}

void Texture::bind(void) {
    glBindTexture(GL_TEXTURE_2D, name_);
}

void Texture::set_interpolation(Interpolation interpolation) {
    glBindTexture(GL_TEXTURE_2D, name_);

    switch (interpolation) {
        case Interpolation::nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;

        case Interpolation::linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;

        case Interpolation::linear_mipmap_linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_LINEAR
            );
            glGenerateMipmap(GL_TEXTURE_2D);
            break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::get_height(void) {
    glBindTexture(GL_TEXTURE_2D, name_);

    int tex_height = 0;
    glGetTexLevelParameteriv(
        GL_TEXTURE_2D,
        0,
        GL_TEXTURE_HEIGHT,
        &tex_height
    );

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex_height;
}

int Texture::get_width(void) {
    glBindTexture(GL_TEXTURE_2D, name_);

    int tex_width = 0;
    glGetTexLevelParameteriv(
        GL_TEXTURE_2D,
        0,
        GL_TEXTURE_WIDTH,
        &tex_width
    );

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex_width;
}

void Texture::replace_image(const cv::Mat& input_image) {
    if (input_image.empty()) {
        throw std::runtime_error{"replacement image is empty"};
    }

    cv::Mat image = input_image.clone();
    cv::flip(image, image, 0);

    if ((image.rows != get_height()) || (image.cols != get_width())) {
        throw std::runtime_error("improper image replacement size");
    }

    glBindTexture(GL_TEXTURE_2D, name_);

    GLint tex_format = 0;
    glGetTexLevelParameteriv(
        GL_TEXTURE_2D,
        0,
        GL_TEXTURE_INTERNAL_FORMAT,
        &tex_format
    );

    switch (image.type()) {
        case CV_8UC1:
            if (tex_format != GL_R8) {
                throw std::runtime_error(
                    "improper image replacement channel data, GL_R8 was the original"
                );
            }

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                image.cols,
                image.rows,
                GL_RED,
                GL_UNSIGNED_BYTE,
                image.data
            );
            break;

        case CV_8UC3:
            if (tex_format != GL_RGB8) {
                throw std::runtime_error(
                    "improper image replacement channel data, GL_RGB8 was the original"
                );
            }

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                image.cols,
                image.rows,
                GL_BGR,
                GL_UNSIGNED_BYTE,
                image.data
            );
            break;

        case CV_8UC4:
            if (tex_format != GL_RGBA8) {
                throw std::runtime_error(
                    "improper image replacement channel data, GL_RGBA8 was the original"
                );
            }

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                image.cols,
                image.rows,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                image.data
            );
            break;

        default:
            throw std::runtime_error{
                "unsupported number of channels or channel depth in texture"
            };
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}