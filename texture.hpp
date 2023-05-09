#pragma once

#include <string>

class Texture {
    unsigned texture = 0;
public:
    Texture();
    Texture(const std::string filename);
    void bind() const;
};
