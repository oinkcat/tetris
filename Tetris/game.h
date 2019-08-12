// Определение общих функций логики игры

const int GAME_COLS = 10;
const int GAME_ROWS = 20;
const int FIG_SIZE = 4;

/* Обратный вызов - конец игры */
typedef void(*ENDGAMEPROC)(void);

/* Запрошенная фигура */
enum REQUESTED_FIGURE
{
	CURRENT, NEXT
};

/* Отступы */
struct Offsets
{
	int x;
	int y;
};

/** Логика игры */
class TetrisGame
{
private:
	static const int N_FIGURES = 6;

	/* Текущий счет */
	int score;

	/* Игровое поле */
	char field[GAME_ROWS][GAME_COLS];
	
	/* Падающая фигура */
	char figure[FIG_SIZE][FIG_SIZE];

	/* Падающая фигура */
	char nextFigure[FIG_SIZE][FIG_SIZE];

	/* Координаты фигуры */
	int figOffsetX, figOffsetY;

	/* Индекс следующей фигуры */
	int nextFigureIdx;
	
	/* Функция обратного вызова - конец игра */
	ENDGAMEPROC gameIsOverFn;

	/* Шаблоны фигур */
	const char templates[N_FIGURES][FIG_SIZE][FIG_SIZE] = {
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

	/* Начальные отступы сверху */
	const char initialOffsets[N_FIGURES] = { 0, -1, -1, -1, -1, -1 };

	/* Отобразить новую фигуру */
	void SpawnFigure();

	/* Поместить блоки фигуры на поле */
	void PlaceFigureBlocks();

	/* Удалить линии из блоков */
	void CollapseLines();

	/* Проверить, можно ли передвинуть фигуру */
	bool CheckCanMove(char fragment[FIG_SIZE][FIG_SIZE], Offsets coords);

	/* Проверить вхождение координат в рамки поля */
	static bool InBounds(Offsets coords)
	{
		return coords.x >= 0 && coords.x < GAME_COLS && coords.y < GAME_ROWS;
	}

public:
	/* Инициализация */
	void Initialize(ENDGAMEPROC gameOverCallback);

	/* Обновлене состояния */
	void UpdateState();

	/* Переместить фигуру */
	void MoveFigure(int dx);

	/* Уронить фигуру */
	void DropFigure();

	/* Повернуть фигуру */
	void RotateFigure();

	/* Выдать число набранных очков */
	int getScore()
	{
		return this->score;
	}

	/* Выдать текущую или следующую фигуру */
	char* getFigure(REQUESTED_FIGURE type)
	{
		auto figure = type == CURRENT ? this->figure : this->nextFigure;
		return &figure[0][0];
	}

	/* Выдать игровое поле */
	char* getField()
	{
		return &this->field[0][0];
	}

	/* Выдать отступы фигуры от начала поля */
	Offsets getFigureOffsets()
	{
		return { this->figOffsetX, figOffsetY };
	}
};