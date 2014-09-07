#include "Game.h"
#include "atlas.sprites.h"
#include "background.sprites.h"
#include "overlay.sprites.h"

#include <bricabrac/Game/GameActorImpl.h>
#include <bricabrac/Game/Timer.h>
#include <bricabrac/Math/MathUtil.h>
#include <bricabrac/Math/Random.h>
#include <bricabrac/Logging/Logging.h>

constexpr float GRAVITY = -30;
constexpr float M_PLATFORM = 100;
constexpr int   BASE_SCORE = 2;
constexpr int   BUZZER_DURATION = 60; // seconds

using namespace brac;

enum Group : cpGroup { gr_platform = 1, gr_ring };

enum Layer : cpLayers { l_play = 1<<0, l_fixtures = 1<<1 };

enum CollisionType : cpCollisionType { ct_universe = 1, ct_sides, ct_dunk, ct_wall };

struct CharacterImpl : BodyShapes<Character> {
    CharacterImpl(cpSpace * space, int personality, vec2 const & pos)
    : BodyShapes{space, newBody(1, INFINITY, pos), atlas.ball, CP_NO_GROUP, l_play | l_fixtures}
    {
        for (auto & shape : shapes()) cpShapeSetElasticity(&*shape, 1);
    }
    
    void drop() {
        setForce({0, 0});
    }
};

struct PlatformImpl : public BodyShapes<Platform> {
    ShapePtr glow;
    float radius_;
    
    PlatformImpl(cpSpace * space, vec2 const & pos, float radius)
    : BodyShapes{space, newBody(M_PLATFORM, INFINITY, pos), sensor(atlas.glow), CP_NO_GROUP, l_play}
    , radius_(radius)
    {
        setForce({0, -GRAVITY});
        
        glow = newCircleShape(radius, {0, 0})(body());
    }

    float radius() const override {
        return radius_;
    }
};

struct BarrierImpl : public BodyShapes<Barrier> {
    ConstraintPtr pivot, limit, spring;

    BarrierImpl(cpSpace * space, cpBody * world, vec2 const & hinge, float angle)
    : BodyShapes{space, newBody(0.1, 10, hinge), newBoxShape(0.1, 1.8, {0, -0.8}), gr_ring, l_fixtures}
    {
        setAngle(angle);

        pivot = newPivotJoint(world, body(), hinge);
        limit = newRotaryLimitJoint(world, body(), std::min(0.0f, angle), std::max(0.0f, angle));
        spring = newDampedRotarySpring(world, body(), -2 * angle, 80);
    }
};

struct DoorImpl : public BodyShapes<Door> {
    DoorImpl(cpSpace * space, vec2 const & pos)
    : BodyShapes{space, newStaticBody(pos), sensor(atlas.door)}
    { }
};

struct SwishImpl : public BodyShapes<Swish> {
    SwishImpl(cpSpace * space, vec2 const & pos)
    : BodyShapes{space, newStaticBody(pos), sensor(overlay.box)} // overlay.swish hidden in atlas (TEMP)
    { }
};

struct Game::Members : Game::State, GameImpl<CharacterImpl, PlatformImpl, BarrierImpl, DoorImpl, SwishImpl> {
    ShapePtr worldBox{sensor(boxShape(30, 30, {0, 0}, 0), ct_universe)};
    ShapePtr walls[3], hoop[2];
    ShapePtr dunk{sensor(segmentShape({-0.6, 5.5}, {0.6, 5.5}), ct_dunk)};
    size_t n_for_n = 0;
    bool touched_sides = false;
    int bounced_walls = 0;
    size_t score_modifier = 0;
    std::unique_ptr<Ticker> tick;
    std::unique_ptr<CancelTimer> hoop_timer;
    brac::Stopwatch watch{false};

    Members(SpaceTime & spaceTime) : Impl(spaceTime) { }
};

Game::Game(SpaceTime & spaceTime, GameMode mode, float tly, float sly) : GameBase{spaceTime}, m{new Members{spaceTime}} {
    cpBody * world = m->spaceTime.staticBody;

    m->mode = mode;
    m->three_line_y = tly;
    m->shot_line_y = sly;
    
    auto createCharacter = [=]{
        vec2 v;
        do {
            v = {rand<float>(-5, 5), rand<float>(2, 10)};
        } while (-3 < v.x && v.x < 3 && v.y > 4);
        
        auto & d = m->emplace<DoorImpl>(v); door_open();
        delay(1, [=]{ m->emplace<CharacterImpl>(0, v); release_ball(); }).cancel(destroyed);
        delay(2, [=, &d]{ m->removeWhenSpaceUnlocked(d); }).cancel(d.destroyed);
    };

    ended += [=]{
        m->alert = "";
    };

    m->onSeparate([=](CharacterImpl & character, NoActor<ct_universe> &) {
        switch (mode) {
            case m_arcade:
                gameOver();
                break;
            case m_buzzer:
                if (!m->actors<CharacterImpl>().empty()) {
                    m->removeWhenSpaceUnlocked(character);
                    createCharacter();
                }
                break;
            case m_menu:
                break;
        }
    });
    
    if (mode == m_menu)
    {
        delay(0, [=]{ show_menu(); }).cancel(destroyed);
    }
    else
    {
        m->setGravity({0, GRAVITY});

        m->watch.start();

        if (mode == m_buzzer) {
            m->clock = BUZZER_DURATION;
            m->tick.reset(new Ticker{1, [=]{
                --m->clock;
                if (m->clock == 0) {
                    gameOver();
                } else if (m->clock <= 10) {
                    clock_beep();
                }
            }});
        }
        
        delay(0.1, [=] { createCharacter(); }).cancel(destroyed);

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
            cpShapeSetCollisionType(&*wall, ct_wall);
        }
        
        for (int i = 0; i < 2; ++i) {
            vec2 hinge{2.0f * i - 1, 6};
            auto & hoop = m->hoop[i] = m->circleShape(0.2, hinge);
            cpShapeSetGroup(&*hoop, gr_ring);
            cpShapeSetLayers(&*hoop, l_fixtures);
            cpShapeSetCollisionType(&*hoop, ct_sides);
            cpShapeSetElasticity(&*hoop, 0.3);

            m->emplace<BarrierImpl>(world, hinge, 0.5f - i);
        }

        m->onPostSolve([=](CharacterImpl & character, PlatformImpl &, cpArbiter * arb) {
            m->whenSpaceUnlocked([=, &character]{
                if(cpArbiterIsFirstContact(arb)) {
                    bounced(character, to_vec2(cpArbiterTotalImpulseWithFriction(arb)));
                }
            }, arb);
        });

        m->onSeparate([=](CharacterImpl & character, PlatformImpl & p) {
            m->alert = "";
            if (m->n_for_n % 2 != 0) {
                m->n_for_n = 0;
            }
            ++m->n_for_n;
            m->touched_sides = false;
            m->bounced_walls = 0;
            m->score_modifier = 0;
            if(character.pos().y < m->three_line_y) {
                m->score_modifier += 1; // 3 pointer
            }
            m->actors<SwishImpl>().clear();
            auto v = character.vel();
            auto sp = length(v);
            if (sp > 130) {
                character.setVel(130 * unit(v));
            }
            m->removeWhenSpaceUnlocked(p);
        });

        m->onCollision([=](CharacterImpl & character, NoActor<ct_dunk> &, cpArbiter * arb) {
            if (character.vel().y < 0) {
                m->score += BASE_SCORE + m->score_modifier;
                scored();
                m->hoop_state = hoop_on;
                m->hoop_timer.reset(new CancelTimer(delay(1.8, [=]{ m->hoop_state = hoop_off; })));
                m->hoop_timer->cancel(destroyed);
                //m->actors<SwishImpl>().emplace(vec2{0, 5});
                if (BASE_SCORE + m->score_modifier == 3) {
                    m->alert = "3 POINTS!";
                }
                if (!m->touched_sides) {
                    m->alert = "SWISH!";
                    sharpshot();
                }
                if (!m->touched_sides && m->bounced_walls == 0) {
                    if(BASE_SCORE + m->score_modifier == 3) {
                        m->alert = "AMAZING!";
                    } else {
                        m->alert = "PERFECT!";
                    }
                }
                if (m->bounced_walls > 2) {
                    m->alert = "PINBALL!";
                }
                if (++m->n_for_n > 2) {
                    n_for_n(m->n_for_n / 2);
                    switch (m->n_for_n / 2)
                    {
                        case 2: m->alert = "2 IN A ROW!"; break;
                        case 3: m->alert = "HAT TRICK!"; break;
                        case 4: m->alert = "UNSTOPPABLE!"; break;
                        default:
                            m->alert = std::to_string(m->n_for_n/2) + " IN A ROW!";
                    }
                }
                return true;
            } else {
                character.setVel({0, 0});
                return false;
            }
        });
        
        m->onCollision([=](CharacterImpl &, NoActor<ct_sides> &, cpArbiter * arb) {
            m->touched_sides = true;
            if(cpArbiterIsFirstContact(arb)) {
                touched_sides();
            }
        });

        m->onCollision([=](CharacterImpl &, NoActor<ct_wall> &, cpArbiter * arb) {
            ++m->bounced_walls;
            if(cpArbiterIsFirstContact(arb)) {
                bounced_wall();
            }
        });

        m->quit->clicked += [=]{
            gameOver();
        };
    }
}

void Game::gameOver() {
    if (m->mode == m_buzzer) {
        m->tick.reset();
    }
    m->watch.stop();
    m->duration = m->watch.time();
    end();
}

Game::~Game() { }

Game::State const & Game::state() const { return *m; }

std::unique_ptr<TouchHandler> Game::fingerTouch(vec2 const & p, float radius) {
    if (m->quit->contains(p)) {
        struct ButtonHandler : TouchHandler {
            std::weak_ptr<Button> weak_quit;

            ButtonHandler(std::shared_ptr<Button> const & quit)
            : weak_quit(quit)
            {
                quit->pressed = true;
            }

            virtual void moved(vec2 const & p, bool) {
                if (auto quit = weak_quit.lock()) {
                    quit->pressed = quit->contains(p);
                }
            }

            virtual void ended() override {
                if (auto quit = weak_quit.lock()) {
                    if (quit->pressed) {
                        quit->clicked();
                    }
                }
            }
        };

        return std::unique_ptr<TouchHandler>{new ButtonHandler{m->quit}};
    } else {
        struct BounceTouchHandler : TouchHandler {
            std::weak_ptr<Game> weak_self;
            ConstraintPtr finger[2];

            BounceTouchHandler(Game & self, vec2 const & p, float radius)
            : weak_self{self.shared_from_this()}
            {
                if (p.y < self.m->shot_line_y) {
                    PlatformImpl & platform{self.m->emplace<PlatformImpl>(p, radius)};

                    cpBody * world = self.m->spaceTime.staticBody;

                    constexpr float c = 50000;
                    constexpr float k = c * c / (4 * M_PLATFORM);  // Critically damped

                    finger[0].reset(cpDampedSpringNew(world, platform.body(), {1000, 0}, {0, 0}, 1000, k, c));
                    finger[1].reset(cpDampedSpringNew(world, platform.body(), {0, 1000}, {0, 0}, 1000, k, c));

                    for (int i = 0; i < 2; ++i) {
                        cpSpaceAdd(&self.m->spaceTime, finger[i]);
                    }

                    adjustSprings(p);
                } else {
                    foul();
                }
            }

            ~BounceTouchHandler() {
                if (auto self = weak_self.lock()) {
                    if (!self->m->actors<PlatformImpl>().empty()) {
                        self->m->removeWhenSpaceUnlocked(*self->m->actors<PlatformImpl>().begin());
                    }
                }
            }

            virtual void moved(vec2 const & p, bool) {
                if (auto self = weak_self.lock()) {
                    if (!self->m->actors<PlatformImpl>().empty()) {
                        adjustSprings(p);
                        if (p.y > self->m->shot_line_y) {
                            foul();
                            self->m->removeWhenSpaceUnlocked(*self->m->actors<PlatformImpl>().begin());
                        }
                    }
                }
            }

            void foul() {
                if (auto self = weak_self.lock()) {
                    self->foul();
                    self->m->line_state = line_red;
                    delay(0.5, [=]{ self->m->line_state = line_default; });
                }
            }

            void adjustSprings(vec2 const & p) {
                if (auto self = weak_self.lock()) {
                    cpDampedSpringSetRestLength(&*finger[0], length(p - vec2{1000, 0}));
                    cpDampedSpringSetRestLength(&*finger[1], length(p - vec2{0, 1000}));
                }
            }
        };
        
        return std::unique_ptr<TouchHandler>{new BounceTouchHandler{*this, p, 0.8f + radius}};
    }
}

void Game::doUpdate(float dt) { m->update(dt); }

void Game::getActors(size_t actorId, void * buf) const { m->getActorsForController(actorId, buf); }
