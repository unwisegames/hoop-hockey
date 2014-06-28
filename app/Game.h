#ifndef INCLUDED__Chickens__Game_h
#define INCLUDED__Chickens__Game_h

#include <bricabrac/Game/GameActor.h>
#include <bricabrac/Utility/Signal.h>

struct Character : brac::Actor { enum State { neutral, happy, sad, excited }; };
struct Platform : brac::Actor { };
struct Barrier : brac::Actor { };

class Game : public brac::GameBase {
public:
    enum State { playing, stopped };

    brac::Signal<void(Character const &)> bounced;
    brac::Signal<void(size_t const &)> ended;

    Game();
    ~Game();

    size_t score() const;

    virtual std::unique_ptr<brac::TouchHandler> fingerTouch(brac::vec2 const & p, float radius) override;

private:
    struct Members;
    std::unique_ptr<Members> m;

    virtual void updated(float dt) override;
    virtual void getActors(size_t actorId, void * buf) const override;
};

#endif // INCLUDED__Chickens__Game_h
