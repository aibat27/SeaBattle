#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <iomanip>
#include "Win10.h"
using namespace std;

#define TITLE_WIDTH 151
#define TITLE_HEIGHT 36
#define TITLE_FONT_SIZE 20

#define WIDTH 80
#define HEIGHT 20
#define FONT_SIZE 36

// генерация случайных чисел
random_device Rand;
#define RAND(min,max) (Rand()%((max)-(min)+1)+(min))

// перечисление для сторон бардюра
enum
{
	B_UP, B_RU, B_RIGHT, B_RD, B_DOWN, B_LD, B_LEFT, B_LU
};

// перечисление для направлений корабля
enum
{
	SHIP_H, SHIP_V
};

// перечисление для ходов
enum
{
	TURN_P, TURN_C
};

// структура координат
struct Coord
{
	int row, col; // строки, столбцы
};

// структура цвета
struct Color
{
	int backgr, sym; // цвет фона и символа
};

// структура ячейки поля
struct FieldCell
{
	Coord pos;
	char sym;
	bool attacked;
};

// структура корабля
struct Ship
{
	Coord pos; // координаты левого верхнего угла корабля
	int direction = SHIP_H; // направление корабля
	int Size = 1; // размер корабля
};

int backgr = _COLOR::BLUE; // цвет фона игрового поля
int turn = TURN_P; // кто ходит первым
int roundCnt = 1; // количество раундов

bool gameOver = false; // закончилась ли игра
bool shipsPlaced = false; // установлены ли все корабли

// меню
Color titleC = { _COLOR::BLACK, _COLOR::WHITE }; // цвет заставки
Color menuC = { _COLOR::WHITE, _COLOR::BLACK }; // цвет элемента меню
Color menuSelC = { _COLOR::WHITE, _COLOR::BRIGHT_WHITE }; // цвет выделенного цвета меню
Color hintC = { _COLOR::WHITE, _COLOR::GRAY }; // цвет подсказок

// игровое поле
FieldCell* field = nullptr; // массив ячеек поля
FieldCell* fieldEnemy = nullptr; // массив ячеек поля врага
int fieldH = 10, fieldW = 10; // высота и ширина поля
int fieldIndentX = 15, fieldIndentY = 3; // отступ левого верхнего угла поля от краев окна по x и y
char fieldSym = '~'; // символ пустой ячейки поля
Color fieldC = { backgr, _COLOR::GRAY }; // цвет пусой ячейки поля
Color fieldBoardC = { _COLOR::BRIGHT_WHITE, _COLOR::BLACK }; // цвет бардюра поля

// корабли
int shipsCnt[4]{ 0, 0, 0, 0 }; // количество кораблей размеров в 1, 2, 3, 4 
char shipSym = char(4); // символ корабля
Color shipC = { backgr, _COLOR::WHITE }; // цвет корабля
Color shipGhostC = { backgr, _COLOR::GREEN }; // цвет призрака корабля

char attackSym = 'X'; // символ атакованной ячейки поля
Color attackC = { backgr, _COLOR::RED }; // цвет атакованной ячейки
Color attackGhostC = { backgr, _COLOR::LIGHT_GREEN }; // цвет призрака ячейки которая будет атакована
Color attackShipC = { backgr, _COLOR::LIGHT_RED }; // цвет атакованного корабля

Ship temp = {};

// прототипы ф-ций
void StartTitle(); // ф-ция для вывода заставки при запуске
void StartMenu(); //  ф-ция для вывода главного меню
void StartGame(); // ф-ция для начала новой игры
void ContinueGame(); // ф-ция для загрузки игры
void Game(); // ф-ция игры
void FillField(FieldCell*); // ф-ция для заполнения поля кораблями случайным образом
void DrawField(FieldCell*, int*, bool, Ship& = temp); // ф-ция для рисования поля
bool CheckGameOver(FieldCell*);
void DrawBoard(Coord&); // ф-ция для рисования бардюра поля
int CheckPlace(Ship&); // ф-ция для проверки не выходит ли корабль за границы поля
bool CheckShips(FieldCell*, Ship&); // ф-ция для проверки не находятся ли вокруг корабля другие корабли
int CheckBorder(int); // ф-ция для проверки находится ли корабль на краю поля, возвращающая сторону сторону края поля
void SetColor(Color&); // перегруженная ф-ция для установки цвета
void SetPos(Coord&); // перегруженная ф-ция для установки курсора на координаты


int main()
{
	SetConsoleMode(TITLE_WIDTH, TITLE_HEIGHT, TITLE_FONT_SIZE); // ф-ция для настройки размера консоли и шрифта

	StartTitle(); // ф-ция для вывода заставки при запуске
	StartMenu(); // ф-ция для вывода главного меню

	system("pause > nul");
	return 0;
}

void StartTitle()
{
	SetColor(titleC);
	system("cls");

	string space = "     "; // разделитель между буквами

	for (int i = 0; i < (TITLE_HEIGHT - 9) / 2; i++)
	{
		cout << "\n";
	}

	//            S                           E                           A                                      B                             A                            T                         T                        L                         E
	cout << "   _______ " << space << " _________ " << space << "              " << space << space << " _________  " << space << "              " << space << " _________ " << space << " _________ " << space << " _        " << space << " _________ " << "\n";
	cout << "  /  _____|" << space << "|  _______|" << space << "      /\\      " << space << space << "|  ______ \\ " << space << "      /\\      " << space << "|___   ___|" << space << "|___   ___|" << space << "| |       " << space << "|  _______|" << "\n";
	cout << " /  /      " << space << "| |        " << space << "     /  \\     " << space << space << "| |      \\ \\" << space << "     /  \\     " << space << "    | |    " << space << "    | |    " << space << "| |       " << space << "| |        " << "\n";
	cout << " \\  \\___   " << space << "| |_______ " << space << "    / /\\ \\    " << space << space << "| |______/ /" << space << "    / /\\ \\    " << space << "    | |    " << space << "    | |    " << space << "| |       " << space << "| |_______ " << "\n";
	cout << "  \\___  \\  " << space << "|  _______|" << space << "   / /__\\ \\   " << space << space << "|  ______  |" << space << "   / /__\\ \\   " << space << "    | |    " << space << "    | |    " << space << "| |       " << space << "|  _______|" << "\n";
	cout << "      \\  \\ " << space << "| |        " << space << "  / /    \\ \\  " << space << space << "| |      \\ \\" << space << "  / /    \\ \\  " << space << "    | |    " << space << "    | |    " << space << "| |       " << space << "| |        " << "\n";
	cout << " _____/  / " << space << "| |_______ " << space << " / /      \\ \\ " << space << space << "| |______/ /" << space << " / /      \\ \\ " << space << "    | |    " << space << "    | |    " << space << "| |______ " << space << "| |_______ " << "\n";
	cout << "|_______/  " << space << "|_________|" << space << "/_/        \\_\\" << space << space << "|_________/ " << space << "/_/        \\_\\" << space << "    |_|    " << space << "    |_|    " << space << "|________|" << space << "|_________|" << "\n";

	Sleep(2500); // задержка
	system("cls"); // очистка экрана
	SetConsoleMode(WIDTH, HEIGHT, FONT_SIZE);
}

void StartMenu()
{
	string menu[10]{}; // массив с элементами меню
	int menuSize = 0; // количество элементов меню
	int select = 0; // индекс выбранного элемента
	bool exitMenu = false; // нужно ли выйти из меню
	int row = 0, col = 0; // переменные для установки строки и столбца

	ifstream fin("menu.txt"); // открыть файл с элементами меню

	if (!fin.is_open())
	{
		cout << "Ошибка открытия файла для чтения!" << "\n";
		return;
	}

	// запись строк из файла в массив
	for (; !fin.eof(); menuSize++)
	{
		getline(fin, menu[menuSize]);
	}

	fin.close(); // закрытие файла

	SetColor(menuC.backgr, 0); // установка цвета фона
	system("cls"); // очистка экрана

	// цикл рисования меню
	while (!exitMenu)
	{
		for (row = 0; row < (HEIGHT - menuSize) / 2; row++); // выбор строки

		// проходимся по всем элементам меню
		for (int i = 0; i < menuSize; i++)
		{
			// выбор столбца
			for (col = 0; col < (WIDTH - menu[i].length()) / 2; col++)
			{
				if (select == i)
				{
					SetColor(menuSelC); // установка цвета для выбранного элемента меню
				}
				else
				{
					SetColor(menuC); // установка цвета для элемента меню
				}
			}

			SetPos(row + i, col); // установка курсора на заданную строку и столбец

			cout << menu[i]; // вывод элемента меню
		}

		int key = _getch(); // получение кода нажатой клавишы

		switch (key) // обработка кодов клавиш
		{
		case _KEY::UP: case _KEY::LEFT:

			if (select == 0)
			{
				select = menuSize - 1;
			}
			else
			{
				select--;
			}

			break;
		case _KEY::DOWN: case _KEY::RIGHT:

			if (select == menuSize - 1)
			{
				select = 0;
			}
			else
			{
				select++;
			}

			break;
		case _KEY::ENTER:
			switch (select)
			{
			case 0:
				StartGame(); // ф-ция для начала новой игры
				break;
			case 1:
				// ContinueGame(); // ф-ция для продолжения сохраненной игры
				break;
			case 2:
				// Statistics();
				break;
			case 3:
				// Settings();
				break;
			case 4:
				// About();
				break;
			case 5:
				exitMenu = true; // выход из меню
				break;
			}

			break;
		case _KEY::ESC:
			exitMenu = true; // выход из меню
			break;
		}
	}

	SetColor(menuC); // установка цвета для элемента меню
	system("cls");
}

void StartGame()
{
	system("cls");

	SetColor(hintC);

	SetPos(14, 15);
	cout << "E - увеличить размер корабля. R - крутить корабль";

	SetPos(15, 15);
	cout << "G - установить все корабли случайным образом";

	system("chcp 866 > nul");

	field = new FieldCell[fieldH * fieldW]{}; // обявление массива ячеек поля

	int select[4]{ 0, -1, -1, -1 }; // выбор ячеек поля для установки корабля
	int direction = SHIP_H; // направление корабля
	int Size = 1; // размер корабля

	// заполнение ячеек поля координатами
	for (int i = 0; i < fieldH; i++)
	{
		for (int j = 0; j < fieldW; j++)
		{
			field[i * fieldW + j].pos.row = i + fieldIndentY;
			field[i * fieldW + j].pos.col = j * 2 + fieldIndentX;
		}
	}

	// заполнение поля пустыми ячейками
	for (int i = 0; i < fieldH * fieldW; i++)
	{
		field[i].sym = fieldSym;
	}

	DrawBoard(field[0].pos); // ф-ция для рисования бардюра поля

	// цикл заполнения поля кораблями
	while (!shipsPlaced)
	{
		Ship ship = {};

		int row = int(select[0] / fieldW); // получение строки выбранной ячейки
		int col = select[0] % fieldW; // получение столбца выбранной ячейки

		ship.pos.row = row; // запись строки выбранной ячейки в корабль
		ship.pos.col = col; // запись строки выбранной ячейки в корабль
		ship.direction = direction; // запись направления корабля в корабль
		ship.Size = Size; // запись размера корабля в корабль

		DrawField(field, select, false, ship);

		int key = _getch();

		if (key == CURSOR1 || key == CURSOR2)
		{
			key = _getch();
		}

		switch (key)
		{
		case _KEY::UP:

			if (select[0] < fieldW)
			{
				select[0] += (fieldH - 1) * fieldW;
			}
			else
			{
				select[0] -= fieldW;
			}

			break;
		case _KEY::DOWN:

			if (select[Size - 1] >= (fieldH - 1) * fieldW)
			{
				select[0] = select[0] % fieldH;
			}
			else
			{
				select[0] += fieldW;
			}

			break;
		case _KEY::LEFT:

			if (select[0] % fieldW == 0 && direction == SHIP_H)
			{
				select[0] += fieldW - Size;
			}
			else if (select[0] % fieldW == 0 && direction == SHIP_V)
			{
				select[0] += fieldW - 1;
			}
			else
			{
				select[0]--;
			}

			break;
		case _KEY::RIGHT:

			if ((select[Size - 1] + 1) % fieldW == 0 && direction == SHIP_H)
			{
				select[0] -= (fieldW - Size);
			}
			else if ((select[0] + 1) % fieldW == 0 && direction == SHIP_V)
			{
				select[0] -= (fieldW - 1);
			}
			else
			{
				select[0]++;
			}

			break;
		case 'e': case 'E':

			if (Size == 1)
			{
				if (shipsCnt[1] == 3)
				{
					Size = 3;
				}
				else
				{
					Size = 2;
				}
			}
			else if (Size == 2)
			{
				if (shipsCnt[2] == 2)
				{
					Size = 4;
				}
				else
				{
					Size = 3;
				}
			}
			else if (Size == 3)
			{
				if (shipsCnt[3] == 1)
				{
					select[1] = -1;
					select[2] = -1;

					Size = 1;
				}
				else
				{
					Size = 4;
				}
			}
			else if (Size == 4)
			{
				select[1] = -1;
				select[2] = -1;
				select[3] = -1;

				if (shipsCnt[0] == 4)
				{
					Size = 2;
				}
				else
				{
					Size = 1;
				}
			}

			break;
		case 'r': case 'R':

			if (direction == SHIP_H)
			{
				direction = SHIP_V;
			}
			else if (direction == SHIP_V)
			{
				direction = SHIP_H;
			}

			break;
		case 'g': case 'G':

			FillField(field);
			DrawField(field, select, false);
			shipsPlaced = true;

			break;
		case _KEY::ENTER:

			if (CheckShips(field, ship) == true)
			{
				SetColor(menuC);
				SetPos(18, 25);
				cout << "                               ";

				for (int i = 0; i < Size; i++)
				{
					if (CheckPlace(ship) == 1)
					{
						field[int(select[0] / fieldW) * fieldW + (fieldW - 1) - (Size - 1) + i].sym = shipSym;
					}
					else if (CheckPlace(ship) == 2)
					{
						field[select[0] + i].sym = shipSym;
					}
					else if (CheckPlace(ship) == 3)
					{
						field[(fieldH - 1) * fieldW + (select[0] % fieldW) - (Size - 1) * fieldW + (i * fieldW)].sym = shipSym;
					}
					else if (CheckPlace(ship) == 4)
					{
						field[select[0] + (i * fieldW)].sym = shipSym;
					}
				}

				if (Size == 1)
				{
					shipsCnt[0]++;
				}
				else if (Size == 2)
				{
					shipsCnt[1]++;
				}
				else if (Size == 3)
				{
					shipsCnt[2]++;
				}
				else if (Size == 4)
				{
					shipsCnt[3]++;
				}

				if (int((shipsCnt[0] + shipsCnt[1] + shipsCnt[2] + shipsCnt[3]) / 10))
				{
					shipsPlaced = true;
				}
			}
			else
			{
				system("chcp 1251 > nul");

				SetColor(menuC);
				SetPos(18, 25);
				cout << "Корабль сюда установить нельзя!";

				system("chcp 866 > nul");
			}

			break;
		case _KEY::ESC:

		{
			system("chcp 1251 > nul");

			SetColor(menuC);
			SetPos(18, 30);
			cout << "Выйти в главное меню?(y/n)";
			int answer = _getche();

			if (answer == 'y' || answer == 'Y')
			{
				Sleep(500);
				system("cls");
				return;
			}

			SetPos(18, 30);
			cout << "                            ";

			system("chcp 866 > nul");
		}

		break;
		}

		if (shipsPlaced == false)
		{
			// проверка установлено ли максимальное количество кораблей размера Size
			if (Size == 1)
			{
				if (shipsCnt[0] == 4)
				{
					Size = 2;
				}
			}

			if (Size == 2)
			{
				if (shipsCnt[1] == 3)
				{
					Size = 3;
				}
			}

			if (Size == 3)
			{
				if (shipsCnt[2] == 2)
				{
					Size = 4;
				}
			}

			if (Size == 4)
			{
				select[1] = -1;
				select[2] = -1;
				select[3] = -1;

				if (shipsCnt[3] == 1)
				{
					Size = 1;
				}
			}

			ship.direction = direction;
			ship.Size = Size; // запись размера корабля в корабль

			// проходимся по элементам корабля
			for (int i = 0; i < Size; i++)
			{
				if (CheckPlace(ship) == 1) // проверка уходят ли элементы корабля на следующую строку по горизонтали
				{
					select[i] = (row * fieldW) + (fieldW - 1) - (Size - 1) + i;
				}
				else if (CheckPlace(ship) == 2) // проверка не уходят ли элементы корабля на следующую строку по горизонтали
				{
					select[i] = select[0] + i;
				}
				else if (CheckPlace(ship) == 3) // проверка уходят ли элементы корабля на строку за границей поля по вертикали
				{
					select[i] = (fieldH - 1) * fieldW + col - (Size - 1) * fieldW + (i * fieldW);
				}
				else if (CheckPlace(ship) == 4) // проверка не уходят ли элементы кораблян на строку на границой поля по вертикали
				{
					select[i] = select[0] + (i * fieldW);
				}
			}
		}
	}

	// цикл для удаления призрачных элементов с поля
	for (int i = 0; i < Size; i++)
	{
		SetPos(field[select[i]].pos);

		if (field[select[i]].sym == fieldSym)
		{
			SetColor(fieldC);
		}
		else if (field[select[i]].sym == shipSym)
		{
			SetColor(shipC);
		}

		cout << field[select[i]].sym;
	}

	Game();
}

void ContinueGame()
{
	Game();
}

void Game()
{
	fieldEnemy = new FieldCell[fieldH * fieldW]{}; // обьявление массива ячеек поля врага
	int select = 0; // выбор ячейки вражеского поля для атаки
	int attack = 0; // выбор ячейки поля для атаки компьютером
	int row = fieldIndentY; // строка левого верхнего угла поля врага
	int col = WIDTH - (fieldW * 2 + fieldIndentX - 2); // столбец левого верхнего угла поля врага
	int cnt = 0;

	// заполнение ячеек поля координатами
	for (int i = 0; i < fieldH; i++)
	{
		for (int j = 0; j < fieldW; j++)
		{
			fieldEnemy[i * fieldW + j].pos.row = row + i;
			fieldEnemy[i * fieldW + j].pos.col = col + j * 2;
		}
	}

	FillField(fieldEnemy);

	DrawBoard(fieldEnemy[0].pos); // ф-ция для рисования бардюра поля

	int arr[4] = { select, -1, -1, -1 };
	DrawField(fieldEnemy, arr, true);

	// цикл игры
	while (!gameOver)
	{
		if (turn == TURN_P)
		{
			int key = _getch();

			if (key == CURSOR1 || key == CURSOR2)
			{
				key = _getch();
			}

			switch (key)
			{
			case _KEY::UP:

				if (select < fieldW)
				{
					select += (fieldH - 1) * fieldW;
				}
				else
				{
					select -= fieldW;
				}

				break;
			case _KEY::DOWN:

				if (select >= (fieldH - 1) * fieldW)
				{
					select = select % fieldH;
				}
				else
				{
					select += fieldW;
				}

				break;
			case _KEY::LEFT:

				if (select % fieldW == 0)
				{
					select += fieldW - 1;
				}
				else
				{
					select--;
				}

				break;
			case _KEY::RIGHT:

				if ((select + 1) % fieldW == 0)
				{
					select -= fieldW;
					select++;
				}
				else
				{
					select++;
				}

				break;
			case _KEY::ENTER:

				if (fieldEnemy[select].attacked == false)
				{
					fieldEnemy[select].attacked = true;

					if (fieldEnemy[select].sym != shipSym)
					{
						fieldEnemy[select].sym = attackSym;
						turn = TURN_C;
					}

					if (CheckGameOver(fieldEnemy) == true)
					{
						system("chcp 1251 > nul");
						SetColor(menuC);
						SetPos(16, 35);
						cout << "Вы выиграли!";

						system("chcp 866 > nul");
					}
				}

				break;
			}

			int arr[4] = { select, -1, -1, -1 };
			DrawField(fieldEnemy, arr, true);
		}
		else if (turn == TURN_C)
		{
			Sleep(400);

			do
			{
				attack = RAND(0, (fieldH * fieldW - 1));

			} while (field[attack].attacked == true);

			field[attack].attacked = true;

			if (field[attack].sym != shipSym)
			{
				field[attack].sym = attackSym;
				turn = TURN_P;
			}

			if (CheckGameOver(field) == true)
			{
				system("chcp 1251 > nul");
				SetColor(menuC);
				SetPos(16, 35);
				cout << "Выиграл компьютер!";

				system("chcp 866 > nul");
			}

			int arr[4] = { -1, -1, -1, -1 };
			DrawField(field, arr, false);
		}
	}

	delete[] field;
	delete[] fieldEnemy;
}

void FillField(FieldCell* field)
{
	Ship ship = {};
	int row = 0;
	int col = 0;
	ship.direction = 0;
	ship.Size = 1;

	for (int i = 0; i < fieldH * fieldW; i++)
	{
		field[i].sym = fieldSym;
	}

	for (int i = 0; i < 4; i++) // 0 1 2 3
	{
		ship.Size = i + 1;

		for (int j = i; j < 4; j++) // 0 1 2 3, 1 2 3, 2 3, 3
		{
			do
			{
				ship.direction = RAND(0, 1);

				if (ship.direction == SHIP_H)
				{
					row = RAND(0, fieldH - 1);
					col = RAND(0, fieldH - ship.Size - 1);
				}
				else if (ship.direction == SHIP_V)
				{
					row = RAND(0, fieldH - ship.Size - 1);
					col = RAND(0, fieldH - 1);
				}

				ship.pos.row = row;
				ship.pos.col = col;

			} while (!CheckShips(field, ship));

			for (int k = 0; k < ship.Size; k++)
			{
				if (ship.direction == SHIP_H)
				{
					field[row * fieldH + col + k].sym = shipSym;
				}
				else if (ship.direction == SHIP_V)
				{
					field[(row + k) * fieldH + col].sym = shipSym;
				}
			}
		}
	}
}

void DrawField(FieldCell* field, int* select, bool isEnemy, Ship& ship)
{
	for (int i = 0; i < fieldH; i++)
	{
		for (int j = 0; j < fieldW; j++)
		{
			int pos = i * fieldH + j;

			SetPos(field[pos].pos);

			if (select[0] == pos || select[1] == pos || select[2] == pos || select[3] == pos)
			{
				if (isEnemy == true)
				{
					SetColor(attackGhostC);

					if (field[pos].attacked == true)
					{
						cout << field[pos].sym;
					}
					else
					{
						cout << fieldSym;
					}
				}
				else
				{
					SetColor(shipGhostC);

					for (int k = 0; k < ship.Size; k++)
					{
						if (CheckPlace(ship) == 1)
						{
							SetPos(field[int(select[0] / fieldW) * fieldW + (fieldW - 1) - (ship.Size - 1) + k].pos);
						}
						else if (CheckPlace(ship) == 2)
						{
							SetPos(field[select[0] + k].pos);
						}
						else if (CheckPlace(ship) == 3)
						{
							SetPos(field[(fieldH - 1) * fieldW + (select[0] % fieldW) - (ship.Size - 1) * fieldW + (k * fieldW)].pos);
						}
						else if (CheckPlace(ship) == 4)
						{
							SetPos(field[select[0] + (k * fieldW)].pos);
						}

						cout << shipSym;
					}
				}
			}
			else if (field[pos].sym == fieldSym)
			{
				SetColor(fieldC);
				cout << fieldSym;
			}
			else if (field[pos].sym == shipSym)
			{
				if (isEnemy == true)
				{
					if (field[pos].attacked == true)
					{
						SetColor(attackC);
						cout << shipSym;
					}
					else
					{
						SetColor(fieldC);
						cout << fieldSym;
					}
				}
				else
				{
					if (field[pos].attacked == true)
					{
						SetColor(attackC);
						cout << shipSym;
					}
					else
					{
						SetColor(shipC);
						cout << shipSym;
					}
				}
			}
			else if (field[pos].sym == attackSym)
			{
				SetColor(attackC);
				cout << attackSym;
			}

			SetPos(field[i * fieldH + j].pos.row, field[i * fieldH + j].pos.col + 1);
			SetColor(fieldC);

			cout << ' ';
		}
	}
}
void DrawBoard(Coord& fieldPos)
{
	SetColor(fieldBoardC);

	Coord pos = fieldPos;
	pos.row--;

	SetPos(pos);

	for (int i = 0; i < fieldW; i++)
	{
		cout << char('A' + i) << ' ';
	}

	pos.col -= 2;

	SetPos(pos);
	cout << "  "; // вывод угла бардюра

	for (int i = 0; i < fieldH; i++)
	{
		pos.row++;

		SetPos(pos);
		cout << setw(2) << i + 1;
	}
}

bool CheckGameOver(FieldCell* field)
{
	int cnt = 0;

	for (int i = 0; i < fieldH * fieldW; i++)
	{
		if (field[i].sym == shipSym && field[i].attacked == false)
		{
			cnt++;
		}
	}

	if (cnt == 0)
	{
		gameOver = true;
	}

	return gameOver;
}

int CheckPlace(Ship& ship)
{
	Coord pos = ship.pos;
	int direction = ship.direction;
	int Size = ship.Size;

	int select = pos.row * fieldW + pos.col;

	if (direction == SHIP_H)
	{
		if ((select + Size - 1) % fieldW >= 0 && (select + Size - 1) % fieldW < Size - 1)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else if (direction == SHIP_V)
	{
		if (select + (Size - 1) * fieldW >= fieldH * fieldW)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}
	else
	{
		return -1;
	}
}

bool CheckShips(FieldCell* field, Ship& ship)
{
	int direction = ship.direction;
	int Size = ship.Size;
	int row = ship.pos.row;
	int col = ship.pos.col;
	int iMin = 0, iMax = 3, jMin = 0, jMax = Size + 2;
	int sum = 0;

	int select = row * fieldW + col;

	if (direction == SHIP_H)
	{
		switch (CheckBorder(select + Size - 1))
		{
		case B_UP:
			iMin++;

			break;
		case B_RU:
			iMin++;
			jMax--;

			break;
		case B_RIGHT:
			jMax--;

			break;
		case B_RD:
			iMax--;
			jMax--;

			break;
		case B_DOWN:
			iMax--;

			break;
		case B_LD:
			iMax--;
			jMin++;

			break;
		case B_LEFT:
			jMin++;

			break;
		case B_LU:
			iMin++;
			jMin++;

			break;
		}

		for (int i = iMin; i < iMax; i++)
		{
			for (int j = jMin; j < jMax; j++)
			{
				if (field[(row + i - 1) * fieldH + (col + j - 1)].sym == shipSym)
				{
					sum += int(shipSym);
				}
			}
		}
	}
	else if (direction == SHIP_V)
	{
		switch (CheckBorder(select + (Size - 1) * fieldW))
		{
		case B_UP:
			jMin++;

			break;
		case B_RU:
			jMin++;
			iMax--;

			break;
		case B_RIGHT:
			iMax--;

			break;
		case B_RD:
			jMax--;
			iMax--;

			break;
		case B_DOWN:
			jMax--;

			break;
		case B_LD:
			jMax--;
			iMin++;

			break;
		case B_LEFT:
			iMin++;

			break;
		case B_LU:
			jMin++;
			iMin++;

			break;
		}

		for (int j = jMin; j < jMax; j++)
		{
			for (int i = iMin; i < iMax; i++)
			{
				if (field[(row + j - 1) * fieldH + (col + i - 1)].sym == shipSym)
				{
					sum += int(shipSym);
				}
			}
		}
	}

	SetColor(menuC);

	if (sum == 0)
	{
		return true;
	}
	else if (sum > 0)
	{
		return false;
	}
}

int CheckBorder(int pos)
{
	int direction = -1;
	int row = int(pos / fieldW);
	int col = pos % fieldW;

	if (row == 0)
	{
		direction = B_UP;

		if (col == 0)
		{
			direction = B_LU;
		}
		else if (col == fieldW - 1)
		{
			direction = B_RU;
		}
	}
	else if (row == fieldH - 1)
	{
		direction = B_DOWN;

		if (col == 0)
		{
			direction = B_LD;
		}
		else if (col == fieldW - 1)
		{
			direction = B_RD;
		}
	}
	else if (col == 0)
	{
		direction = B_LEFT;
	}
	else if (col == fieldW - 1)
	{
		direction = B_RIGHT;
	}

	SetColor(menuC);

	return direction;
}

void SetColor(Color& col)
{
	SetColor(col.backgr, col.sym);
}

void SetPos(Coord& pos)
{
	SetPos(pos.row, pos.col);
}
