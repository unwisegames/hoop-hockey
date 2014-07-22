#include "GameOver.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

void GameOver::onUpdate(float dt) { }

void GameOver::onDraw() {
    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale(1.1));

    SpriteProgram::drawText("GAME OVER", menufont.glyphs, TextAlign::right, pmv() * mat4::scale(0.5) * mat4::translate({3.3, 14.7, 0}));

    SpriteProgram::draw(overlay.box, pmv() * mat4::scale(1.6) * mat4::translate({0, 2.7, 0 }));
    SpriteProgram::draw(overlay.box, pmv() * mat4::scale(1.6) * mat4::translate({0, 1, 0 }));
    SpriteProgram::draw(overlay.score, pmv() * mat4::scale(1.6) * mat4::translate({0.9, 2.7, 0}));
    SpriteProgram::draw(overlay.score, pmv() * mat4::scale(1.6) * mat4::translate({0.9, 1, 0}));
    SpriteProgram::drawText("SCORE", menufont.glyphs, TextAlign::right, pmv() * mat4::scale(0.45) * mat4::translate({-4, 9, 0}));
    SpriteProgram::drawText("BEST", menufont.glyphs, TextAlign::right, pmv() * mat4::scale(0.45) * mat4::translate({-4.4, 2.9, 0}));

    SpriteProgram::drawText(std::to_string(score_), scorefont.glyphs, TextAlign::right,
                            pmv() * mat4::scale(1.2) * mat4::translate({2.36, 0.92, 0}));
    SpriteProgram::drawText(std::to_string(score_), scorefont.glyphs, TextAlign::right,
                            pmv() * mat4::scale(1.2) * mat4::translate({2.36, 3.19, 0}));

    SpriteProgram::drawText("TAP ANYWHERE", menufont.glyphs, TextAlign::right, pmv() * mat4::scale(0.6) * mat4::translate({4.7, -4, 0}));
    SpriteProgram::drawText("TO PLAY AGAIN", menufont.glyphs, TextAlign::right, pmv() * mat4::scale(0.6) * mat4::translate({4.5, -6, 0}));
}

void GameOver::onResize(brac::vec2 const & size) {
    float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -INFINITY, 6, INFINITY, -halfH, -20, halfH, 20);
}

std::unique_ptr<TouchHandler> GameOver::onTouch(vec2 const & worldPos, float radius) {
    newGame();
    popController();
    return TouchHandler::dummy();
}
