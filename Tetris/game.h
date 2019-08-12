// Определение общих функций логики игры

const int GAME_COLS = 10;
const int GAME_ROWS = 20;
const int FIG_SIZE = 4;

// Текущий счет
extern int score;

// Игровое поле
extern char field[GAME_ROWS][GAME_COLS];

// Падающая фигура
extern char figure[FIG_SIZE][FIG_SIZE];

// Падающая фигура
extern char nextFigure[FIG_SIZE][FIG_SIZE];

// Координаты фигуры
extern int figOffsetX, figOffsetY;

// Обратный вызов - конец игры
typedef void(*ENDGAMEPROC)(void);

// Инициализация
void InitializeGame(ENDGAMEPROC gameOverCallback);

// Обновлене состояния
void UpdateGame();

// Переместить фигуру
void MoveFigure(int dx);

// Уронить фигуру
void DropFigure();

// Повернуть фигуру
void RotateFigure();