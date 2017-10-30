// ����������� ����� ������� ������ ����

const int GAME_COLS = 10;
const int GAME_ROWS = 20;
const int FIG_SIZE = 4;

// ������� ����
extern int score;

// ������� ����
extern char field[GAME_ROWS][GAME_COLS];

// �������� ������
extern char figure[FIG_SIZE][FIG_SIZE];

// �������� ������
extern char nextFigure[FIG_SIZE][FIG_SIZE];

// ���������� ������
extern int figOffsetX, figOffsetY;

// �������� ����� - ����� ����
typedef void(*ENDGAMEPROC)(void);

// �������������
void InitializeGame(ENDGAMEPROC gameOverCallback);

// ��������� ���������
void UpdateGame();

// ����������� ������
void MoveFigure(int dx);

// ������� ������
void DropFigure();

// ��������� ������
void RotateFigure();