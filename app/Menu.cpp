#include "Menu.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"
#include <bricabrac/Game/Bragging.h>
#include <bricabrac/Utility/UrlOpener.h>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

bool Menu::onUpdate(float dt) { return true; }

void Menu::onDraw() {
    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.title, pmv() * mat4::translate({0, 4.5, 0}) * mat4::scale(1.8));

    arcade      ->draw(pmv());
    buzzer      ->draw(pmv());
    gamecenter  ->draw(pmv());
    twitter     ->draw(pmv());
    stats       ->draw(pmv());
}

void Menu::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {100, INFINITY});
}

std::unique_ptr<TouchHandler> Menu::onTouch(vec2 const & worldPos, float) {
    if (auto handler = Button::handleTouch(worldPos, {arcade, buzzer, gamecenter, twitter, stats})) {
        return handler;
    }
    return TouchHandler::absorb();
}
