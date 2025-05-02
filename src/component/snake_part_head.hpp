#ifndef SRC_COMPONENT_SNAKE_PART_HEAD_HPP
#define SRC_COMPONENT_SNAKE_PART_HEAD_HPP

struct SnakePartHead
{
    float speed;
    float speedUpFactor;
    char previousPartDirection; // 'w', 'a', 's', or 'd'
}; // struct SnakePartHead

#endif // SRC_COMPONENT_SNAKE_PART_HEAD_HPP
