#include "Controller.h"
#include "atlas.sprites.h"

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
}

void Controller::onUpdate(float dt) {
    m->game->update(dt);
    m->angle += dt;
}

void Controller::onDraw() {
    SpriteProgram::draw(m->game->actors<Platform>   (), pmv());
    SpriteProgram::draw(m->game->actors<Character>  (), pmv());
}

std::unique_ptr<TouchHandler> Controller::onTouch(vec2 const & worldPos, float radius) {
    return m->game->fingerTouch(worldPos, radius);
}
