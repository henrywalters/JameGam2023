//
// Created by henry on 3/15/23.
//

#ifndef JAMEGAM_ASSETS_HPP
#define JAMEGAM_ASSETS_HPP

#include <string>

#include "hagame/utils/file.h"
#include "hagame/utils/store.h"
#include "hagame/graphics/shaderProgram.h"
#include "hagame/graphics/texture.h"
#include "hagame/graphics/font.h"

#ifdef __EMSCRIPTEN__
const std::string ASSET_DIR = "./assets/";
#else
const std::string ASSET_DIR = "../assets/";
#endif

static hg::utils::Store<std::shared_ptr<hg::graphics::ShaderProgram>> SHADERS;
static hg::utils::Store<std::shared_ptr<hg::graphics::Texture>> TEXTURES;
static hg::utils::Store<std::shared_ptr<hg::graphics::Font>> FONTS;


std::shared_ptr<hg::graphics::ShaderProgram> loadShader(std::string name, std::string vertPath, std::string fragPath) {
    auto vertSrc = hg::utils::f_read(ASSET_DIR + vertPath);
    auto fragSrc = hg::utils::f_read(ASSET_DIR + fragPath);

    auto shader = std::make_shared<hg::graphics::ShaderProgram>(
            name,
            hg::graphics::Shader::LoadVertex(vertSrc),
            hg::graphics::Shader::LoadFragment(fragSrc)
    );

    SHADERS.set(name, shader);

    return shader;
}

std::shared_ptr<hg::graphics::Texture> loadTexture(std::string name, std::string path) {
    auto texture = std::make_shared<hg::graphics::Texture>(ASSET_DIR + path);
    TEXTURES.set(name, texture);
    return texture;
}

std::shared_ptr<hg::graphics::Font> loadFont(std::string name, std::string path) {
    auto font = std::make_shared<hg::graphics::Font>( ASSET_DIR + path);
    FONTS.set(name, font);
    return font;
}

#endif
