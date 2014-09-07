#ifndef INCLUDED__Bounce__Menu_h
#define INCLUDED__Bounce__Menu_h

#include <bricabrac/Game/GameController.h>
#include "Game.h"
#include "UI.h"
#include "buttons.sprites.h"

#include <memory>

class Menu : public brac::GameController {
public:
    bool newGame = false;
    GameMode mode = m_menu;

    std::shared_ptr<Button> arcade     = makeButton(buttons.arcade,     { 0,  0.5});
    std::shared_ptr<Button> buzzer     = makeButton(buttons.buzzer,     { 0, -2.5});
    std::shared_ptr<Button> gamecenter = makeButton(buttons.gamecenter, {-3, -5.7});
    std::shared_ptr<Button> twitter    = makeButton(buttons.twitter,    { 3, -5.7});
    std::shared_ptr<Button> stats      = makeButton(buttons.stats,      { 0, -5.7});

    Menu();

private:
    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__Menu_h
