#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define N 3
#define WIN_SIZE 420
#define CELL_SIZE (WIN_SIZE/N)
#define THICKNESS 4
#define PADDING_X (CELL_SIZE/4)
#define PADDING_O (CELL_SIZE/5)

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

typedef struct
{
    Cell board[N][N];
    Player player;
    int num_filled;
} Game;

static SDL_Color bg = {0x16, 0x16, 0x16, 0xff};
static SDL_Color fg = {0x2c, 0x2c, 0x2c, 0xff};
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Color player_colors[NUM_PLAYERS] = {
    [PLAYER_X] = {0x5f, 0x87, 0x87, 0xff},
    [PLAYER_O] = {0x87, 0x5f, 0x5f, 0xff}
};

void render_player(int row, int col, Player player)
{
    SDL_Color c = player_colors[player];
    if(player == PLAYER_X)
    {
        int x1 = col*CELL_SIZE + PADDING_X;
        int y1 = row*CELL_SIZE + PADDING_X;
        int x2 = col*CELL_SIZE + CELL_SIZE - PADDING_X;
        int y2 = row*CELL_SIZE + CELL_SIZE - PADDING_X;
        thickLineRGBA(renderer, x1, y1, x2, y2, THICKNESS, c.r, c.g, c.b, c.a);
        y1 += CELL_SIZE - (PADDING_X*2);
        y2 -= CELL_SIZE - (PADDING_X*2);
        thickLineRGBA(renderer, x1, y1, x2, y2, THICKNESS, c.r, c.g, c.b, c.a);
    }
    else
    {
        int cx = col*CELL_SIZE + CELL_SIZE/2;
        int cy = row*CELL_SIZE + CELL_SIZE/2;
        filledCircleRGBA(renderer, cx, cy, CELL_SIZE/2 - PADDING_O, c.r, c.g, c.b, c.a);
        filledCircleRGBA(renderer, cx, cy, CELL_SIZE/2 - THICKNESS - PADDING_O, bg.r, bg.g, bg.b, bg.a);
    }
}

void render_game(Game *game)
{
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, fg.a);
    SDL_RenderDrawLine(renderer, CELL_SIZE, 0, CELL_SIZE, WIN_SIZE);
    SDL_RenderDrawLine(renderer, WIN_SIZE*2/3, 0, WIN_SIZE*2/3, WIN_SIZE);
    SDL_RenderDrawLine(renderer, 0, WIN_SIZE/3, WIN_SIZE, WIN_SIZE/3);
    SDL_RenderDrawLine(renderer, 0, WIN_SIZE*2/3, WIN_SIZE, WIN_SIZE*2/3);
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            if(game->board[i][j] != CELL_EMPTY)
                render_player(i, j, game->board[i][j] - CELL_FULL);
        }
    }

    SDL_RenderPresent(renderer);
}


void reset(Game *game, Player *winner)
{
    render_game(game);
    SDL_ShowSimpleMessageBox(0, NULL, winner ? (*winner == PLAYER_O ? "o won!" : "x won!") : "draw!", window);
    memset(game, 0, sizeof(Game));
}

int check_rows(Game *game)
{
    for(int i = 0; i < N; ++i)
    {
        if(game->board[i][0] != CELL_EMPTY && game->board[i][0] == game->board[i][1] && game->board[i][0] == game->board[i][2])
            return 1;
    }

    return 0;
}

int check_cols(Game *game)
{
    for(int i = 0; i < N; ++i)
    {
        if(game->board[0][i] != CELL_EMPTY && game->board[0][i] == game->board[1][i] && game->board[0][i] == game->board[2][i])
            return 1;
    }

    return 0;
}

int check_diagonals(Game *game)
{
    return (game->board[0][0] != CELL_EMPTY && game->board[0][0] == game->board[1][1] && game->board[0][0] == game->board[2][2]) ||
       (game->board[0][2] != CELL_EMPTY && game->board[0][2] == game->board[1][1] && game->board[0][2] == game->board[2][0]);
}

void update_state(Game *game, int row, int col)
{
    if(game->board[row][col] == CELL_EMPTY)
    {
        game->board[row][col] = CELL_FULL + game->player;
        ++game->num_filled;
        if(check_rows(game) || check_cols(game) || check_diagonals(game))
            reset(game, &game->player);
        else if(game->num_filled == N*N)
            reset(game, NULL);
        else
            game->player = 1 - game->player;
    }
}

int main(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    window = SDL_CreateWindow("tic-tac-toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_SIZE, WIN_SIZE, 0);
    if(!window)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }

    Game game = {0};
    int quit = 0;
    SDL_Event e = {0};
    while(!quit)
    {
        render_game(&game);
        SDL_PollEvent(&e);
        switch(e.type)
        {
            case SDL_QUIT: quit = 1; break;
            case SDL_MOUSEBUTTONUP: update_state(&game, e.button.y/CELL_SIZE, e.button.x/CELL_SIZE); break;
            default: break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
