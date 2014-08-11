#include "Menu.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

bool Menu::onUpdate(float dt) { return true; }

void Menu::onDraw() {
    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.title, pmv() * mat4::scale(1.8) * mat4::translate({0, 2, 0}));
    
    arcade.draw(pmv());
    buzzer.draw(pmv());
    
    //SpriteProgram::drawText("ARCADE", menufont.glyphs, 0, pmv() * mat4::translate({0, 5, 0}));
}

void Menu::onResize(brac::vec2 const & size) {
    //float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY);
}

std::unique_ptr<TouchHandler> Menu::onTouch(vec2 const & worldPos, float radius) {
    struct MenuTouchHandler : TouchHandler {
        std::weak_ptr<Menu> weak_self;
        Button & arc;
        Button & buz;
        GameMode & m;
        bool & newGame;
        vec2 pos;
        
        MenuTouchHandler(Menu & self, vec2 const & p, float radius)
        :   arc(self.arcade),
            buz(self.buzzer),
            m(self.mode),
            newGame(self.newGame),
            pos(p)
        {
            self.arcade.pressed = self.arcade.within(p);
            self.buzzer.pressed = self.buzzer.within(p);
        }
        
        ~MenuTouchHandler() { }
        
        virtual void moved(vec2 const & p, bool) {
            pos = p;
            arc.pressed = arc.within(p);
            buz.pressed = buz.within(p);
        }
        
        virtual void ended() {
            arc.pressed = false;
            buz.pressed = false;
            if (arc.within(pos)) {
                m = m_arcade;
                newGame = true;
                arc.click();
            }
            if (buz.within(pos)) {
                m = m_buzzer;
                newGame = true;
                buz.click();
            }
        }
    };
    return std::unique_ptr<TouchHandler>{new MenuTouchHandler{*this, worldPos, radius}};
}
