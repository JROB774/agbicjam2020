#ifndef GAME_HPP
#define GAME_HPP

GLOBAL constexpr const char* START_GAME_MAP       = "tutorial-grid-00.bmp";
GLOBAL constexpr float       START_GAME_X         =  56;
GLOBAL constexpr float       START_GAME_Y         = 172;
GLOBAL constexpr Flip        START_GAME_FLIP      = FLIP_NONE;
GLOBAL constexpr const char* START_CHALLENGE_MAP  = "tutorial-grid-00.bmp";
GLOBAL constexpr float       START_CHALLENGE_X    =  56;
GLOBAL constexpr float       START_CHALLENGE_Y    = 172;
GLOBAL constexpr Flip        START_CHALLENGE_FLIP = FLIP_NONE;
GLOBAL constexpr const char* START_TUTORIAL_MAP   = "tutorial-grid-00.bmp";
GLOBAL constexpr float       START_TUTORIAL_X     =  56;
GLOBAL constexpr float       START_TUTORIAL_Y     = 172;
GLOBAL constexpr Flip        START_TUTORIAL_FLIP  = FLIP_NONE;

GLOBAL constexpr float GRAVITY = 32.0f;

GLOBAL struct GameState
{
    Music mus_fanfare;
    Music mus_game;
    Music mus_tutorial;
    Music mus_challenge;

    Dog dog;

    bool challenge_locked; // @TEMPORARY: JUST SO THE CHALLENGE CAN BE LOCKED FOR NOW...
    bool doing_win_sequence;

} gGameState;

INTERNAL void InitGame   ();
INTERNAL void QuitGame   ();
INTERNAL void UpdateGame (float dt);
INTERNAL void RenderGame (float dt);

INTERNAL void StartGame (std::string start_map, float start_x, float start_y, Flip start_flip, Music& music);
INTERNAL void EndGame   ();

INTERNAL void StartWinSequence ();
INTERNAL void EndWinSequence   ();
INTERNAL void DoWinSequence    ();

#endif /* GAME_HPP */
