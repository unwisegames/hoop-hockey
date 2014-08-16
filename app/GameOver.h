#ifndef INCLUDED__Bounce__GameOver_h
#define INCLUDED__Bounce__GameOver_h

#include <bricabrac/Game/GameController.h>
#include "UI.h"
#include "Game.h"
#include "buttons.sprites.h"

#include <memory>

class GameOver : public brac::GameController {
public:
    GameOver(size_t score, GameMode m)
    : score_(score), mode(m) { }
    
    bool newGame = false;
    GameMode mode;
    
    Button restart {buttons.restart, {0, -4.3}};
    Button exit {buttons.exit, {0, -1.3}};

private:
    size_t score_;

    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__GameOver_h
