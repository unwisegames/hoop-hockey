#ifndef INCLUDED__Bounce__GameOver_h
#define INCLUDED__Bounce__GameOver_h

#include <bricabrac/Game/GameController.h>
#include "UI.h"
#include "Game.h"
#include "overlay.sprites.h"

#include <memory>

class GameOver : public brac::GameController {
public:
    GameOver(size_t score, GameMode m)
    : score_(score), mode(m), restart(Button(overlay.restart, brac::vec2(0, -1.3))), exit(Button(overlay.exit, brac::vec2(0, -4.3))) { }
    
    bool newGame = false;
    GameMode mode;
    
    Button restart;
    Button exit;

private:
    size_t score_;

    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__GameOver_h
