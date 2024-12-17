#include "entity.hpp"
#include "game.hpp"
#include "music.hpp"
namespace entity {
float speed_all = 0.01;
float speed_up_down = 0.03;
int get_block_x(float x)
{
    if (x < 0)
        return (int)x - 1;
    return (int)x;
}
std::vector<class entity> entities;
std::vector<class arrow> arrows;
template <class T>
T mul(T a, int b)
{

    for (int i = 0; i < b - 1; i++) {
        a *= a;
    }
    return a;
}
int entity::move(game* game)
{

    if (pow(this->x - game->players[0].x, 2) + pow(this->y - game->players[0].y, 2) <= 64 * 64) {
        if (this->type <= ENTITY_TYPE_MOB_MAX) {
            if (this->x < game->players[0].x) {
                if (this->if_reach_side(game, side::SIDE_RIGHT)) {
                    return move_state::MOVE_STATE_JUMP;
                } else {
                    return move_state::MOVE_STATE_RUN_RIGHT;
                }
            } else if (this->x > game->players[0].x) {
                if (this->if_reach_side(game, side::SIDE_LEFT)) {
                    return move_state::MOVE_STATE_JUMP;
                } else {
                    return move_state::MOVE_STATE_RUN_LEFT;
                }
            } else {
                return move_state::MOVE_STATE_JUMP;
            }
        } else {
            switch (this->move_) {
            case move_state::MOVE_STATE_RUN_LEFT:
                if (rand() % 300 == 0) {
                    return move_state::MOVE_STATE_RUN_RIGHT;
                } else if (rand() % 300 == 1) {
                    return move_state::MOVE_STATE_STAND;
                } else {
                    if (this->if_reach_side(game, side::SIDE_LEFT)) {
                        return move_state::MOVE_STATE_JUMP;
                    } else {
                        return move_state::MOVE_STATE_RUN_LEFT;
                    }
                }
                break;
            case move_state::MOVE_STATE_RUN_RIGHT:
                if (rand() % 300 == 0) {
                    return move_state::MOVE_STATE_RUN_LEFT;
                } else if (rand() % 300 == 1) {
                    return move_state::MOVE_STATE_STAND;
                } else {
                    if (this->if_reach_side(game, side::SIDE_RIGHT)) {
                        return move_state::MOVE_STATE_JUMP;
                    } else {
                        return move_state::MOVE_STATE_RUN_RIGHT;
                    }
                }
                break;
            case move_state::MOVE_STATE_STAND:
                if (rand() % 300 == 0) {
                    return move_state::MOVE_STATE_RUN_LEFT;
                } else if (rand() % 300 == 1) {
                    return move_state::MOVE_STATE_RUN_RIGHT;
                } else {
                    return move_state::MOVE_STATE_STAND;
                }
                break;

            default:
                return move_state::MOVE_STATE_STAND;
                break;
            }
        }
    }
    return move_state::MOVE_STATE_STAND;
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
    if (mul<int>(start.x - end.x, 2) + mul<int>(start.y - end.y, 2) > 128) {
        return;
    }
    this->path.push_back(start);
    // A* —∞¬∑À„∑®
}
int entity::if_reach_side(game* game, int side)
{
    int x1, x2, y1, y2;
    if (side == 0) { // left
        x1 = get_block_x(this->x - 0.55);
        x2 = get_block_x(this->x - 0.55);
        y1 = this->y - 0.3;
        y2 = this->y + 0.3;
    } else if (side == 1) { // right
        x1 = get_block_x(this->x + 0.55);
        x2 = get_block_x(this->x + 0.55);
        y1 = this->y - 0.3;
        y2 = this->y + 0.3;
    } else if (side == 2) { // up
        x1 = get_block_x(this->x - 0.3);
        x2 = get_block_x(this->x + 0.3);
        y1 = this->y + 0.55;
        y2 = this->y + 0.55;
    } else if (side == 3) { // down
        x1 = get_block_x(this->x - 0.3);
        x2 = get_block_x(this->x + 0.3);
        y1 = this->y - 0.55;
        y2 = this->y - 0.55;
    }
    if (game->world.get_block(x1, y1) == block_type::BLOCK_AIR && game->world.get_block(x2, y2) == block_type::BLOCK_AIR) {
        return 0;
    }
    if (game->world.get_block(x1, y1) == block_type::BLOCK_SKY && game->world.get_block(x2, y2) == block_type::BLOCK_SKY) {
        return 0;
    }
    if (game->world.get_block(x1, y1) == block_type::BLOCK_VOID && game->world.get_block(x2, y2) == block_type::BLOCK_VOID) {
        return 0;
    }

    return 1;
}
float entity::get_distance_to_side(game* game, int side)
{
    int x1, x2, y1, y2;
    float distance = 0;
    if (side == 0) { // left
        x1 = get_block_x(this->x - 0.55 - 1);
        x2 = get_block_x(this->x - 0.55 - 1);
        y1 = this->y - 0.45;
        y2 = this->y + 0.45;
        if ((game->world.get_block(x1, y1) == block_type::BLOCK_AIR || game->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (game->world.get_block(x1, y1) != block_type::BLOCK_VOID && game->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (game->world.get_block(x1, y1) != block_type::BLOCK_AIR && game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
            distance = this->x - 0.5 - x1 - 1;
        } else {
            distance = this->x - 0.5 - x2 - 1;
        }
    } else if (side == 1) {
        x1 = get_block_x(this->x + 0.55 + 1);
        x2 = get_block_x(this->x + 0.55 + 1);
        y1 = this->y - 0.45;
        y2 = this->y + 0.45;
        if ((game->world.get_block(x1, y1) == block_type::BLOCK_AIR || game->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (game->world.get_block(x1, y1) != block_type::BLOCK_VOID && game->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (game->world.get_block(x1, y1) != block_type::BLOCK_AIR && game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
            distance = x2 - 1 - this->x - 0.5 + 1;
        }
    } else if (side == 2) {
        x1 = get_block_x(this->x - 0.45);
        x2 = get_block_x(this->x + 0.45);
        y1 = this->y + 0.55;
        y2 = this->y + 0.55;
        if ((game->world.get_block(x1, y1) == block_type::BLOCK_AIR || game->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (game->world.get_block(x1, y1) != block_type::BLOCK_VOID && game->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (game->world.get_block(x1, y1) != block_type::BLOCK_AIR && game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
            distance = this->y - 0.55 - y1 + 1;
        } else {
            distance = this->y - 0.55 - y2 + 1;
        }
    } else if (side == 3) {
        x1 = get_block_x(this->x - 0.45);
        x2 = get_block_x(this->x + 0.45);
        y1 = this->y - 0.55;
        y2 = this->y - 0.55;
        if ((game->world.get_block(x1, y1) == block_type::BLOCK_AIR || game->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (game->world.get_block(x1, y1) != block_type::BLOCK_VOID && game->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (game->world.get_block(x1, y1) != block_type::BLOCK_AIR && game->world.get_block(x1, y1) != block_type::BLOCK_SKY && game->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
            distance = y1 - 1 - this->y - 0.55;
        } else {
            distance = y2 - 1 - this->y - 0.55;
        }
    }
    if (distance < 0)
        distance = 0;
    return distance;
}
void entity::update(game* game)
{
    this->attack(game);
    if (this->move_ == move_state::MOVE_STATE_JUMP) {
        if (this->jump_state > 100) {
            this->jump_state = 0;
            this->move_ = move_state::MOVE_STATE_STAND;
        } else {
            if (this->if_reach_side(game, side::SIDE_UP)) {
                this->move_ = move_state::MOVE_STATE_STAND;
                this->jump_state = 0;
            } else {
                auto dis = this->get_distance_to_side(game, side::SIDE_UP);
                if (dis < speed_up_down) {
                    this->y += dis;
                } else {
                    this->y += speed_up_down;
                }
                this->jump_state++;
            }
        }

    } else {
        if (!this->if_reach_side(game, side::SIDE_DOWN)) {
            auto dis = this->get_distance_to_side(game, side::SIDE_DOWN);
            if (dis < speed_up_down) {
                this->y -= dis;
            } else {
                this->y -= speed_up_down;
            }
        }
    }
    auto next_move = this->move(game);
    switch (next_move) {
    case move_state::MOVE_STATE_JUMP:
        if (this->move_ == move_state::MOVE_STATE_JUMP) {
            break;
        } else {
            if (this->if_reach_side(game, side::SIDE_DOWN)) {
                this->move_ = move_state::MOVE_STATE_JUMP;
            } else {
                this->move_ = move_state::MOVE_STATE_STAND;
            }
        }
    case move_state::MOVE_STATE_RUN_LEFT:
        if (this->move_ != move_state::MOVE_STATE_JUMP) {
            this->move_ = move_state::MOVE_STATE_RUN_LEFT;
            // this->move_state = 0;
        }
        if (this->move_ == move_state::MOVE_STATE_RUN_LEFT) {
            this->move_state++;
            this->move_state %= 100;
        }
        if (!this->if_reach_side(game, side::SIDE_LEFT)) {
            auto dis = this->get_distance_to_side(game, side::SIDE_LEFT);
            if (dis < speed_all) {
                this->x -= dis;
            } else {
                this->x -= speed_all;
            }
        }
        break;
    case move_state::MOVE_STATE_RUN_RIGHT:
        if (this->move_ != move_state::MOVE_STATE_JUMP) {
            this->move_ = move_state::MOVE_STATE_RUN_RIGHT;
            // this->move_state = 0;
        }
        if (this->move_ == move_state::MOVE_STATE_RUN_RIGHT) {
            this->move_state++;
            this->move_state %= 100;
        }
        if (!this->if_reach_side(game, side::SIDE_RIGHT)) {
            auto dis = this->get_distance_to_side(game, side::SIDE_RIGHT);
            if (dis < speed_all) {
                this->x += dis;
            } else {
                this->x += speed_all;
            }
        }
        break;
    case move_state::MOVE_STATE_STAND:
        if (this->move_ != move_state::MOVE_STATE_JUMP) {
            this->move_ = move_state::MOVE_STATE_STAND;
            this->move_state = 0;
        }
        break;
    default:
        this->move_ = move_state::MOVE_STATE_STAND;
        this->move_state = 0;
        break;
    }
}
item entity::get_drop()
{
    return this->drop;
}
std::string entity::get_name()
{
    switch (this->type) {
    case entity_type::ENTITY_TYPE_PLAYER:
        return "player";
        break;
    case entity_type::ENTITY_TYPE_COW:
        return "cow";
        break;
    case entity_type::ENTITY_TYPE_PIG:
        return "pig";
        break;
    case entity_type::ENTITY_TYPE_ZOMBIE:
        return "zombie";
        break;
    case entity_type::ENTITY_TYPE_SKELETON:
        return "skeleton";
        break;
    case entity_type::ENTITY_TYPE_CREEPER:
        return "creeper";
        break;
    case entity_type::ENTITY_TYPE_ENDERMAN:
        return "enderman";
        break;
    default:
        return "unknown";
        break;
    }
}
void entity::attack(game* game)
{
    if (this->player_enemy == false) {
        return;
    } else {
        if (game->game_tick % 100 != 0) {
            return;
        }
        int attack_index = 0;
        float dis = 0;
        switch (this->type) {
        case entity_type::ENTITY_TYPE_ZOMBIE:
            dis = pow(this->x - game->players[attack_index].x, 2) + pow(this->y - game->players[attack_index].y, 2);
            if (dis <= 4) {
                game->players[attack_index].health -= 10;
                //≤•∑≈“Ù¿÷
                char mis[2048] = {};
                auto num = rand() % 100000;
                sprintf(mis, "open %s alias %d", (std::string(MUSIC_PRE_PATH) + "hit3.mp3").c_str(), num);
                mciSendString(mis, NULL, 0, NULL);
                sprintf(mis, "play %d", num);
                mciSendString(mis, NULL, 0, NULL);
            }
            break;
        case entity_type::ENTITY_TYPE_SKELETON:
            dis = pow(this->x - game->players[attack_index].x, 2) + pow(this->y - game->players[attack_index].y, 2);
            if (dis <= 64) {
                POINTFLOAT start = { this->x, this->y };
                POINTFLOAT end = { game->players[attack_index].x + (rand() % 300 / 100.0) - 1.5, game->players[attack_index].y + (rand() % 300 / 100.0) - 1.5 };
                arrow new_arrow;
                new_arrow.start = start;
                new_arrow.end = end;
                new_arrow.damage = 10;
                new_arrow.speed = 0.1;
                new_arrow.game_tick = 0;
                new_arrow.pos = start;
                arrows.push_back(new_arrow);
            }
            break;
        case entity_type::ENTITY_TYPE_CREEPER:
            dis = pow(this->x - game->players[attack_index].x, 2) + pow(this->y - game->players[attack_index].y, 2);
            if (dis <= 9) {
                int stage = dis;
                if (rand() % (9 - stage + 1) == 0) {
                    game->players[attack_index].health -= 5 * (9 - stage + 1);
                    this->health = 0;
                    char mis[2048] = {};
                    auto num = rand() % 100000;
                    sprintf(mis, "open %s alias %d", (std::string(MUSIC_PRE_PATH) + "creeper-explosion-sound-106759.mp3").c_str(), num);
                    mciSendString(mis, NULL, 0, NULL);
                    sprintf(mis, "play %d", num);
                    mciSendString(mis, NULL, 0, NULL);
                    // ÷‹Œß5*5 ∑ΩøÈ±¨’®
                    for (int i = -2; i <= 2; i++) {
                        for (int j = -2; j <= 2; j++) {
                            if (y + j < 0 || y + j > BLOCKS_PER_CHUNK_Y) {
                                continue;
                            }
                            block* blk = game->world.get_block_ptr(get_block_x(this->x + i), this->y + j);
                            if (rand() % 3 == 0) {
                                blk->type = block_type::BLOCK_AIR;
                                blk->data = NULL;
                                blk->data_size = 0;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}

void arrow::move(game* game)
{
    this->game_tick++;
    if (this->start.x - this->end.x == 0) {
        if (this->start.y < this->end.y) {
            this->pos.y += this->speed;
        } else {
            this->pos.y -= this->speed;
        }
    } else {
        auto k = (this->end.y - this->start.y) / (this->end.x - this->start.x);
        auto d = this->start.y - k * this->start.x;
        if (this->start.x < this->end.x) {
            this->pos.x += this->speed / sqrt(1 + k * k);
            this->pos.y = k * this->pos.x + d;
        } else {
            this->pos.x -= this->speed / sqrt(1 + k * k);
            this->pos.y = k * this->pos.x + d;
        }
    }
}
void update_other(game* game)
{
    for (int i = 0; i < arrows.size(); i++) {
        int attack_index = 0;
        arrows[i].move(game);
        if (arrows[i].game_tick > sqrt(pow(arrows[i].start.x - arrows[i].end.x, 2) + pow(arrows[i].start.y - arrows[i].end.y, 2)) / arrows[i].speed) {
            arrows.erase(arrows.begin() + i);
        } else if (game->world.get_block(get_block_x(arrows[i].pos.x), arrows[i].pos.y) != block_type::BLOCK_AIR) {
            arrows.erase(arrows.begin() + i);
        } else if (pow(arrows[i].pos.x - game->players[attack_index].x, 2) + pow(arrows[i].pos.y - game->players[attack_index].y, 2) <= 1) {
            game->players[attack_index].health -= arrows[i].damage;
            arrows.erase(arrows.begin() + i);
            char mis[2048] = {};
            auto num = rand() % 100000;
            sprintf(mis, "open %s alias %d", (std::string(MUSIC_PRE_PATH) + "hit3.mp3").c_str(), num);
            mciSendString(mis, NULL, 0, NULL);
            sprintf(mis, "play %d", num);
            mciSendString(mis, NULL, 0, NULL);
        }
    }
}

}