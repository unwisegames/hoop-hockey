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

    arcade      .draw(pmv());
    buzzer      .draw(pmv());
    gamecenter  .draw(pmv());
    twitter     .draw(pmv());
    stats       .draw(pmv());
}

void Menu::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {100, INFINITY});
}

std::unique_ptr<TouchHandler> Menu::onTouch(vec2 const & worldPos, float radius) {
    struct MenuTouchHandler : TouchHandler {
        Button * button;
        size_t index;
        Menu & self;
        vec2 pos;

        MenuTouchHandler(Menu & self, vec2 const & p, Button * button, size_t index)
        : button(button)
        , index(index)
        , self(self)
        , pos(p)
        { }

        ~MenuTouchHandler() { }

        virtual void moved(vec2 const & p, bool) override {
            pos = p;
            button->pressed = button->contains(p);
        }

        virtual void ended() override {
            button->pressed = false;
            if (button->contains(pos)) {
                if (index == 0) { // arcade
                    self.mode = m_arcade;
                    self.newGame = true;
                } else if (index == 1) { // buzzer
                    self.mode = m_buzzer;
                    self.newGame = true;
                }

                button->click();

                if (index == 2) { // gamecenter
                    presentBragUI();
                } else if (index == 3) { // twitter
                    UrlOpener::open("http://www.twitter.com/UnwiseGames");
                }
            }
        }
    };

    Button * buttons[] = {&arcade, &buzzer, &gamecenter, &twitter, &stats};
    for (auto & b : buttons) {
        if (b->contains(worldPos)) {
            return std::unique_ptr<TouchHandler>{new MenuTouchHandler{*this, worldPos, b, static_cast<size_t>(&b - buttons)}};
        }
    }
    return {};
}
