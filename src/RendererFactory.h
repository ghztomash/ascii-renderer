#pragma once

#include "baseRenderer.h"
#include "implRenderer.h"

class RendererFactory {
    public:
    static shared_ptr<baseRenderer> newRenderer(RendererType type) {
        shared_ptr<baseRenderer> obj;

        switch (type) {
            case RECT_RENDERER:
                {
                    shared_ptr<rectRenderer> obj;
                    obj = make_shared<rectRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case CIRCLE_RENDERER:
                {
                    shared_ptr<circRenderer> obj;
                    obj = make_shared<circRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
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
            case CUBE_RENDERER:
                {
                    shared_ptr<cubeRenderer> obj;
                    obj = make_shared<cubeRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case SPHERE_RENDERER:
                {
                    shared_ptr<sphereRenderer> obj;
                    obj = make_shared<sphereRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case CYLINDER_RENDERER:
                {
                    shared_ptr<cylinderRenderer> obj;
                    obj = make_shared<cylinderRenderer>();
                    obj->setup();
                    return obj;
                    break;
                }
            case CONE_RENDERER:
                {
                    shared_ptr<coneRenderer> obj;
                    obj = make_shared<coneRenderer>();
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
                obj = make_shared<baseRenderer>();
                break;
        }
        obj->setup();
        return obj;
    };
};
