#include "Controller.h"
#include "brag.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "digifont.sprites.h"

#include <iostream>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

struct Controller::Members {
    std::shared_ptr<Game> game;
    float angle = 0;
};

Controller::Controller() : m{new Controller::Members{}} {
    newGame();
}

Controller::~Controller() { }

void Controller::newGame() {
    m->game = std::make_shared<Game>();

    // TODO: Announce achievements.

    m->game->bounced += [](Character const & character, vec2 const & impulse) {
        // TODO: Bounce noise
    };
    m->game->scored += [=]() {
        size_t score = m->game->score();
        if (score <= 25) {
            brag::score25(4 * score, []{});
        }
        if (score <= 100) {
            brag::score100(score, []{});
        }
    };

    m->game->n_for_n += [=](size_t n){
        switch (n) {
            case 2: brag::twofortwo(100, []{}); break;
            case 3: brag::hattrick(100, []{}); break;
            case 4: brag::hattrick2(100, []{}); break;
        }
    };

    m->game->sharpshot += [=]{
        brag::sharpshot(100, []{});
    };
}

void Controller::onUpdate(float dt) {
    if (!m->game->update(dt)) {
        newGame();
    }
    m->angle += dt;
}

void Controller::onDraw() {
    SpriteProgram::draw(background.bg, pmv());
    
    SpriteProgram::draw(atlas.scoreboard, pmv() * mat4::scale(0.7) * mat4::translate({0, 13, 0}));

    SpriteProgram::drawText(std::to_string(m->game->score()), scorefont.glyphs, TextAlign::right,
                            pmv() * mat4::translate({1.25, 8.7, 0}));
    
    SpriteProgram::draw(atlas.threeline, pmv() * mat4::translate({0, THREE_LINE_Y, 0}));
    SpriteProgram::drawText("3PT", digifont.glyphs, TextAlign::right, pmv() * mat4::translate({5.3, THREE_LINE_Y-0.65, 0}) * mat4::scale(0.2));

    SpriteProgram::draw(m->game->actors<Door>       (), pmv());

    //SpriteProgram::draw(m->game->actors<Platform>   (), pmv());
    SpriteProgram::draw(m->game->actors<Character>  (), pmv());

    SpriteProgram::draw(atlas.hoop[m->game->hoop_state()], pmv() * mat4::translate({0, 5.3, 0}));

    // This currently does nothing because a new game starts immediately after a game ends.
    if (!*m->game) {
        SpriteProgram::drawText(std::to_string(m->game->score()), scorefont.glyphs, TextAlign::right,
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
