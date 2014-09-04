#ifndef INCLUDED__Bounce__Stats_h
#define INCLUDED__Bounce__Stats_h

#include <bricabrac/Game/GameController.h>
#include "UI.h"
#include "Game.h"
#include "buttons.sprites.h"

#include <memory>

class Stats : public brac::GameController {
public:
    Stats(size_t arcGames, size_t buzGames, size_t arcPoints, size_t buzPoints, size_t arcBest, size_t buzBest, float longest);

    bool doClose = false;

    Button exit {buttons.exit, {0, -5.8}};

private:
    size_t arcGames_;
    size_t buzGames_;
    size_t arcPoints_;
    size_t buzPoints_;
    size_t arcBest_;
    size_t buzBest_;
    std::string arcAvg_ = "0";
    std::string buzAvg_ = "0";
    std::string longestStr_ = "";
    float longest_;

    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__Stats_h
