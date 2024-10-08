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
#include "headers/loader.h"
#include "headers/gamestate.h"



int main(int args, char* argsc[])
{
    //delete ptr;
    const int screenWidth = 900;
    const int screenHeight = 900;

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

    FontGlobals::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);
    SDL_Event e;
    bool quit = false;
    glClearColor(0,0,0,1);
    bool eventsEmpty = true;

    GameState::init();

    CardUI::cardTextFont.reset((new CardTextFont()));
    CardUI::blankCard.init("sprites/cards/card_template.png");
    //Callable* ptr = (new SequenceUnit([](int runtime,Callable&){}));

    /*RenderCamera camera;
    ViewPort::setZRange((0.1,100);
    camera.init({0,0,CAMERA_Z});
    ViewPort::currentCamera = &camera;*/
    auto start = loadHand();
    for (auto card : start)
    {
        GameState::getGameState()->addCard(card,HAND);
    }
    //Hand hand(start);

    GameState::getGameState()->newTurn();

    EnemyState enemy(("sprites/card_back.png"));
    MasterCardsUI::init();

   // auto asdf = hand.draw(deck,10);
    //masterUI.drawCards(asdf);
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

        if (KeyManager::getJustPressed() == SDLK_BACKQUOTE)
        {
            std::cout << GameState::getGameState()->getTracker<BoardState>()->getResources().toString() << "\n";
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //enemyUI.update();
        MasterCardsUI::getUI()->update();

        //PolyRender::requestRect(glm::vec4(0,0,screenWidth,screenHeight),glm::vec4(1,0,0,1),true,0,0);
        PolyRender::requestRect(handRect,glm::vec4(0,0.7,0,1),false,0,0);
        PolyRender::requestRect(GameUI::getPlayRect(),glm::vec4(1,0,0,1),false,0,0);
        PolyRender::requestRect(GameUI::getEnemyRect(),glm::vec4(1,1,0,1),false,0,0);
        PolyRender::requestRect(GameUI::getDeckRect(),glm::vec4(1,0,1,1),false,0,0);

        //Font::tnr.requestWrite({"HELLO!!!!",{320,320,100,100},{1,0,0,1},0,1});


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
