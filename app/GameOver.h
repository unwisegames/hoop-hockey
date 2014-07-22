#ifndef INCLUDED__Chickens__GameOver_h
#define INCLUDED__Chickens__GameOver_h

#include <bricabrac/Game/GameController.h>

#include <memory>

class GameOver : public brac::GameController {
public:
    GameOver(size_t score) : score_(score) { }

    brac::Signal<void()> newGame;

private:
    size_t score_;

    virtual void onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Chickens__GameOver_h
