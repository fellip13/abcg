#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Up1, Down1, Up2, Down2, ESC, Menu };
enum class State { Menu, Playing, ScoreP1, ScoreP2, Player1Win, Player2Win };

struct GameData {
  State m_state{State::Playing};
  std::bitset<6> m_input;  // [Up1, Down1, Up2, Down2, ESC, Menu]
};

#endif