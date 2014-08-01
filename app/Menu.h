#ifndef INCLUDED__Bounce__Menu_h
#define INCLUDED__Bounce__Menu_h

#include <bricabrac/Game/GameController.h>
#include "Game.h"

#include <memory>

class Button {
public:
    Button(brac::SpriteDef s, brac::vec2 p) : sprite_(s), pos_(p) { }
    
    void draw(brac::mat4 pmv);
    bool within(brac::vec2 v);
    
private:
    brac::SpriteDef sprite_;
    brac::vec2 pos_;
};

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
