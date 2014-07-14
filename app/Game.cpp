#include "Game.h"
#include "atlas.sprites.h"
#include "background.sprites.h"

#include <bricabrac/Game/GameActorImpl.h>
#include <bricabrac/Game/Timer.h>
#include <bricabrac/Math/MathUtil.h>
#include <bricabrac/Math/Random.h>

constexpr float GRAVITY = -30;
constexpr float M_PLATFORM = 100;
constexpr int   BASE_SCORE = 2;

using namespace brac;

enum Group : cpGroup { gr_platform = 1, gr_ring };

enum Layer : cpLayers { l_play = 1<<0, l_fixtures = 1<<1 };

enum CollisionType : cpCollisionType { ct_universe = 1, ct_sides, ct_dunk };

struct CharacterImpl : BodyShapes<Character> {
    CharacterImpl(int personality, vec2 const & pos)
    : BodyShapes{newBody(1, INFINITY, pos), atlas.ball, CP_NO_GROUP, l_play | l_fixtures}
//    : BodyShapes{newBody(1, INFINITY, pos), atlas.characters[personality][0], CP_NO_GROUP, l_play | l_fixtures}
    {
        //setForce({0, 30});
        for (auto & shape : shapes()) cpShapeSetElasticity(&*shape, 1);
    }
    
    void drop() {
        setForce({0, 0});
    }
};

struct PlatformImpl : public BodyShapes<Platform> {
    ShapePtr glow;
    
    PlatformImpl(vec2 const & pos, float radius)
    : BodyShapes{newBody(M_PLATFORM, INFINITY, pos), sensor(atlas.glow), CP_NO_GROUP, l_play}
//    : BodyShapes{newBody(M_PLATFORM, INFINITY, pos), newCircleShape(radius), CP_NO_GROUP, l_play}
    {
        setForce({0, 30}); // Antigravity
        
        glow = newCircleShape(radius, {0, 0})(body());
        
        for (auto & shape : shapes()) {
            cpShapeSetElasticity(&*shape, 0.3);
        }
    }
};

struct BarrierImpl : public BodyShapes<Barrier> {
    ConstraintPtr pivot, limit, spring;

    BarrierImpl(cpBody * world, vec2 const & hinge, float angle)
    : BodyShapes{newBody(0.1, 10, hinge), newBoxShape(0.1, 1.8, {0, -0.8}), gr_ring, l_fixtures}
    {
        setAngle(angle);

        pivot = newPivotJoint(world, body(), hinge);
        limit = newRotaryLimitJoint(world, body(), std::min(0.0f, angle), std::max(0.0f, angle));
        spring = newDampedRotarySpring(world, body(), -2 * angle, 80);
    }
};

struct DoorImpl : public BodyShapes<Door> {
    DoorImpl(vec2 const & pos)
    : BodyShapes{newStaticBody(pos), sensor(atlas.door)}
    { }
};

struct Game::Members : GameImpl<CharacterImpl, PlatformImpl, BarrierImpl, DoorImpl> {
    ShapePtr worldBox{sensor(boxShape(30, 30, {0, 0}, 0), ct_universe)};
    ShapePtr walls[3], hoop[2];
    ShapePtr dunk{sensor(segmentShape({-1, 6}, {1, 6}), ct_dunk)};
    size_t score = 0;
    size_t n_for_n = 0;
    bool touched_sides = false;
    Game::HoopState hoop_state = hoop_off;
    size_t score_modifier = 0;
};

Game::Game() : m{new Members} {
    cpBody * world = m->spaceTime.staticBody;

    m->setGravity({0, GRAVITY});

    auto createCharacter = [=]{
        vec2 v;
        do {
            v = {rand<float>(-5, 5), rand<float>(2, 10)};
        } while (-3 < v.x && v.x < 3 && v.y > 4);

        auto & d = m->actors<DoorImpl>().emplace(v);
        delay(1, [=]{ m->actors<CharacterImpl>().emplace(0, v); }).cancel(destroyed);
        delay(2, [=, &d]{ m->removeWhenSpaceUnlocked(d); }).cancel(d.destroyed);
    };

    createCharacter();

    float hh = 0.5 * background.bg.size().y;
    vec2 wallVerts[3][2] = {
        {{-6, -30}, {-6, 30}},
        {{ 6, -30}, { 6, 30}},
        {{-6,  hh}, { 6, hh}},
    };
    for (int i = 0; i < 3; ++i) {
        auto & wall = m->walls[i] = m->segmentShape(wallVerts[i][0], wallVerts[i][1]);
        cpShapeSetLayers(&*wall, l_fixtures);
        cpShapeSetElasticity(&*wall, 0.7);
    }
    
    for (int i = 0; i < 2; ++i) {
        vec2 hinge{2.0f * i - 1, 6};
        auto & hoop = m->hoop[i] = m->circleShape(0.2, hinge);
        cpShapeSetGroup(&*hoop, gr_ring);
        cpShapeSetLayers(&*hoop, l_fixtures);
        cpShapeSetCollisionType(&*hoop, ct_sides);
        cpShapeSetElasticity(&*hoop, 0.3);

        m->actors<BarrierImpl>().emplace(world, hinge, 0.5f - i);
    }

    m->onPostSolve([=](CharacterImpl & character, PlatformImpl &, cpArbiter * arb) {
        m->whenSpaceUnlocked([=, &character]{
            bounced(character, to_vec2(cpArbiterTotalImpulseWithFriction(arb)));
        }, arb);
    });

    m->onSeparate([=](CharacterImpl & character, PlatformImpl &) {
        if (m->n_for_n % 2 != 0) {
            m->n_for_n = 0;
        }
        ++m->n_for_n;
        m->touched_sides = false;
        m->hoop_state = hoop_off;
        m->score_modifier = 0;
        if(character.pos().y < THREE_LINE_Y) {
            m->score_modifier += 1; // 3 pointer
        }
    });

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_universe> &) {
        end();
    });

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_dunk> &, cpArbiter * arb) {
        if (character.vel().y < 0) {
            m->score += BASE_SCORE + m->score_modifier;
            scored();
            m->hoop_state = hoop_on;
            if (++m->n_for_n > 2) {
                n_for_n(m->n_for_n / 2);
            }
            if (!m->touched_sides) {
                sharpshot();
            }
        }
    });

    m->onCollision([=](CharacterImpl &, NoActor<ct_sides> &) {
        m->touched_sides = true;
    });
}

Game::~Game() { }

size_t Game::score() const { return m->score; }

Game::HoopState Game::hoop_state() const { return m->hoop_state; }

std::unique_ptr<TouchHandler> Game::fingerTouch(vec2 const & p, float radius) {
    struct BounceTouchHandler : TouchHandler {
        std::weak_ptr<Game> weak_self;
        PlatformImpl & platform;
        ConstraintPtr finger[2];

        BounceTouchHandler(Game & self, vec2 const & p, float radius)
        : weak_self{self.shared_from_this()}
        , platform{self.m->actors<PlatformImpl>().emplace(p, radius)}
        {
            cpBody * world = self.m->spaceTime.staticBody;

            constexpr float c = 50000;
            constexpr float k = c * c / (4 * M_PLATFORM);  // Critically damped

            finger[0].reset(cpDampedSpringNew(world, platform.body(), {1000, 0}, {0, 0}, 1000, k, c));
            finger[1].reset(cpDampedSpringNew(world, platform.body(), {0, 1000}, {0, 0}, 1000, k, c));

            for (int i = 0; i < 2; ++i) {
                cpSpaceAdd(&self.m->spaceTime, finger[i]);
            }

            adjustSprings(p);
        }

        ~BounceTouchHandler() {
            if (auto self = weak_self.lock()) {
                self->m->removeWhenSpaceUnlocked(*self->m->actors<PlatformImpl>().begin());
            }
        }

        virtual void moved(vec2 const & p, bool) {
            adjustSprings(p);
        }

        void adjustSprings(vec2 const & p) {
            cpDampedSpringSetRestLength(&*finger[0], length(p - vec2{1000, 0}));
            cpDampedSpringSetRestLength(&*finger[1], length(p - vec2{0, 1000}));
        }
    };
    return std::unique_ptr<TouchHandler>{new BounceTouchHandler{*this, p, 0.8f + radius}};
}

void Game::updated(float dt) { m->update(dt); }
void Game::getActors(size_t actorId, void * buf) const { m->getActorsForController(actorId, buf); }
