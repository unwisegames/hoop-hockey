#include "Menu.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

void Button::draw(brac::mat4 pmv) {
    SpriteProgram::draw(sprite_, pmv * mat4::translate({pos_.x, pos_.y, 0}));
}

bool Button::within(brac::vec2 v) {
    auto size = sprite_.size();
    return (v.x > pos_.x - size.x/2) && (v.x < pos_.x + size.x/2)
        && (v.y > pos_.y - size.y/2) && (v.y < pos_.y + size.y/2);
}

bool Menu::onUpdate(float dt) { return true; }

void Menu::onDraw() {
    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.title, pmv() * mat4::scale(1.8) * mat4::translate({0, 2, 0}));
    
    arcade.draw(pmv());
    buzzer.draw(pmv());
}

void Menu::onResize(brac::vec2 const & size) {
    //float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY);
}

std::unique_ptr<TouchHandler> Menu::onTouch(vec2 const & worldPos, float radius) {
    if (arcade.within(worldPos)) {
        mode = m_arcade;
        newGame = true;
    }
    if (buzzer.within(worldPos)) {
        mode = m_buzzer;
        newGame = true;
    }
    return TouchHandler::dummy();
}
