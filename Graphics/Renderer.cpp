#include "Renderer.hpp"

Renderer::Renderer(){
    cam = new Camera();
    WorldGen wg = WorldGen();


    // Generate world
    // ---------------------------
    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/1.87;
    float gain = 0.60;

    //for each pixel, get the value
    float period = 400;
    float amplitude = 20;
    for (int i = 0; i < 16; i++)
    {
        octaves[i] = period;
        scales[i] = amplitude;

        period *= lacunarity;
        amplitude *= gain;
    }

    int nOctaves = sizeof(octaves)/sizeof(float);
    world = wg.generateWorld(1000,1000,0.5f,octaves,scales,nOctaves);

}

void Renderer::repaint(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(skybox != NULL){
    	skybox->draw(cam->skyboxMatrix(),pMatrix);
    }

    glEnable(GL_DEPTH_TEST);


    for(Object * o : renderList){
        o->draw(cam->vMatrix,pMatrix);
    }

    glEnable(GL_CULL_FACE);
}
