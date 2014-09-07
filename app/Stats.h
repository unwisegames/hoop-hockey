#ifndef INCLUDED__Bounce__Stats_h
#define INCLUDED__Bounce__Stats_h

#include "UI.h"
#include "Game.h"
#include "buttons.sprites.h"

#include <bricabrac/Data/Persistent.h>
#include <bricabrac/Game/GameController.h>
#include <bricabrac/Game/Bragging.h>

#include <memory>

class Stats : public brac::GameController {
public:
    struct ModeState {
        brac::Persistent<int>   careerPoints {"careerArcPoints"};
        brac::Persistent<int>   bestScore    {"bestArcScore"};
        brac::Persistent<int>   gamesPlayed  {"arcGamesPlayed"};
        brac::Leaderboard & leaderboardBestScore;

        ModeState(std::string const & prefix, brac::Leaderboard & leaderboardBestScore)
        : careerPoints  {prefix + "CareerPoints"}
        , bestScore     {prefix + "BestScore"}
        , gamesPlayed   {prefix + "GamesPlayed"}
        , leaderboardBestScore(leaderboardBestScore)
        { }

        std::string average() const;

        void reportScore(int score) {
            ++gamesPlayed;
            careerPoints += score;
            if (*bestScore < score) {
                bestScore = score;
                leaderboardBestScore = score;
            }
        }
    };
    struct State {
        ModeState arcade;
        ModeState buzzer;
        brac::Persistent<float> longestGame{"longestGame"};

        State(brac::Leaderboard & arcadeLeaderboard, brac::Leaderboard & buzzerLeaderboard)
        : arcade("arcade", arcadeLeaderboard)
        , buzzer("buzzer", buzzerLeaderboard)
        { }
    };

    Stats(State & state);

    std::shared_ptr<Button> exit = makeButton(buttons.exit, {0, -5.8});

private:
    State & state;
    std::string arcadeAvg_ = "0";
    std::string buzzerAvg_ = "0";
    std::string longestStr_ = "";

    virtual bool onUpdate(float dt) override;
    virtual void onDraw() override;
    virtual void onResize(brac::vec2 const & size) override;
    virtual std::unique_ptr<brac::TouchHandler> onTouch(brac::vec2 const & worldPos, float radius) override;
};

#endif // INCLUDED__Bounce__Stats_h
