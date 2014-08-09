#ifndef INCLUDED__Bounce__UI_h
#define INCLUDED__Bounce__UI_h

#include <bricabrac/Game/GameController.h>
#include <memory>

class Button {
public:
    Button(brac::SpriteDef s, brac::vec2 p) : sprite_(s), pos_(p) { }
    
    void draw(brac::mat4 pmv);
    bool within(brac::vec2 v);
    
    bool pressed = false;
    
    //size_t i;
    //brac::Signal<void()> clik;

    //virtual void click();
    
private:
    brac::SpriteDef sprite_;
    brac::vec2 pos_;
};

#endif // INCLUDED__Bounce__UI_h
