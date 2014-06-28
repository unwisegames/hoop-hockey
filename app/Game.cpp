#include "Game.h"
#include "atlas.sprites.h"

#include <bricabrac/Game/GameActorImpl.h>
#include <bricabrac/Game/Timer.h>
#include <bricabrac/Math/MathUtil.h>

constexpr float START_HEIGHT = 9;
constexpr float GRAVITY = -30;

using namespace brac;

enum Group : cpGroup { gr_platform = 1, gr_ring };

enum CollisionType : cpCollisionType { ct_universe = 1, ct_dunk };

struct CharacterImpl : BodyShapes<Character> {
    CharacterImpl(int personality, vec2 const & pos)
    : BodyShapes{newBody(1, 10, pos), atlas.characters[personality][0]}
    {
        for (auto & shape : shapes()) cpShapeSetElasticity(&*shape, 1);
    }
};

struct PlatformImpl : public BodyShapes<Platform> {
    ShapePtr circle;

    PlatformImpl(vec2 const & pos) : BodyShapes{newStaticBody(pos), atlas.platform, CP_NO_GROUP} {
        // Replace shrinkwrapped shape with a perfect circle.
        for (auto & shape : shapes()) cpShapeSetLayers(&*shape, 0);
        circle = newCircleShape(10, {0, -10})(body());
        cpShapeSetElasticity(&*circle, 1);
        //cpShapeSetFriction(&*circle, INFINITY);
    }
};

struct BarrierImpl : public BodyShapes<Barrier> {
    ConstraintPtr pivot, limit, spring;

    BarrierImpl(cpBody * world, vec2 const & hinge, float angle)
    : BodyShapes{newBody(0.1, 10, hinge), newBoxShape(0.1, 1.8, {0, -0.8}), gr_ring}
    {
        setAngle(angle);

        pivot = newPivotJoint(world, body(), hinge);
        limit = newRotaryLimitJoint(world, body(), std::min(0.0f, angle), std::max(0.0f, angle));
        spring = newDampedRotarySpring(world, body(), -2 * angle, 80);
    }
};

struct Game::Members : GameImpl<CharacterImpl, PlatformImpl, BarrierImpl> {
    ShapePtr worldBox{sensor(boxShape(30, 30, {0, 0}, 0), ct_universe)};
    ShapePtr walls[2], hoop[2];
    ShapePtr dunk{sensor(segmentShape({-1, 6}, {1, 6}), ct_dunk)};
    Game::State state = playing;
    size_t score = 0;
};

Game::Game() : m{new Members} {
    m->setGravity({0, GRAVITY});

    auto createCharacter = [=]{
        m->actors<CharacterImpl>().emplace(0, vec2{-0.4, START_HEIGHT});
    };

    createCharacter();

    for (int i = 0; i < 2; ++i) {
        m->walls[i] = m->segmentShape({12 * (i - 0.5f), -30}, {12 * (i - 0.5f), 30});
        cpShapeSetElasticity(&*m->walls[i], 0.7);
    }

    for (int i = 0; i < 2; ++i) {
        vec2 hinge{2.0f * i - 1, 6};
        m->hoop[i] = m->circleShape(0.2, hinge);
        cpShapeSetGroup(&*m->hoop[i], gr_ring);
        cpShapeSetElasticity(&*m->hoop[i], 0.3);

        m->actors<BarrierImpl>().emplace(m->spaceTime.staticBody, hinge, 0.5f - i);
    }

    m->onCollision([=](CharacterImpl &, PlatformImpl & platform) {
        m->removeWhenSpaceUnlocked(platform);
    });

    m->onPreSolve([=](CharacterImpl & character, PlatformImpl &, cpArbiter * arb) {
    });

    m->onSeparate([=](CharacterImpl & character, PlatformImpl &, cpArbiter * arb) {
        constexpr float m = 1; // Normalised mass
        float E_kinetic = 0.5 * m * length_sq(character.vel());
        float E_potential = m * GRAVITY * (START_HEIGHT - character.pos().y);
        float E = E_kinetic + E_potential;

        // In initial experiments, the ball exited the hoop with about -120 J.
        if (float dE = E < 0 ? std::min(-E, 60.0f) : E > 10 ? E : 0) {
            E_kinetic += dE;
            character.setVel(sqrtf(2 * E_kinetic / m) * unit(character.vel()));
        }
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
