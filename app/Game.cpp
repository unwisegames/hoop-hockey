#include "Game.h"
#include "atlas.sprites.h"

#include <bricabrac/Game/GameActorImpl.h>
#include <bricabrac/Game/Timer.h>

using namespace brac;

enum Group : cpGroup { gr_platform = 1 };

enum CollisionType : cpCollisionType { ct_universe = 1 };

struct CharacterImpl : BodyShapes<Character> {
    CharacterImpl(int personality, vec2 const & pos)
    : BodyShapes(newBody(1, 10, pos), atlas.characters[personality][0])
    {
        for (auto & shape : shapes()) cpShapeSetElasticity(&*shape, 1);
    }
};

struct PlatformImpl : public BodyShapes<Platform> {
    ShapePtr circle;

    PlatformImpl(vec2 const & pos) : BodyShapes(newStaticBody(pos), atlas.platform, CP_NO_GROUP) {
        // Replace shrinkwrapped shape with a perfect circle.
        for (auto & shape : shapes()) cpShapeSetLayers(&*shape, 0);
        circle = newCircleShape(10, {0, -10})(body());
        cpShapeSetElasticity(&*circle, 1);
        //cpShapeSetFriction(&*circle, INFINITY);
    }
};

struct Game::Members : GameImpl<CharacterImpl, PlatformImpl> {
    ShapePtr worldBox;
    Game::State state = playing;
    size_t score = 0;
};

Game::Game() : m{new Members} {
    m->setGravity({0, -30});

    auto createCharacter = [=]{
        m->actors<CharacterImpl>().emplace(0, vec2{0, 9});
    };

    createCharacter();

    m->worldBox = m->boxSensor(30, 30, {0, 0}, ct_universe);

    m->onCollision([=](CharacterImpl & character, PlatformImpl & platform) {
        m->removeWhenSpaceUnlocked(platform);
    });

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_universe> &) {
        m->removeWhenSpaceUnlocked(character);
        createCharacter();
    });
}

Game::~Game() { }

size_t Game::score() const { return m->score; }

std::unique_ptr<TouchHandler> Game::fingerTouch(vec2 const & p, float radius) {
    auto platform = makeActor<PlatformImpl>(p);

    // Does the new platform touch any characters?...
    int i = 0;
    ShapeQuery(&m->spaceTime, &*platform->shapes()[0],
               [&](cpShape *shape, cpContactPointSet *points) {
                   if (cpShapeGetCollisionType(shape) == CharacterImpl::collision_type) {
                       ++i;
                   }
               });

    // ...If so, don't add.
    if (!i) {
        m->actors<PlatformImpl>().clear();
        m->actors<PlatformImpl>().add(std::move(platform));
    }
    return TouchHandler::null();
}

void Game::updated(float dt) { m->update(dt); }

void Game::getActors(size_t actorId, void * buf) const { m->getActorsForController(actorId, buf); }
