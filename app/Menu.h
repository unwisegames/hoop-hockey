#ifndef INCLUDED__Bounce__Menu_h
#define INCLUDED__Bounce__Menu_h

#include <bricabrac/Game/GameController.h>
#include "Game.h"
#include "UI.h"

#include <memory>

class Menu : public brac::GameController {
public:
    Menu(Button a, Button b) : arcade(a), buzzer(b) { }
    
    bool newGame = false;
    GameMode mode = m_menu;
    
    Button arcade;
    Button buzzer;

private:
    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__Menu_h
