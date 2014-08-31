#include "GameOver.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

bool GameOver::onUpdate(float dt) { return true; }

void GameOver::onDraw() {
    auto drawText = [&](std::string const & text, vec2 pos, float scale) {
        SpriteProgram::drawText(text, menufont.glyphs, 0, pmv() * mat4::translate({pos, 0}) * mat4::scale(scale), 0.05);
    };

    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale(1.1) * mat4::translate({0, 0.2, 0}));

    drawText("GAME OVER", {0, 6.3}, 0.5);

    auto drawScore = [&](float y, std::string const & label, size_t value) {
        SpriteProgram::draw(overlay.box, pmv() * mat4::translate({0, y, 0 }) * mat4::scale(1.6));
        SpriteProgram::draw(overlay.score, pmv() * mat4::translate({1.44, y, 0}) * mat4::scale(1.6));

        drawText(label, {-2.45, y - 0.25f}, 0.45);
        SpriteProgram::drawText(std::to_string(value), scorefont.glyphs, 1,
                                pmv() * mat4::translate({3.65, y - 0.5f, 0}) * mat4::scale(1.2));
    };
    drawScore(4.0, "SCORE", score_);
    drawScore(1.7, "BEST", best_);

    restart.draw(pmv());
    exit.draw(pmv());
}

void GameOver::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {100, INFINITY});
}

std::unique_ptr<TouchHandler> GameOver::onTouch(vec2 const & worldPos, float radius) {
    struct GameOverTouchHandler : TouchHandler {
        std::weak_ptr<GameOver> weak_self;
        Button & res;
        Button & exi;
        GameMode & m;
        bool & newGame;
        vec2 pos;
        
        GameOverTouchHandler(GameOver & self, vec2 const & p, float radius)
        :   res(self.restart),
            exi(self.exit),
            m(self.mode),
            newGame(self.newGame),
            pos(p)
        {
            self.restart.pressed = self.restart.within(p);
            self.exit.pressed = self.exit.within(p);
        }
        
        ~GameOverTouchHandler() { }
        
        virtual void moved(vec2 const & p, bool) {
            pos = p;
            res.pressed = res.within(p);
            exi.pressed = exi.within(p);
        }
        
        virtual void ended() {
            res.pressed = false;
            exi.pressed = false;
            if (res.within(pos)) {
                newGame = true;
                res.click();
            }
            if (exi.within(pos)) {
                m = m_menu;
                newGame = true;
                exi.click();
            }
        }
    };
    return std::unique_ptr<TouchHandler>{new GameOverTouchHandler{*this, worldPos, radius}};
}
