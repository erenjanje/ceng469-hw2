#pragma once

class Framebuffer {
    unsigned fbo = 0;
    unsigned texture = 0;
    unsigned depth_buffer = 0;
    unsigned sampler = 0;
    unsigned rbo;
    int w;
    int h;
public:
    Framebuffer();
    Framebuffer(const int width, const int height);

    int get_width() const;
    int get_height() const;
    void bind() const;
    void unbind() const;
    unsigned get_sampler() const;
    unsigned get_texture() const;
};

