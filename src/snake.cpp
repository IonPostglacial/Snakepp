#include <stdint.h>

extern "C" void canvas_set_fill_style(uint32_t color);
extern "C" void canvas_fill_rect(int32_t x, int32_t y, int32_t width, int32_t height);
extern "C" void canvas_fill();
extern "C" void snake_score_changed(int32_t score);
extern "C" void snake_step_period_updated(int32_t period);
extern "C" void snake_game_over();
extern "C" int32_t js_random(int32_t max);

constexpr uint32_t COLOR_BACKGROUND = 0x00000000;
constexpr uint32_t COLOR_SNAKE = 0x00ff00;
constexpr uint32_t COLOR_APPLE = 0xff0000;
constexpr int32_t CELL_SIZE = 10;
constexpr int32_t GRID_WIDTH = 40;
constexpr int32_t GRID_HEIGHT = 40;

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class KeyCode
{
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
};

bool direction_is_opposite(Direction dir, Direction other)
{
    return dir == Direction::UP && other == Direction::DOWN ||
           dir == Direction::DOWN && other == Direction::UP ||
           dir == Direction::LEFT && other == Direction::RIGHT ||
           dir == Direction::RIGHT && other == Direction::LEFT;
}

struct Position
{
    int32_t x;
    int32_t y;

    bool operator ==(Position other) 
    {
        return x == other.x && y == other.y;
    }

    Position moved(Direction direction)
    {
        Position pos = *this;
        switch (direction) {
        case Direction::UP:
            pos.y--;
            break;
        case Direction::DOWN:
            pos.y++;
            break;
        case Direction::LEFT:
            pos.x--;
            break;
        case Direction::RIGHT:
            pos.x++;
            break;
        }
        return pos;
    }
};

struct Snake
{
    Position segments[GRID_WIDTH * GRID_HEIGHT];
    int32_t length;
    int32_t head_index;
    Direction direction;

    Position head_position()
    {
        return segments[head_index];
    }

    Position next_head_position()
    {
        return segments[head_index].moved(direction);
    }

    bool eats_himself()
    {
        for (int32_t i = 0; i < length; i++) {
            if (i == head_index) {
                continue;
            }
            if (head_position() == segments[i]) {
                return true;
            }
        }
        return false;
    }

    bool is_out_of_bounds(int32_t width, int32_t height)
    {
        Position headPosition = head_position();
        return headPosition.x < 0 ||
            headPosition.x >= width ||
            headPosition.y < 0 ||
            headPosition.y >= height;
    }

    void move_ahead()
    {
        Position nextHeadPosition = next_head_position();
        if (head_index == length - 1) {
            head_index = 0;
        } else {
            head_index++;
        }
        segments[head_index] = nextHeadPosition;
    }

    void grow()
    {
        Position nextHeadPosition = next_head_position();
        if (head_index == length) {
            segments[length] = nextHeadPosition;
        } else {
            for (int i = length; i > head_index; i--) {
                segments[i + 1] = segments[i];
            }
            segments[head_index + 1] = nextHeadPosition;
        }
        length++;
    }
};

void paint_background()
{
    canvas_set_fill_style(COLOR_BACKGROUND);
    canvas_fill_rect(0, 0, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
}

void paint_snake(const Snake &snake)
{
    canvas_set_fill_style(COLOR_SNAKE);
    for (int i = 0; i < snake.length; i++) {
        Position segment = snake.segments[i];
        canvas_fill_rect(segment.x * CELL_SIZE, segment.y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    }
}

void paint_apple(Position apple)
{
    canvas_set_fill_style(COLOR_APPLE);
    canvas_fill_rect(apple.x * CELL_SIZE, apple.y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
}

static Snake snake;
static Position apple;
static int32_t stepPeriod;
static int32_t score;
static int32_t next_reward;

void change_snake_direction(Direction d) 
{
    if (direction_is_opposite(snake.direction, d)) {
        return;
    }
    snake.direction = d;
}

extern "C" void on_key_down(enum KeyCode code)
{
    switch (code) {
    case KeyCode::ARROW_UP:
        change_snake_direction(Direction::UP);
        break;
    case KeyCode::ARROW_DOWN:
        change_snake_direction(Direction::DOWN);
        break;
    case KeyCode::ARROW_LEFT:
        change_snake_direction(Direction::LEFT);
        break;
    case KeyCode::ARROW_RIGHT:
        change_snake_direction(Direction::RIGHT);
        break;
    }
}

void speedup_game()
{
    if (stepPeriod > 50) {
        stepPeriod -= 25;
        snake_step_period_updated(stepPeriod);
    }
}

bool snake_will_eat_apple() 
{ 
    return snake.next_head_position() == apple;
}

void update_score()
{
    score += next_reward;
    next_reward += 10;
}

void teleport_apple()
{
    apple.x = js_random(GRID_WIDTH);
    apple.y = js_random(GRID_HEIGHT);
}

void repaint()
{
    paint_background();
    paint_snake(snake);
    paint_apple(apple);
    canvas_fill();
}

extern "C" void step(int32_t timestamp)
{
    if (snake_will_eat_apple()) {
        snake.grow();
        teleport_apple();
        speedup_game();
        update_score();
        snake_score_changed(score);
    } else {
        snake.move_ahead();
    }
    if (snake.is_out_of_bounds(GRID_WIDTH, GRID_HEIGHT) || snake.eats_himself()) {
        snake_game_over();
    }
    repaint();
}

extern "C" void init()
{
    stepPeriod = 300;
    next_reward = 10;
    teleport_apple();
    snake.length = 4;
    snake.head_index = 3;
    snake.direction = Direction::RIGHT;
    snake.segments[1].x = 1;
    snake.segments[2].x = 2;
    snake.segments[3].x = 3;
    repaint();
    snake_score_changed(0);
}