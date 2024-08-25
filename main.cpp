#include <iostream>
#include <time.h>
#include <SDL.h>

#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"
#include "resourcesMaster.h"
#include "sequencer.h"

#include "headers/deck.h"
#include "headers/UI.h"

int main(int args, char* argsc[])
{
    //delete ptr;
    const int screenWidth = 640;
    const int screenHeight = 640;

    srand(time(NULL));

    ResourcesConfig::loadConfig();

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

    GLContext::init(screenWidth,screenHeight);

    glEnable(GL_MULTISAMPLE);

    SDL_StopTextInput();
    ViewPort::init(screenWidth,screenHeight);

    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);
    SDL_Event e;
    bool quit = false;
    glClearColor(0,0,0,1);
    bool eventsEmpty = true;

    //Callable* ptr = (new SequenceUnit([](int runtime,Callable&){}));

    /*RenderCamera camera;
    ViewPort::setZRange((0.1,100);
    camera.init({0,0,CAMERA_Z});
    ViewPort::currentCamera = &camera;*/
    Deck deck;
    Hand hand;
    hand.draw(deck,5);
    HandUI handUI;
    handUI.resetHand(hand.getHand());
    auto asdf = hand.draw(deck,5);
    handUI.drawCards(asdf);
    glm::vec4 handRect = GameUI::getHandRect();
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            eventsEmpty = false;
            KeyManager::update(e);
            MouseManager::update(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        if (eventsEmpty)
        {
            KeyManager::update(e);
            MouseManager::update(e);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handUI.update();
        PolyRender::requestRect(handRect,glm::vec4(0,0.7,0,1),false,0,0);

        SequenceManager::run();

        ViewPort::update();
        PolyRender::render();

        SpriteManager::render();
        GLContext::update();
        eventsEmpty = true;
        DeltaTime::update();
        SDL_Delay(std::max(0,10 - DeltaTime::deltaTime));

    }
    return 0;
}
