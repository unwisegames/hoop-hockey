#ifndef INCLUDED__Bounce__UI_h
#define INCLUDED__Bounce__UI_h

#include <bricabrac/Game/GameController.h>
#include <memory>
#include "buttons.sprites.h"

class Button {
public:
    Button(brac::SpriteDef const s[], brac::vec2 p) : sprites_{s[0], s[1]}, pos_(p) { }
    
    void draw(brac::mat4 pmv);
    bool contains(brac::vec2 v);
    
    bool pressed = false;
    
    //size_t i;
    brac::Signal<void()> click;

    //virtual void click();
    
private:
    brac::SpriteDef sprites_[2]; // default/pressed
    brac::vec2 pos_;
};

#endif // INCLUDED__Bounce__UI_h
