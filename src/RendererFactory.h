#pragma once
#include "ImplRenderer.h"

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
};

const vector<std::string> RENDERER_NAMES = {"circmouse", "circwaves",
                                            "noise", "lua",
                                            "dotype-g", "dotype-h", "dotype-k", "dotype-l",
                                            "dotype-n0", "dotype-n1", "dotype-n2", "dotype-n3", "dotype-n8"};

class RendererFactory {
    public:
    static shared_ptr<BaseRenderer> newRenderer(rendererType type) {
        shared_ptr<BaseRenderer> obj;

        switch (type) {
            case CIRC_MOUSE_RENDERER:
                {
                    shared_ptr<circMouseRenderer> obj;
                    obj = make_shared<circMouseRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case CIRC_WAVES_RENDERER:
                {
                    shared_ptr<circWavesRenderer> obj;
                    obj = make_shared<circWavesRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case NOISE_RENDERER:
                {
                    shared_ptr<noiseRenderer> obj;
                    obj = make_shared<noiseRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case LUA_RENDERER:
                {
                    shared_ptr<luaRenderer> obj;
                    obj = make_shared<luaRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_G_RENDERER:
                {
                    shared_ptr<doTypeGRenderer> obj =
                        make_shared<doTypeGRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_H_RENDERER:
                {
                    shared_ptr<doTypeHRenderer> obj =
                        make_shared<doTypeHRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_K_RENDERER:
                {
                    shared_ptr<doTypeKRenderer> obj =
                        make_shared<doTypeKRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_L_RENDERER:
                {
                    shared_ptr<doTypeLRenderer> obj =
                        make_shared<doTypeLRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_N0_RENDERER:
                {
                    shared_ptr<doTypeN0Renderer> obj =
                        make_shared<doTypeN0Renderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_N1_RENDERER:
                {
                    shared_ptr<doTypeN1Renderer> obj =
                        make_shared<doTypeN1Renderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_N2_RENDERER:
                {
                    shared_ptr<doTypeN2Renderer> obj =
                        make_shared<doTypeN2Renderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_N3_RENDERER:
                {
                    shared_ptr<doTypeN3Renderer> obj =
                        make_shared<doTypeN3Renderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case DOTYPE_N8_RENDERER:
                {
                    shared_ptr<doTypeN8Renderer> obj =
                        make_shared<doTypeN8Renderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            default:
                obj = make_shared<BaseRenderer>();
                break;
        }
        obj->setup();
        return obj;
    };
};
