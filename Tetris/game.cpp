#include "stdafx.h"
#include "game.h"

static const int N_FIGURES = 6;

// Очки
int score = 0;

// Поле
char field[GAME_ROWS][GAME_COLS];

// Шаблоны фигур
static char templates[N_FIGURES][FIG_SIZE][FIG_SIZE] = {
    {
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0
    },
    {
        0, 0, 0, 0,
        1, 1, 1, 0,
        0, 0, 1, 0,
        0, 0, 0, 0
    },
    {
        0, 0, 0, 0,
        1, 1, 1, 0,
        1, 0, 0, 0,
        0, 0, 0, 0
    },
    {
        0, 0, 0, 0,
        0, 1, 1, 0,
        1, 1, 0, 0,
        0, 0, 0, 0
    },
    {
        0, 0, 0, 0,
        1, 1, 0, 0,
        0, 1, 1, 0,
        0, 0, 0, 0
    },
    {
        0, 0, 0, 0,
        0, 1, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0
    },
};

// Начальные отступы сверху
static char initialOffsets[N_FIGURES] = { 0, -1, -1, -1, -1, -1 };

// Индекс следующей фигуры
static int nextFigureIdx;

// Функция обратного вызова - конец игра
static ENDGAMEPROC gameIsOverFn;

// Текущая падающая фигура
char figure[FIG_SIZE][FIG_SIZE];

// Следующая фигура
char nextFigure[FIG_SIZE][FIG_SIZE];

// Координаты фигуры
int figOffsetX, figOffsetY;

// Отобразить новую фигуру
static void SpawnFigure()
{
    // Установка текущей фигуры
    memcpy(figure, nextFigure, FIG_SIZE * FIG_SIZE);
    figOffsetX = (GAME_COLS - FIG_SIZE) / 2;
    figOffsetY = initialOffsets[nextFigureIdx];

    // Следующая фигура
    nextFigureIdx = rand() % N_FIGURES;
    memcpy(nextFigure, templates[nextFigureIdx], FIG_SIZE * FIG_SIZE);
}

// Поместить блоки фигуры на поле
static void PlaceFigureBlocks()
{
    for (int fx = 0; fx < FIG_SIZE; fx++)
    {
        for (int fy = 0; fy < FIG_SIZE; fy++)
        {
            if (figure[fy][fx] != 0)
            {
                field[fy + figOffsetY][fx + figOffsetX] = 1;
            }
        }
    }

    score += 5;
}

// Удалить линии из блоков
static void CollapseLines()
{
    int bottomLine = figOffsetY + FIG_SIZE;

    for (int lineIdx = figOffsetY; lineIdx <= bottomLine; lineIdx++)
    {
        // Определить, выстроена ли линия
        bool isLine = true;
        for (int i = 0; i < GAME_COLS; i++)
        {
            if (field[lineIdx][i] == 0)
            {
                isLine = false;
                break;
            }
        }

        // Сдвинуть блоки вниз
        if (isLine)
        {
            for (int i = lineIdx; i >= 1; i--)
            {
                memcpy(field[i], field[i - 1], GAME_COLS);
            }
            memset(field[0], 0, GAME_COLS);
            score += 100;
        }
    }
}

// Находится ли блок в пределах поля
inline static bool InBounds(int x, int y)
{
    return x >= 0 && x < GAME_COLS && y < GAME_ROWS;
}

// Проверить, можно ли передвинуть фигуру
static bool CheckCanMove(char fragment[FIG_SIZE][FIG_SIZE], int dx, int dy)
{
    for (int fy = 0; fy < FIG_SIZE; fy++)
    {
        for (int fx = 0; fx < FIG_SIZE; fx++)
        {
            if (fragment[fy][fx] == 0)
                continue;

            int fieldX = fx + figOffsetX;
            int fieldY = fy + figOffsetY;
            
            if (InBounds(fieldX + dx, fieldY + dy))
            {
                if (field[fieldY + dy][fieldX + dx] != 0)
                    return false;
            }
            else
                return false;
        }
    }

    return true;
}

// Инициализировать игровое поле
void InitializeGame(ENDGAMEPROC gameOverCallback)
{
    gameIsOverFn = gameOverCallback;

    // Установить начальную фигуру
    nextFigureIdx = rand() % N_FIGURES;
    memcpy(nextFigure, templates[nextFigureIdx], FIG_SIZE * FIG_SIZE);

    score = 0;
    memset(field, 0, GAME_ROWS * GAME_COLS);
    SpawnFigure();
}

// Обновить состояние игры
void UpdateGame()
{
    if (CheckCanMove(figure, 0, 1))
    {
        figOffsetY++;
    }
    else
    {
        PlaceFigureBlocks();
        CollapseLines();
        SpawnFigure();

        if (!CheckCanMove(figure, 0, 0))
        {
            PlaceFigureBlocks();
            memset(figure, 0, FIG_SIZE * FIG_SIZE);
            gameIsOverFn();
        }
    }
}

// Сместить фигуру по оси X
void MoveFigure(int dx)
{
    if (CheckCanMove(figure, dx, 0))
    {
        figOffsetX += dx;
    }
}

// Бросить фигуру
void DropFigure()
{
    while (CheckCanMove(figure, 0, 1))
    {
        figOffsetY++;
    }
}

// Развернуть фигуру
void RotateFigure()
{
    char rotated[FIG_SIZE][FIG_SIZE];

    for (int i = 0; i < FIG_SIZE; i++)
    {
        for (int j = 0; j < FIG_SIZE; j++)
        {
            rotated[FIG_SIZE - i - 1][j] = figure[j][i];
        }
    }

    if (CheckCanMove(rotated, 0, 0))
    {
        memcpy(&figure, &rotated, FIG_SIZE * FIG_SIZE);
    }
}