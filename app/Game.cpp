#include "Game.h"
#include "atlas.sprites.h"

#include <bricabrac/Game/GameActorImpl.h>
#include <bricabrac/Game/Timer.h>

constexpr float START_HEIGHT = 9;

using namespace brac;

enum Group : cpGroup { gr_platform = 1 };

enum CollisionType : cpCollisionType { ct_universe = 1, ct_dunk };

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
        circle = newCircleShape(20, {0, -20})(body());
        cpShapeSetElasticity(&*circle, 1);
        //cpShapeSetFriction(&*circle, INFINITY);
    }
};

struct Game::Members : GameImpl<CharacterImpl, PlatformImpl> {
    ShapePtr worldBox;
    ShapePtr walls[2];
    ShapePtr hoop[2];
    ShapePtr dunk;
    Game::State state = playing;
    size_t score = 0;
};

Game::Game() : m{new Members} {
    m->setGravity({0, -30});

    auto createCharacter = [=]{
        m->actors<CharacterImpl>().emplace(0, vec2{-0*5, START_HEIGHT});
    };

    createCharacter();

    m->worldBox = m->sensor(m->boxShape(30, 30, {0, 0}, 0), ct_universe);

    for (int i = 0; i < 2; ++i) {
        m->walls[i] = m->segmentShape({12.0f * i - 6, -30}, {12.0f * i - 6, 30});
        cpShapeSetElasticity(&*m->walls[i], 1);
    }

    for (int i = 0; i < 2; ++i) {
        m->hoop[i] = m->circleShape(0.2, {2.0f * i - 1, 6});
        cpShapeSetElasticity(&*m->hoop[i], 1);
    }

    m->dunk = m->sensor(m->segmentShape({-1, 6}, {1, 6}), ct_dunk);

    m->onCollision([=](CharacterImpl & character, PlatformImpl & platform) {
        m->removeWhenSpaceUnlocked(platform);
    });

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_universe> &) {
        m->removeWhenSpaceUnlocked(character);
        createCharacter();
    });

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_dunk> &, cpArbiter * arb) {
        if (character.vel().y < 0) {
            std::cerr << "Scored!\n";
        }
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

void Game::updated(float dt) {
    // Maintain constant energy.
    if (0) {
    auto & character = *m->actors<CharacterImpl>().begin();
    if (character.vel() != vec2{0, 0}) {
        float speed = sqrtf(2 * (START_HEIGHT - character.pos().y));
        character.setVel(speed * unit(character.vel()));
    }
    }

    m->update(dt);
}

void Game::getActors(size_t actorId, void * buf) const { m->getActorsForController(actorId, buf); }
