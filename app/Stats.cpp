#include "Stats.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

#include <cmath>
#include <iomanip>
#include <sstream>

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>

std::string Stats::ModeState::average() const {
    if (*gamesPlayed) {
        std::ostringstream oss;
        //(oss << std::setprecision(2) << (float(*careerPoints) / *gamesPlayed)); This is returning whole numbers; not sure why. Reverting for now.
        (oss << roundf((float(*careerPoints) / float(*gamesPlayed)) * 100) / 100);
        return oss.str();
    }
    return "0";
}


std::string durationString(int seconds) {
    int h = (seconds / 60 / 60) % 24;
    int m = (seconds / 60) % 60;
    int s = seconds % 60;

    std::ostringstream oss;
    if (h > 0) {
        oss << h << "H ";
    }
    if (m > 0) {
        oss << m << "M ";
    }
    oss << s << "SEC";

    return oss.str();
}

Stats::Stats(Stats::State & state)
: state(state)
, arcadeAvg_(state.arcade.average())
, buzzerAvg_(state.buzzer.average())
{
    exit->clicked += [=]{ pop(); };

    if (*state.longestGame > 0) {
        longestStr_ = durationString(*state.longestGame);
    }
}

bool Stats::onUpdate(float dt) { return true; }

void Stats::onDraw() {
    auto drawText = [&](std::string const & text, vec2 pos, float scale) {
        SpriteProgram::drawText(text, menufont.glyphs, 0, pmv() * mat4::translate({pos, 0}) * mat4::scale(scale), 0.05);
    };

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale({1.27, 1.22, 1.22}));

    drawText("CAREER", {0, 6.8}, 0.55);
    drawText("ARCADE", {-0.15, 5}, 0.4);
    drawText("BUZZER", {3.3, 5}, 0.4);

    auto drawStat = [&](std::string const & label, float y, std::string const & arc, std::string const & buz) {
        SpriteProgram::draw(overlay.stat, pmv() * mat4::translate({0, y, 0}));
        drawText(label, {-3.35, y - 0.25f}, 0.4);
        drawText(arc, {-0.15, y - 0.25f}, 0.4);
        drawText(buz, {3.3, y - 0.25f}, 0.4);
    };

    drawStat("GAMES"    ,  4.0, std::to_string(*state.arcade.gamesPlayed ), std::to_string(*state.buzzer.gamesPlayed ));
    drawStat("POINTS"   ,  2.2, std::to_string(*state.arcade.careerPoints), std::to_string(*state.buzzer.careerPoints));
    drawStat("AVG."     ,  0.4, arcadeAvg_                                , buzzerAvg_                                );
    drawStat("BEST"     , -1.4, std::to_string(*state.arcade.bestScore   ), std::to_string(*state.buzzer.bestScore   ));

    SpriteProgram::draw(overlay.statsingle, pmv() * mat4::translate({0, -3.2, 0}));
    drawText("LONGEST", {-3.1, -3.45}, 0.4);
    drawText(longestStr_, {1.8, -3.45}, 0.4);

    exit->draw(pmv());
}

void Stats::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {90, INFINITY});
}

std::unique_ptr<TouchHandler> Stats::onTouch(vec2 const & worldPos, float radius) {
    if (auto handler = exit->handleTouch(worldPos)) {
        return handler;
    }
    return TouchHandler::absorb();
}
