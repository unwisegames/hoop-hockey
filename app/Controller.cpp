#include "Controller.h"
#include "GameOver.h"

#include "brag.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "digifont.sprites.h"
#include "overlay.sprites.h"
#include "menufont.sprites.h"
#include "sounds.h"
#include "headerfont.sprites.h"

#include <iostream>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

struct Controller::Members {
    std::shared_ptr<Game> game;
    sounds audio{0.5, 1};
    float angle = 0;
};

Controller::Controller() : m{new Controller::Members{}} {
    newGame();
}

Controller::~Controller() { }

void Controller::newGame() {
    m->game = std::make_shared<Game>();

    // TODO: Announce achievements.

    m->game->bounced += [=](Character const & character, vec2 const & impulse) {
        //if(brac::length_sq(impulse) > 400) {
            m->audio.bounce.play();
        //}
    };
    
    m->game->door_open += [=] {
        std::cerr << "here";
        m->audio.open.play();
    };
    
    m->game->bounced_wall += [=] {
        m->audio.wall2.play();
    };
    
    m->game->touched_sides += [=] {
        m->audio.wall.play();
    };
    
    m->game->scored += [=]() {
        size_t score = m->game->score();
        if (score <= 25) {
            brag::score25(4 * score, []{});
        }
        if (score <= 100) {
            brag::score100(score, []{});
        }
        m->audio.swish.play();
        m->audio.horn.play();
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
    
    m->game->ended += [=] {
        m->audio.buzz.play();

        size_t score = m->game->score();
        brag::score = score;

        auto gameOver = emplaceController<GameOver>(score);
        gameOver->newGame += [=]{ newGame(); };
    };
}

void Controller::onUpdate(float dt) {
    if (!m->game->update(dt)) {
        //newGame();
    }
    m->angle += dt;
}

void Controller::onDraw() {
    SpriteProgram::draw(background.bg, pmv());
    
    SpriteProgram::draw(atlas.scoreboard, pmv() * mat4::scale(0.7) * mat4::translate({0, 13, 0}));

    SpriteProgram::drawText(std::to_string(m->game->score()), scorefont.glyphs, 1,
                            pmv() * mat4::translate({1.25, 8.7, 0}));
    
    SpriteProgram::draw(atlas.threeline, pmv() * mat4::translate({0, THREE_LINE_Y, 0}));
    SpriteProgram::drawText("3PT", digifont.glyphs, 1, pmv() * mat4::translate({5.3, THREE_LINE_Y-0.65, 0}) * mat4::scale(0.2));

    //SpriteProgram::draw(atlas.shotline, pmv() * mat4::translate({0, SHOT_LINE_Y, 0}));

    SpriteProgram::draw(m->game->actors<Door>       (), pmv());

    SpriteProgram::draw(m->game->actors<Platform>   (), pmv());
    SpriteProgram::draw(m->game->actors<Character>  (), pmv());

    SpriteProgram::draw(atlas.hoop[m->game->hoop_state()], pmv() * mat4::translate({0, 5.3, 0}));

    if(m->game->message() != "") {
        //SpriteProgram::drawText(m->game->message(), headerfont.glyphs, TextAlign::right, pmv() * mat4::translate({0, 5, 0}));
    };
    SpriteProgram::draw(m->game->actors<Swish>       (), pmv());
}

void Controller::onResize(brac::vec2 const & size) {
    float halfH = 0.5 * background.bg.size().y;
    ortho(-6, -6, 6, 6, 0, -INFINITY, halfH, halfH);
}

std::unique_ptr<TouchHandler> Controller::onTouch(vec2 const & worldPos, float radius) {
    if (m->game->hasEnded()) {
        newGame();
    }
    return m->game->fingerTouch(worldPos, radius);
}
