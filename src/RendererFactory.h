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
            default:
                obj = make_shared<baseRenderer>(); 
                break;
        }
        obj->setup();
        return obj;
    };
};


