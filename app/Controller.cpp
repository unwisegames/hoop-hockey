#include "Controller.h"
#include "brag.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "digifont.sprites.h"

#include <iostream>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

struct Controller::Members {
    std::unique_ptr<Game> game;
    float angle = 0;
};

Controller::Controller() : m{new Controller::Members{}} {
    newGame();
}

Controller::~Controller() { }

void Controller::newGame() {
    m->game.reset(new Game);

    // TODO: Announce achievements.

    m->game->scored.connect([=](size_t score) {
        if (score <= 25) {
            brag::score25(4 * score, []{});
        }
        if (score <= 100) {
            brag::score100(score, []{});
        }
    });

    m->game->n_for_n.connect([=](size_t n){
        switch (n) {
            case 2: brag::twofortwo(100, []{}); break;
            case 3: brag::hattrick(100, []{}); break;
            case 4: brag::hattrick2(100, []{}); break;
        }
    });

    m->game->sharpshot.connect([=]{
        brag::sharpshot(100, []{});
    });
}

void Controller::onUpdate(float dt) {
    m->game->update(dt);
    m->angle += dt;
}

void Controller::onDraw() {
    SpriteProgram::draw(background.bg, pmv());
    
    SpriteProgram::draw(atlas.scoreboard, pmv() * mat4::scale(0.7) * mat4::translate({0, 13, 0}));

    //SpriteProgram::draw(m->game->actors<Platform>   (), pmv());
    SpriteProgram::draw(m->game->actors<Character>  (), pmv());

    SpriteProgram::drawText(std::to_string(m->game->score()), digifont.glyphs, TextAlign::right,
                            pmv() * mat4::translate({1.3, 8.7, 0}) * mat4::scale(0.35));

    if (m->game->state() == Game::State::stopped) {
        SpriteProgram::drawText(std::to_string(m->game->score()), digifont.glyphs, TextAlign::right,
                                pmv() * mat4::translate({0, 0, 0}));
    }
}

void Controller::onResize(brac::vec2 const & size) {
    float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -6, 6, 6, 0, -INFINITY, halfH, halfH);
}

std::unique_ptr<TouchHandler> Controller::onTouch(vec2 const & worldPos, float radius) {
    return m->game->fingerTouch(worldPos, radius);
}
