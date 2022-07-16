#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 3

typedef enum
{
    PLAYER_X = 0,
    PLAYER_O,
    NUM_PLAYERS
} Player;

typedef enum
{
    CELL_EMPTY = 0,
    CELL_FULL
} Cell;

static char player_chars[NUM_PLAYERS] =
{
    [PLAYER_X] = 'x',
    [PLAYER_O] = 'o',
};

typedef struct
{
    int row;
    int col;
} Cursor;

typedef struct
{
    Cell board[SIZE][SIZE];
    Cursor cur;
    Player player;
    bool is_over;
    int num_filled;
} Game;

void render_game(Game *game)
{
    for(int i = 0; i < SIZE; ++i)
    {
        for(int j = 0; j < SIZE; ++j)
        {
            Cell cell = game->board[i][j];
            int selected = game->cur.row == i && game->cur.col == j;
            fprintf(stdout, selected ? "[%c]": " %c ", cell == CELL_EMPTY ? '-' : player_chars[cell - CELL_FULL]);
        }
        putc('\n', stdout);
    }

}

void finalize(Game *game, Player *winner)
{
    system("clear");
    render_game(game);
    if(winner)
        printf("\n%c won!\n", player_chars[*winner]);
    else
        printf("\ndraw!\n");
    game->is_over = true;
}

bool check_rows(Game *game)
{
    for(int i = 0; i < SIZE; ++i)
    {
        if(game->board[i][0] != CELL_EMPTY &&
           game->board[i][0] == game->board[i][1] && game->board[i][0] == game->board[i][2])
            return true;
    }

    return false;
}

bool check_cols(Game *game)
{
    for(int i = 0; i < SIZE; ++i)
    {
        if(game->board[0][i] != CELL_EMPTY &&
           game->board[0][i] == game->board[1][i] && game->board[0][i] == game->board[2][i])
            return true;
    }

    return false;
}

bool check_diagonals(Game *game)
{
    return (game->board[0][0] != CELL_EMPTY &&
        game->board[0][0] == game->board[1][1] && game->board[0][0] == game->board[2][2]) ||
           (game->board[0][2] != CELL_EMPTY &&
        game->board[0][2] == game->board[1][1] && game->board[0][2] == game->board[2][0]);
}

void update_state(Game *game, char c)
{
    switch(c)
    {
        case 'w': --game->cur.row; break;
        case 'a': --game->cur.col; break;
        case 's': ++game->cur.row; break;
        case 'd': ++game->cur.col; break;
        case 'q': game->is_over = 1; break;
        case ' ':
        {
            if(game->board[game->cur.row][game->cur.col] == CELL_EMPTY)
            {
                game->board[game->cur.row][game->cur.col] = CELL_FULL + game->player;
                ++game->num_filled;
                if(check_rows(game) || check_cols(game) || check_diagonals(game))
                    finalize(game, &game->player);
                else if(game->num_filled == SIZE*SIZE)
                    finalize(game, NULL);
                game->player = 1 - game->player;
            }
        } break;
        default: break;
    }
    if(game->cur.col < 0) game->cur.col = 0;
    if(game->cur.col >= SIZE) game->cur.col = SIZE - 1;
    if(game->cur.row < 0) game->cur.row = 0;
    if(game->cur.row >= SIZE) game->cur.row = SIZE - 1;
}

int main(void)
{
    static Game game = {0};
    static char cmd[256];

    while(!game.is_over)
    {
        system("clear");
        render_game(&game);

        printf("> ");
        fgets(cmd, sizeof(cmd), stdin);
        size_t len = strlen(cmd);
        for(size_t i = 0; i < len; ++i)
            update_state(&game, cmd[i]);
    }

    return 0;
}
