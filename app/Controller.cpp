#include "Controller.h"
#include "GameOver.h"
#include "Menu.h"
#include "Stats.h"

#include "brag.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "digifont.sprites.h"
#include "menufont.sprites.h"
#include "sounds.h"
#include "headerfont.sprites.h"

#include <bricabrac/Math/MathUtil.h>
#include <bricabrac/Utility/UrlOpener.h>

#include <iostream>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

struct Controller::Members {
    std::shared_ptr<Game> game;
    sounds audio{0.5, 1};
    float three_line_y = 0;
    float shot_line_y = 0;
    bool newGame = false;
    GameMode mode = m_menu;

    // Persistent data
    Stats::State statsState{brag::arcscore, brag::buzscore};
};

Controller::Controller() : m{new Controller::Members{}} {
    newGame(MODE);
}

Controller::~Controller() { }

void Controller::newGame(GameMode mode) {
    m->newGame = false;
    m->game = std::make_shared<Game>(spaceTime(), mode, m->three_line_y, m->shot_line_y);

    m->audio.music->setLoopCount(-1);
    mode == m_menu ? m->audio.music->play() : m->audio.music->stop();

    // TODO: Announce achievements.

    m->game->bounced += [=](Character const & character, vec2 const & impulse) {
        //if(brac::length_sq(impulse) > 400) {
        m->audio.bounce.play();
        //}
    };

    auto click = [=] { m->audio.click.play(); };

    auto newGame = [=](GameMode mode) {
        return [=]{
            click();
            m->mode = mode;
            m->newGame = true;
        };
    };

    m->game->door_open      += [=] { m->audio.open  .play(); };
    m->game->release_ball   += [=] { m->audio.pop   .play(); };
    m->game->clock_beep     += [=] { m->audio.beep  .play(); };
    m->game->bounced_wall   += [=] { m->audio.wall2 .play(); };
    m->game->touched_sides  += [=] { m->audio.wall  .play(); };
    m->game->foul           += [=] { m->audio.foul  .play(); };

    m->game->scored += [=]() {
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
        auto const & state = m->game->state();
        auto mode = state.mode;

        m->audio.buzz.play();

        if (*m->statsState.longestGame < state.duration) {
            m->statsState.longestGame = state.duration;
        }

        auto modeStats = (mode == m_arcade ? &m->statsState.arcade :
                          mode == m_buzzer ? &m->statsState.buzzer :
                          nullptr);

        size_t score = state.score;
        if (score > 0) {
            switch (state.mode) {
                case m_arcade:
                case m_buzzer:
                    modeStats->reportScore(static_cast<int>(score));
                    break;
                case m_menu:
                    break;
            }

            if (score >= 25) {
                brag::score25(100, []{});
                if (score >= 100) {
                    brag::score100(100, []{});
                }
            }

            auto ptPercent = [=](size_t cp, int target) {
                return (float(cp)/target)*100 > 100 ? 100 : (float(cp)/target)*100;
            };
            size_t cp = *m->statsState.arcade.careerPoints + *m->statsState.buzzer.careerPoints;
            brag::career = cp;
            brag::points500  (ptPercent(cp,   500), []{});
            brag::points1000 (ptPercent(cp,  1000), []{});
            brag::points10000(ptPercent(cp, 10000), []{});
        }

        auto gameOver = emplaceController<GameOver>(mode, score, *modeStats->bestScore);

        gameOver->restart   ->clicked += newGame(m->mode);
        gameOver->exit      ->clicked += newGame(m_menu);
    };

    m->game->show_menu += [=] {
        auto menu = emplaceController<Menu>();

        menu->arcade->clicked += newGame(m_arcade);
        menu->buzzer->clicked += newGame(m_buzzer);

        menu->gamecenter->clicked += [=]{
            click();
            presentBragUI();
        };
        menu->twitter->clicked += [=]{
            click();
            UrlOpener::open("http://www.twitter.com/UnwiseGames");
        };

        menu->stats->clicked += [=]{
            click();
            auto stats = emplaceController<Stats>(m->statsState);
            stats->exit->clicked += [=]{
                click();
            };
        };
    };
}

bool Controller::onUpdate(float dt) {
    if (m->newGame) {
        newGame(m->mode);
        return false;
    }
    m->game->update(dt);
    return true;
}

void Controller::onDraw() {
    auto const & state = m->game->state();

    auto & sprite_context = AutoSprite<SpriteProgram>::context();
    sprite_context->tint = {1, 1, 1, 1};

    SpriteProgram::draw(background.bg, pmv());

    if(state.mode == m_buzzer) {
        SpriteProgram::draw(atlas.scoreboard[1], pmv() * mat4::scale(0.7) * mat4::translate({0, 13, 0}));
        SpriteProgram::drawText(std::to_string(state.clock), scorefont.glyphs, 1, pmv() * mat4::translate({-0.5, 8.71, 0}));
    } else {
        SpriteProgram::draw(atlas.scoreboard[0], pmv() * mat4::scale(0.7) * mat4::translate({0, 13, 0}));
    }

    sprite_context->tint = {1, 0, 0, 1};
    SpriteProgram::drawText(std::to_string(state.score), scorefont.glyphs, 1,
                            pmv() * mat4::translate({1.91, 8.71, 0}));
    sprite_context->tint = {1, 1, 1, 1};

    SpriteProgram::draw(atlas.threeline, pmv() * mat4::translate({0, m->three_line_y, 0}));
    SpriteProgram::drawText("3PT", digifont.glyphs, 1, pmv() * mat4::translate({5.6, static_cast<float>(m->three_line_y-0.65), 0}) * mat4::scale(0.2), 0.3);

    SpriteProgram::draw(atlas.shotline[state.line_state], pmv() * mat4::translate({0, m->shot_line_y, 0}));

    SpriteProgram::draw(m->game->actors<Door>       (), pmv());

    for (auto & platform : m->game->actors<Platform>()) {
        SpriteProgram::drawActor(platform, pmv(), mat4::scale(0.65 * platform.radius()));
    }
    SpriteProgram::draw(m->game->actors<Character>  (), pmv());

    SpriteProgram::draw(atlas.hoop[state.hoop_state], pmv() * mat4::translate({0, 5.3, 0}));

    if(state.alert != "") {
        SpriteProgram::drawText(state.alert, headerfont.glyphs, 0, pmv() * mat4::translate({0, 4.6, 0}), -0.05);
    };

    if (state.mode == m_buzzer) {
        state.quit->draw(pmv());
    }
}

void Controller::onResize(brac::vec2 const & size) {
    float halfH = 0.5 * background.bg.size().y;
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, halfH, halfH);
    auto bottom = inv_pmv() * vec3{0, -1, 0};

    // calculating here as onResize hasn't fired when Controller::newGame first called (rendering lines in the wrong spot first up)
    m->three_line_y = mix(bottom.y, 10.0f, 0.33);
    m->shot_line_y = mix(bottom.y, 10.0f, 0.61);
}

std::unique_ptr<TouchHandler> Controller::onTouch(vec2 const & worldPos, float radius) {
    return m->game->fingerTouch(worldPos, radius);
}
