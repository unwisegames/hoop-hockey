#ifndef INCLUDED__Bounce__Game_h
#define INCLUDED__Bounce__Game_h

#include <bricabrac/Game/GameActor.h>
#include <bricabrac/Utility/Signal.h>

constexpr float THREE_LINE_Y = 0;
constexpr float SHOT_LINE_Y = 4.7;

struct Character : brac::Actor { enum State { neutral, happy, sad, excited }; };
struct Platform : brac::Actor { };
struct Barrier : brac::Actor { };
struct Door : brac::Actor { };
struct Swish : brac::Actor { };

enum GameMode { m_arcade, m_buzzer };
const GameMode MODE = []() -> GameMode {
    char const * buzzer_mode = getenv("BOUNCE_BUZZER_MODE");
    if (buzzer_mode && strcmp(buzzer_mode, "YES") == 0) {
        return m_buzzer;
    }
    return m_arcade;
}();

class Game : public brac::GameBase, public std::enable_shared_from_this<Game> {
public:
    enum HoopState { hoop_on = 1, hoop_off = 0 };

    brac::Signal<void(Character const &, brac::vec2 const & impulse)> bounced;
    brac::Signal<void()> scored;
    brac::Signal<void()> touched_sides;
    brac::Signal<void()> door_open;
    brac::Signal<void()> bounced_wall;

    // Achievement-related events
    brac::Signal<void(size_t n)> n_for_n; // n hoops from n hits
    brac::Signal<void()> sharpshot;

    Game(GameMode mode);
    ~Game();

    size_t score() const;
    HoopState hoop_state() const;
    std::string message() const;
    GameMode mode() const;
    size_t clock() const;
    
    virtual std::unique_ptr<brac::TouchHandler> fingerTouch(brac::vec2 const & p, float radius) override;

private:
    struct Members;
    std::unique_ptr<Members> m;

    virtual void updated(float dt) override;
    virtual void getActors(size_t actorId, void * buf) const override;
};

#endif // INCLUDED__Bounce__Game_h
