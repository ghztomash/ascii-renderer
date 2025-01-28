#pragma once
#include "DaysOfTypeRenderer.h"
#include "ImplRenderer.h"
#include "LuaRenderer.h"
#include "ParticlesRenderer.h"
#include <optional>
#include <string>

enum rendererType {
    CIRC_MOUSE_RENDERER,
    CIRC_WAVES_RENDERER,
    NOISE_RENDERER,
    LUA_RENDERER,
    DOTYPE_G_RENDERER,
    DOTYPE_H_RENDERER,
    DOTYPE_K_RENDERER,
    DOTYPE_L_RENDERER,
    DOTYPE_N0_RENDERER,
    DOTYPE_N1_RENDERER,
    DOTYPE_N2_RENDERER,
    DOTYPE_N3_RENDERER,
    DOTYPE_N8_RENDERER,
    PARTICLES_RENDERER,
};

const vector<std::string> RENDERER_NAMES = {"circmouse", "circwaves",
                                            "noise", "lua",
                                            "dotype-g", "dotype-h", "dotype-k", "dotype-l",
                                            "dotype-n0", "dotype-n1", "dotype-n2", "dotype-n3", "dotype-n8", "particles"};

/// Generic function that creates a renderer
template <typename T>
std::shared_ptr<BaseRenderer> createRenderer(std::optional<std::string> customName = std::nullopt) {
    auto obj = std::make_shared<T>();

    if (customName) {
        obj->setup(*customName);
    } else {
        obj->setup();
    }

    return obj;
}

class RendererFactory {
    public:
    static shared_ptr<BaseRenderer> newRenderer(rendererType type, std::optional<std::string> customName = std::nullopt) {
        shared_ptr<BaseRenderer> obj;

        switch (type) {
            case CIRC_MOUSE_RENDERER:
                return createRenderer<circMouseRenderer>(customName);
            case CIRC_WAVES_RENDERER:
                return createRenderer<circWavesRenderer>(customName);
            case NOISE_RENDERER:
                return createRenderer<noiseRenderer>(customName);
            case LUA_RENDERER:
                return createRenderer<luaRenderer>(customName);
            case DOTYPE_G_RENDERER:
                return createRenderer<doTypeGRenderer>(customName);
            case DOTYPE_H_RENDERER:
                return createRenderer<doTypeHRenderer>(customName);
            case DOTYPE_K_RENDERER:
                return createRenderer<doTypeKRenderer>(customName);
            case DOTYPE_L_RENDERER:
                return createRenderer<doTypeLRenderer>(customName);
            case DOTYPE_N0_RENDERER:
                return createRenderer<doTypeN0Renderer>(customName);
            case DOTYPE_N1_RENDERER:
                return createRenderer<doTypeN1Renderer>(customName);
            case DOTYPE_N2_RENDERER:
                return createRenderer<doTypeN2Renderer>(customName);
            case DOTYPE_N3_RENDERER:
                return createRenderer<doTypeN3Renderer>(customName);
            case DOTYPE_N8_RENDERER:
                return createRenderer<doTypeN8Renderer>(customName);
            case PARTICLES_RENDERER:
                return createRenderer<particlesRenderer>(customName);
            default:
                return createRenderer<BaseRenderer>(customName);
        }
    };
};
