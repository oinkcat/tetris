#include "stdafx.h"
#include "game.h"

/* Отобразить новую фигуру */
void TetrisGame::SpawnFigure()
{
    // Установка текущей фигуры
    memcpy(figure, nextFigure, FIG_SIZE * FIG_SIZE);
    figOffsetX = (GAME_COLS - FIG_SIZE) / 2;
    figOffsetY = initialOffsets[nextFigureIdx];

    // Следующая фигура
    nextFigureIdx = rand() % N_FIGURES;
    memcpy(nextFigure, templates[nextFigureIdx], FIG_SIZE * FIG_SIZE);
}

/* Поместить блоки фигуры на поле */
void TetrisGame::PlaceFigureBlocks()
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

/* Удалить линии из блоков */
void TetrisGame::CollapseLines()
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

/* Проверить, можно ли передвинуть фигуру */
bool TetrisGame::CheckCanMove(char fragment[FIG_SIZE][FIG_SIZE], Offsets coords)
{
    for (int fy = 0; fy < FIG_SIZE; fy++)
    {
        for (int fx = 0; fx < FIG_SIZE; fx++)
        {
            if (fragment[fy][fx] == 0)
                continue;

            int fieldX = fx + figOffsetX;
            int fieldY = fy + figOffsetY;
            
			if (InBounds({ fieldX + coords.x, fieldY + coords.y }))
            {
                if (field[fieldY + coords.y][fieldX + coords.x] != 0)
                    return false;
            }
            else
                return false;
        }
    }

    return true;
}

/* Инициализировать игровое поле */
void TetrisGame::Initialize(ENDGAMEPROC gameOverCallback)
{
    gameIsOverFn = gameOverCallback;

    // Установить начальную фигуру
    nextFigureIdx = rand() % N_FIGURES;
    memcpy(nextFigure, templates[nextFigureIdx], FIG_SIZE * FIG_SIZE);

    score = 0;
    memset(field, 0, GAME_ROWS * GAME_COLS);
    SpawnFigure();
}

/* Обновить состояние игры */
void TetrisGame::UpdateState()
{
	if (CheckCanMove(figure, { 0, 1 }))
    {
        figOffsetY++;
    }
    else
    {
        PlaceFigureBlocks();
        CollapseLines();
        SpawnFigure();

		if (!CheckCanMove(figure, { 0, 0 }))
        {
            PlaceFigureBlocks();
            memset(figure, 0, FIG_SIZE * FIG_SIZE);
            gameIsOverFn();
        }
    }
}

/* Сместить фигуру по оси X */
void TetrisGame::MoveFigure(int dx)
{
	if (CheckCanMove(figure, { dx, 0 }))
    {
        figOffsetX += dx;
    }
}

/* Бросить фигуру */
void TetrisGame::DropFigure()
{
	while (CheckCanMove(figure, { 0, 1 }))
    {
        figOffsetY++;
    }
}

/* Развернуть фигуру */
void TetrisGame::RotateFigure()
{
    char rotated[FIG_SIZE][FIG_SIZE];

    for (int i = 0; i < FIG_SIZE; i++)
    {
        for (int j = 0; j < FIG_SIZE; j++)
        {
            rotated[FIG_SIZE - i - 1][j] = figure[j][i];
        }
    }

	if (CheckCanMove(rotated, { 0, 0 }))
    {
        memcpy(&figure, &rotated, FIG_SIZE * FIG_SIZE);
    }
}