#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
#include <iostream>
#include <string>

struct Cell
{
	int xStart;
	int xEnd;
	int YStart;
	int YEnd;
	int coluna;
	int linha;
	int isBomb;
	int marked;
	int revealed;
};

class CampoMinado : public olc::PixelGameEngine
{
	int totalColunas;
	int totalLinhas;
	int totalCellQtd;
	int totalBombQtd;

	int32_t cellSize = 20;
	bool gameOver = 0;
	bool gameWin = 0;
	Cell *cellList;

public:
	CampoMinado(int colunas, int linhas, int bombs)
	{
		totalColunas = colunas;
		totalLinhas = linhas;
		totalCellQtd = totalColunas * totalLinhas;
		totalBombQtd = bombs;
		cellList = new Cell[totalCellQtd];
	}

	void DrawMap()
	{
		for (int linha = 0; linha < totalLinhas; linha++)
		{
			for (int coluna = 0; coluna < totalColunas; coluna++)
			{
				DrawRect(cellSize * coluna, cellSize * linha,
						 cellSize, cellSize, olc::GREEN);
			}
		}
	}
	void GameOver()
	{
		gameOver = true;
		for (int i = 0; i < totalCellQtd; i++)
		{
			if (cellList[i].isBomb == 1)
				cellList[i].revealed = 1;
		}
	}
	int GetCellIndexByColunaLinha(int _linha, int _coluna)
	{
		for (int i = 0; i < totalCellQtd; i++)
		{
			if (cellList[i].coluna == _coluna && cellList[i].linha == _linha)
				return i;
		}
	}

	int GetBombsAround(Cell cell)
	{
		int count = 0;
		for (int linha = -1; linha <= 1; linha++)
		{
			for (int coluna = -1; coluna <= 1; coluna++)
			{
				if (cell.linha + linha < 0 || cell.coluna + coluna < 0 ||
					cell.linha + linha > totalLinhas - 1 ||
					cell.coluna + coluna > totalColunas - 1)
					continue;

				int index = GetCellIndexByColunaLinha(cell.linha + linha, cell.coluna + coluna);

				if (cellList[index].isBomb)
				{
					count++;
				}
			}
		}
		if (count == 0)
		{
			for (int linha = -1; linha <= 1; linha++)
			{
				for (int coluna = -1; coluna <= 1; coluna++)
				{
					if (cell.linha + linha < 0 || cell.coluna + coluna < 0 ||
						cell.linha + linha > totalLinhas - 1 ||
						cell.coluna + coluna > totalColunas - 1)
						continue;

					int index = GetCellIndexByColunaLinha(cell.linha + linha, cell.coluna + coluna);
					cellList[index].revealed = 1;
				}
			}
		}
		return count;
	}

	void CreateCells()
	{
		int index = 0;
		for (int linha = 0; linha < totalLinhas; linha++)
		{
			for (int coluna = 0; coluna < totalColunas; coluna++)
			{
				Cell cell;
				cell.coluna = coluna;
				cell.linha = linha;

				cell.xStart = cellSize * coluna;
				cell.xEnd = cellSize * coluna + cellSize;

				cell.YStart = cellSize * linha;
				cell.YEnd = cellSize * linha + cellSize;

				cell.isBomb = false;
				cell.revealed = false;

				cellList[index] = cell;
				index++;
			}
		}
	}

	void SetBombs()
	{
		int cellIndexCache[totalBombQtd];
		for (int i = 0; i < totalBombQtd; i++)
		{
			cellIndexCache[1] = 0;
		}

		for (int i = 0; i < totalCellQtd; i++)
		{
			cellList[i].marked = 0;
			cellList[i].revealed = 0;
			cellList[i].isBomb = 0;
		}

		for (int i = 0; i < totalBombQtd; i++)
		{
			bool invalidCell = true;
			int cellIndex = 0;

			while (invalidCell)
			{
				cellIndex = GetRandomCell();
				bool contemCell = false;

				for (int i = 0; i < totalBombQtd; i++)
				{
					if (cellIndexCache[i] == cellIndex)
					{
						contemCell = true;
						break;
					}
				}
				if (!contemCell)
					invalidCell = false;
			}
			cellList[cellIndex].isBomb = 1;
			cellIndexCache[i] = cellIndex;
		}
	}

	int GetRandomCell()
	{
		static bool first = true;
		if (first)
		{
			srand(time(NULL)); //seeding for the first time only!
			first = false;
		}
		return 0 + rand() % ((totalCellQtd + 1) - 0);
	}

	void CheckGameWin()
	{
		int notRevealedQtd = 0;
		int bombQtd = 0;
		for (int i = 0; i < totalCellQtd; i++)
		{
			if (cellList[i].revealed == 0)
				notRevealedQtd += 1;
			if (cellList[i].isBomb == 1)
				bombQtd += 1;
		}

		if (notRevealedQtd == bombQtd)
			gameWin = 1;
	}

	void UpdateCells()
	{
		CheckGameWin();
		for (int i = 0; i < totalCellQtd; i++)
		{
			if (cellList[i].revealed == 1)
			{
				if (cellList[i].isBomb == true)
				{
					FillCircle(cellList[i].coluna * cellSize + cellSize / 2,
							   cellList[i].linha * cellSize + cellSize / 2, cellSize / 3, olc::RED);
					GameOver();
				}
				else
				{
					int bombsAroundQtd = GetBombsAround(cellList[i]);
					std::string bombsAround = std::to_string(bombsAroundQtd);
					if (bombsAroundQtd > 0)
						DrawString(cellList[i].coluna * cellSize + cellSize / 2,
								   cellList[i].linha * cellSize + cellSize / 2, bombsAround, olc::WHITE, 1);
				}
			}
			else
			{
				FillRect(cellSize * cellList[i].coluna + 1, cellSize * cellList[i].linha + 1,
						 cellSize - 2, cellSize - 2, olc::GREY);
				if (cellList[i].marked == 1)
				{
					std::string flag = std::to_string(char(197));
					//DrawString(cellList[i].coluna * cellSize + cellSize / 2,
					//		   cellList[i].linha * cellSize + cellSize / 2, flag, olc::WHITE, 1);
					DrawCircle(cellList[i].coluna * cellSize + cellSize / 2,
							   cellList[i].linha * cellSize + cellSize / 2, cellSize / 3, olc::RED);
				}
			}
		}
	}

	int GetCellMouseOver(int mouseX, int mouseY)
	{
		for (int i = 0; i < totalCellQtd; i++)
		{
			if (mouseX >= cellList[i].xStart && mouseX < cellList[i].xStart + cellSize &&
				mouseY >= cellList[i].YStart && mouseY < cellList[i].YStart + cellSize)
			{
				return i;
			}
		}
	}

	void RestartGame()
	{
		SetBombs();
	}

public:
	CampoMinado()
	{
		sAppName = "CampoMinado";
	}

public:
	bool OnUserCreate() override
	{
		CreateCells();
		SetBombs();
		/*for (int i = 0; i < 100; i++)
		{
			std::string index = std::to_string(i);

			std::string cellX = std::to_string(cellList[i].coluna);
			std::string cellY = std::to_string(cellList[i].linha);

			std::string xStart = std::to_string(cellList[i].xStart);
			std::string xEnd = std::to_string(cellList[i].xEnd);

			std::string yStart = std::to_string(cellList[i].YStart);
			std::string yEnd = std::to_string(cellList[i].YEnd);

			cout << "cell index: " + index + " x: " + cellX + " y: " + cellY + " xStart: " + xStart + " xEnd: " + xEnd +
						" yStart: " + yStart + " yEnd: " + yEnd
				 << endl;
		}*/

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		int cellSelectedIndex = GetCellMouseOver(GetMouseX(), GetMouseY());

		Clear(olc::BLACK);

		if (!gameOver)
		{
			if (GetMouse(0).bPressed)
			{
				cellList[cellSelectedIndex].revealed = 1;
			}
			if (GetMouse(1).bPressed)
			{
				if (cellList[cellSelectedIndex].marked == 0)
				{
					cellList[cellSelectedIndex].marked = 1;
				}
				else
				{
					cellList[cellSelectedIndex].marked = 0;
				}
			}
		}
		else if (gameWin)
		{
			gameWin = false;

			DrawString(20,
					   500, "Y O U   W I N !!!", olc::GREEN, 3);
			DrawString(40,
					   550, "\"R\" to restart... ", olc::GREEN, 2);
			if (GetKey(olc::Key::R).bPressed)
			{
				RestartGame();
			}
		}
		else
		{
			gameOver = false;

			DrawString(20,
					   500, "G A M E  O V E R !!!", olc::RED, 3);
			DrawString(40,
					   550, "\"R\" to restart... ", olc::RED, 2);
			if (GetKey(olc::Key::R).bPressed)
			{
				RestartGame();
			}
		}

		DrawMap();
		UpdateCells();

		//Draw debug info
		//std::string mouseX = std::to_string(GetMouseX());
		//std::string mouseY = std::to_string(GetMouseY());
		//std::string cellX = std::to_string(cellList[cellSelectedIndex].coluna);
		//std::string cellY = std::to_string(cellList[cellSelectedIndex].linha);
		//DrawString(300, 0, "mouse: X:" + mouseX + "Y:" + mouseY, olc::RED, 1.5f);
		//DrawString(300, 10, "cell: X:" + cellX + "Y:" + cellY, olc::RED, 1.5f);

		return true;
	}
};
bool is_number(const std::string &s)
{
	return !s.empty() && std::find_if(s.begin(),
									  s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
int main()
{
	bool inMainMenu = true;
	bool inMapSizeMenu = true;
	bool inBombQtdMenu = true;

	int numColunas = 0;
	int numBombs = 0;

	while (inMainMenu)
	{
		string choosedOption;
		cout << "Escolha uma opção e pressione enter :" << endl;
		cout << "1- StartGame" << endl;
		cout << "0- Exit" << endl;
		cout << "-------------------------" << endl;
		cin >> choosedOption;
		cout << "\033[2J\033[1;1H";

		if (choosedOption == "1")
		{
			inMainMenu = false;
		}
		else if (choosedOption == "0")
		{
			exit(0);
		}
		else
		{
			cout << "===================" << endl;
			cout << "=Opção invalida!!!=" << endl;
			cout << "===================" << endl;
			cout << "" << endl;
		}
	}

	while (inMapSizeMenu)
	{
		string choosedOption;
		cout << "Escolha a quantidade de colunas (5-25) :" << endl;
		cout << "-------------------------" << endl;
		cin >> choosedOption;
		cout << "\033[2J\033[1;1H";
		if (is_number(choosedOption))
		{
			int choosedNumber = std::stoi(choosedOption);
			if (choosedNumber >= 5 && choosedNumber <= 25)
			{
				numColunas = choosedNumber;
				inMapSizeMenu = false;
			}
			else
			{
				cout << "======================================================" << endl;
				cout << "=Entrada invalida!!!(Deve ser um numero entre 5 e 25)=" << endl;
				cout << "======================================================" << endl;
				cout << "" << endl;
			}
		}
		else
		{
			cout << "========================================" << endl;
			cout << "=Entrada invalida!!!(Deve ser numerica)=" << endl;
			cout << "========================================" << endl;
			cout << "" << endl;
		}
	}

	while (inBombQtdMenu)
	{
		string choosedOption;
		std::string maxBombsString = std::to_string(numColunas * numColunas - 10);
		cout << "Escolha a quantidade de minas (min: 1 - max :" + maxBombsString + ") :" << endl;
		cout << "-------------------------" << endl;
		cin >> choosedOption;
		cout << "\033[2J\033[1;1H";

		int choosedNumber = std::stoi(choosedOption);
		if (is_number(choosedOption))
		{
			int choosedNumber = std::stoi(choosedOption);
			if (choosedNumber >= 1 && choosedNumber <= numColunas * numColunas - 10)
			{
				numBombs = choosedNumber;
				inBombQtdMenu = false;
			}
			else
			{
				cout << "======================================================" << endl;
				cout << "=Entrada invalida!!!(Deve ser um numero entre 1 e " + maxBombsString + ")=" << endl;
				cout << "======================================================" << endl;
				cout << "" << endl;
			}
		}
		else
		{
			cout << "========================================" << endl;
			cout << "=Entrada invalida!!!(Deve ser numerica)=" << endl;
			cout << "========================================" << endl;
			cout << "" << endl;
		}
	}

	CampoMinado game(numColunas, numColunas, numBombs);
	if (game.Construct(800, 600, 1, 1))
		game.Start();
}