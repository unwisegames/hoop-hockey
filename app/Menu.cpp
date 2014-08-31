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
        Button * button = nullptr;
        size_t index;
        GameMode & m;
        bool & newGame;
        vec2 pos;

        MenuTouchHandler(Menu & self, vec2 const & p, float radius)
        : m(self.mode)
        , newGame(self.newGame)
        , pos(p)
        {
            Button * buttons[] = {&self.arcade, &self.buzzer, &self.gamecenter, &self.twitter, &self.stats};
            for (auto & b : buttons) {
                if (b->contains(p)) {
                    button = b;
                    index = &b - buttons;
                }
            }
        }

        ~MenuTouchHandler() { }

        virtual void moved(vec2 const & p, bool) override {
            pos = p;
            button->pressed = button->contains(p);
        }

        virtual void ended() override {
            button->pressed = false;
            if (button->contains(pos)) {
                if (index == 0) { // arcade
                    m = m_arcade;
                    newGame = true;
                } else if (index == 1) { // buzzer
                    m = m_buzzer;
                    newGame = true;
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
    return std::unique_ptr<TouchHandler>{new MenuTouchHandler{*this, worldPos, radius}};
}
