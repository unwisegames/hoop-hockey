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
        SpriteProgram::drawText(text, menufont.glyphs, 0, pmv() * mat4::translate({pos, 0}) * mat4::scale(scale));
    };

    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale(1.1));

    drawText("GAME OVER", {0, 7.35}, 0.5);

    auto drawScore = [&](float y, std::string const & label, size_t value) {
        SpriteProgram::draw(overlay.box, pmv() * mat4::translate({0, y, 0 }) * mat4::scale(1.6));
        SpriteProgram::draw(overlay.score, pmv() * mat4::translate({1.44, y, 0}) * mat4::scale(1.6));

        drawText(label, {-2.45, y - 0.25f}, 0.45);
        SpriteProgram::drawText(std::to_string(score_), scorefont.glyphs, 1,
                                pmv() * mat4::translate({3.65, y - 0.5f, 0}) * mat4::scale(1.2));
    };
    drawScore(4.3, "SCORE", score_);
    drawScore(1.6, "BEST", score_);

    drawText("TAP ANYWHERE", {0, -2.4}, 0.6);
    drawText("TO PLAY AGAIN", {0, -3.6}, 0.6);
}

void GameOver::onResize(brac::vec2 const & size) {
    //float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY);
}

std::unique_ptr<TouchHandler> GameOver::onTouch(vec2 const & worldPos, float radius) {
    newGame = true;
    return TouchHandler::dummy();
}
