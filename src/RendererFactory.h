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
std::shared_ptr<BaseRenderer> createRenderer(const std::string &typeName,
                                             std::optional<std::string> customName = std::nullopt) {
    auto obj = std::make_shared<T>();

    if (customName) {
        obj->setup(*customName);
    } else {
        obj->setup();
    }
    obj->setRendererTypeName(typeName);

    return obj;
}

class RendererFactory {
    public:
    static shared_ptr<BaseRenderer> newRenderer(rendererType type, std::optional<std::string> customName = std::nullopt) {
        shared_ptr<BaseRenderer> obj;

        switch (type) {
            case CIRC_MOUSE_RENDERER:
                return createRenderer<circMouseRenderer>(RENDERER_NAMES[CIRC_MOUSE_RENDERER], customName);
            case CIRC_WAVES_RENDERER:
                return createRenderer<circWavesRenderer>(RENDERER_NAMES[CIRC_WAVES_RENDERER], customName);
            case NOISE_RENDERER:
                return createRenderer<noiseRenderer>(RENDERER_NAMES[NOISE_RENDERER], customName);
            case LUA_RENDERER:
                return createRenderer<luaRenderer>(RENDERER_NAMES[LUA_RENDERER], customName);
            case DOTYPE_G_RENDERER:
                return createRenderer<doTypeGRenderer>(RENDERER_NAMES[DOTYPE_G_RENDERER], customName);
            case DOTYPE_H_RENDERER:
                return createRenderer<doTypeHRenderer>(RENDERER_NAMES[DOTYPE_H_RENDERER], customName);
            case DOTYPE_K_RENDERER:
                return createRenderer<doTypeKRenderer>(RENDERER_NAMES[DOTYPE_K_RENDERER], customName);
            case DOTYPE_L_RENDERER:
                return createRenderer<doTypeLRenderer>(RENDERER_NAMES[DOTYPE_L_RENDERER], customName);
            case DOTYPE_N0_RENDERER:
                return createRenderer<doTypeN0Renderer>(RENDERER_NAMES[DOTYPE_N0_RENDERER], customName);
            case DOTYPE_N1_RENDERER:
                return createRenderer<doTypeN1Renderer>(RENDERER_NAMES[DOTYPE_N1_RENDERER], customName);
            case DOTYPE_N2_RENDERER:
                return createRenderer<doTypeN2Renderer>(RENDERER_NAMES[DOTYPE_N2_RENDERER], customName);
            case DOTYPE_N3_RENDERER:
                return createRenderer<doTypeN3Renderer>(RENDERER_NAMES[DOTYPE_N3_RENDERER], customName);
            case DOTYPE_N8_RENDERER:
                return createRenderer<doTypeN8Renderer>(RENDERER_NAMES[DOTYPE_N8_RENDERER], customName);
            case PARTICLES_RENDERER:
                return createRenderer<particlesRenderer>(RENDERER_NAMES[PARTICLES_RENDERER], customName);
            default:
                return createRenderer<BaseRenderer>("base", customName);
        }
    };
};
