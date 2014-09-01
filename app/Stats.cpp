#include "Stats.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "scorefont.sprites.h"
#include "menufont.sprites.h"
#include "overlay.sprites.h"

using namespace brac;

#define BRICABRAC_SHADER_NAME Sprite
#include <bricabrac/Shader/LoadShaders.h>
#include <sstream>
#include <math.h>

std::string fstring(float number) {
    std::ostringstream buff;
    buff << number;
    return buff.str();
}

Stats::Stats(size_t arcGames, size_t buzGames, size_t arcPoints, size_t buzPoints, size_t arcBest, size_t buzBest)
:   arcGames_(arcGames),
    buzGames_(buzGames),
    arcPoints_(arcPoints),
    buzPoints_(buzPoints),
    arcBest_(arcBest),
    buzBest_(buzBest)
{
    if (arcGames_ > 0) {
        arcAvg_ = fstring(roundf(float(arcPoints_) / float(arcGames_) * 100) / 100);
    }
    if (buzGames_ > 0) {
        buzAvg_ = fstring(roundf(float(buzPoints_) / float(buzGames_) * 100) / 100);
    }
}

bool Stats::onUpdate(float dt) { return true; }

void Stats::onDraw() {
    auto drawText = [&](std::string const & text, vec2 pos, float scale) {
        SpriteProgram::drawText(text, menufont.glyphs, 0, pmv() * mat4::translate({pos, 0}) * mat4::scale(scale), 0.05);
    };

    SpriteProgram::draw(overlay.fade, pmv());

    SpriteProgram::draw(overlay.window, pmv() * mat4::scale({1.27, 1.22, 1.22}));

    drawText("CAREER", {0, 6.8}, 0.55);
    
    drawText("ARCADE", {-0.15, 5}, 0.4);
    drawText("BUZZER", {3.3, 5}, 0.4);
    
    auto drawStat = [&](std::string const & label, float y, std::string arc, std::string buz) {
        SpriteProgram::draw(overlay.stat, pmv() * mat4::translate({0, y, 0}));
        drawText(label, {-3.35, y - 0.25f}, 0.4);
        drawText(arc, {-0.15, y - 0.25f}, 0.4);
        drawText(buz, {3.3, y - 0.25f}, 0.4);
    };
    
    drawStat("GAMES", 4, std::to_string(arcGames_), std::to_string(buzGames_));
    drawStat("POINTS", 2.2, std::to_string(arcPoints_), std::to_string(buzPoints_));
    drawStat("AVG.", 0.4, arcAvg_, buzAvg_);
    drawStat("BEST", -1.4, std::to_string(arcBest_), std::to_string(buzBest_));

    SpriteProgram::draw(overlay.statsingle, pmv() * mat4::translate({0, -3.2, 0}));
    drawText("LONGEST", {-3.1, -3.45}, 0.4);
    drawText("1 MIN 15 SEC", {1.5, -3.45}, 0.4);
    
    exit.draw(pmv());
}

void Stats::onResize(brac::vec2 const & size) {
    adaptiveOrtho(-6, -6, 6, 6, 0, -INFINITY, 0, INFINITY, {90, INFINITY});
}

std::unique_ptr<TouchHandler> Stats::onTouch(vec2 const & worldPos, float radius) {
    struct StatsTouchHandler : TouchHandler {
        Button & exi;
        bool & doClose;
        vec2 pos;
        
        StatsTouchHandler(Stats & self, vec2 const & p, float radius)
        :   exi(self.exit),
            doClose(self.doClose),
            pos(p)
        {
            self.exit.pressed = self.exit.contains(p);
        }
        
        ~StatsTouchHandler() { }
        
        virtual void moved(vec2 const & p, bool) {
            pos = p;
            exi.pressed = exi.contains(p);
        }
        
        virtual void ended() {
            exi.pressed = false;
            if (exi.contains(pos)) {
                doClose = true;
                exi.click();
            }
        }
    };
    return std::unique_ptr<TouchHandler>{new StatsTouchHandler{*this, worldPos, radius}};
}
