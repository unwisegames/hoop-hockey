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

    SpriteProgram::draw(overlay.title, pmv() * mat4::scale(1.8) * mat4::translate({0, 2.5, 0}));
    
    arcade.draw(pmv());
    buzzer.draw(pmv());
    gamecenter.draw(pmv());
    twitter.draw(pmv());
    stats.draw(pmv());
    
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
        Button & gam;
        Button & twi;
        Button & sta;
        GameMode & m;
        bool & newGame;
        vec2 pos;
        
        MenuTouchHandler(Menu & self, vec2 const & p, float radius)
        :   arc(self.arcade),
            buz(self.buzzer),
            gam(self.gamecenter),
            twi(self.twitter),
            sta(self.stats),
            m(self.mode),
            newGame(self.newGame),
            pos(p)
        {
            self.arcade.pressed = self.arcade.within(p);
            self.buzzer.pressed = self.buzzer.within(p);
            self.gamecenter.pressed = self.gamecenter.within(p);
            self.twitter.pressed = self.twitter.within(p);
            self.stats.pressed = self.stats.within(p);
        }
        
        ~MenuTouchHandler() { }
        
        virtual void moved(vec2 const & p, bool) {
            pos = p;
            arc.pressed = arc.within(p);
            buz.pressed = buz.within(p);
            gam.pressed = gam.within(p);
            twi.pressed = twi.within(p);
            sta.pressed = sta.within(p);
        }
        
        virtual void ended() {
            arc.pressed = false;
            buz.pressed = false;
            gam.pressed = false;
            twi.pressed = false;
            sta.pressed = false;
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
            if (gam.within(pos)) {
                gam.click();
                presentBragUI();
            }
            if (twi.within(pos)) {
                twi.click();
                if (UrlOpener::canOpen("http://www.twitter.com/UnwiseGames")) {
                    UrlOpener::open("http://www.twitter.com/UnwiseGames");
                }
            }
            if (sta.within(pos)) {
                sta.click();
            }
        }
    };
    return std::unique_ptr<TouchHandler>{new MenuTouchHandler{*this, worldPos, radius}};
}
