#include "GameOver.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

GameOver::GameOver(GameMode m, size_t score, size_t best) : score_(score), best_(best) {
    restart ->clicked += [=]{ pop(); };
    exit    ->clicked += [=]{ pop(); };
}

bool GameOver::onUpdate(float dt) { return true; }

void GameOver::onDraw() {
    auto drawText = [&](std::string const & text, vec2 pos, float scale) {
        SpriteProgram::drawText(text, menufont.glyphs, 0, pmv() * mat4::translate({pos, 0}) * mat4::scale(scale), 0.05);
    };

    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale(1.1) * mat4::translate({0, 0.2, 0}));

    drawText("GAME OVER", {0, 6.3}, 0.5);

    auto sprite_context = AutoSprite<SpriteProgram>::context();
    auto drawScore = [&](float y, std::string const & label, size_t value) {
        SpriteProgram::draw(overlay.box, pmv() * mat4::translate({0, y, 0 }) * mat4::scale(1.6));
        SpriteProgram::draw(overlay.score, pmv() * mat4::translate({1.44, y, 0}) * mat4::scale(1.6));

        drawText(label, {-2.45, y - 0.25f}, 0.45);

        sprite_context->tint = {1, 0, 0, 1};
        SpriteProgram::drawText(std::to_string(value), scorefont.glyphs, 1,
                                pmv() * mat4::translate({3.65, y - 0.5f, 0}) * mat4::scale(1.2));
        sprite_context->tint = {1, 1, 1, 1};
    };
    drawScore(4.0, "SCORE", score_);
    drawScore(1.7, "BEST", best_);

    restart->draw(pmv());
    exit->draw(pmv());
}

void GameOver::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {100, INFINITY});
}

std::unique_ptr<TouchHandler> GameOver::onTouch(vec2 const & worldPos, float radius) {
    if (auto handler = Button::handleTouch(worldPos, {restart, exit})) {
        return handler;
    }
    return TouchHandler::absorb();
}
