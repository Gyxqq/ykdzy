#include "entity.hpp"
namespace entity {
template <class T>
T mul(T a, int b)
{

    for (int i = 0; i < b - 1; i++) {
        a *= a;
    }
    return a;
}
void entity::move(game* game)
{
    if (this->player_enemy == true) {
    }
}
void entity::find_path_to_player(game* game)
{
    this->path.clear();
    POINT start;
    start.x = this->x;
    start.y = this->y;
    POINT end;
    end.x = game->players[0].x;
    end.y = game->players[0].y;
    if (mul<int>(start.x - end.x, 2) + mul<int>(start.y - end.y, 2) >128 ) {
        return;
    }
    this->path.push_back(start);
    // A* Ѱ·�㷨

}
}