#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include "board.h"
#include <list>
#include <fstream>
#include <cstdlib>

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  //  The number of cells in a small square
						   //  (usually 3).  The board has
						   //  SquareSize^2 rows and SquareSize^2
						   //  columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

board::board(int sqSize)
	: value(BoardSize + 1, BoardSize + 1)
	, row_conf(BoardSize + 1, BoardSize + 1)
	, col_conf(BoardSize + 1, BoardSize + 1)
	, square_conf(BoardSize + 1, BoardSize + 1)
	// Board constructor
{
	clear();
}

void board::clear()
// Mark all possible values as legal for each board entry
{
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			value[i][j] = Blank;
		}
}

void board::resetCell(int row, int col){
	value[row][col] = Blank;
    updateConflicts();
}

bool board::isSolved(){
	for(int i = 0; i < BoardSize; i++){
		for(int j = 0; j < BoardSize; j++){
			if(this->isBlank(i,j)){
				return false; //found empty cell
			}
		}
	}
	return true; //no empty cells found
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
	char ch;

	clear();

	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			fin >> ch;

			// If the read char is not Blank
			if (ch != '.')
				setCell(i, j, ch - '0');   // Convert char to int
		}

	updateConflicts();
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
	// Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
	// coordinates of the square that i,j is in.  

	return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

void board::updateConflicts()
{
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			if (!isBlank(i, j))
			{
				row_conf[i][value[i][j]] = true;
				col_conf[j][value[i][j]] = true;
				square_conf[squareNumber(i, j)][value[i][j]] = true;
			}
		}
}

void board::printConflicts()
{
	for (int i = 1; i <= BoardSize; i++)
	{
		cout << "ROW " << i << ": ";
		for (int j = 1; j <= BoardSize; j++)
		{
			cout << row_conf[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 1; i <= BoardSize; i++)
	{
		cout << "COL " << i << ": ";
		for (int j = 1; j <= BoardSize; j++)
		{
			cout << col_conf[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 1; i <= BoardSize; i++)
	{
		cout << "SQ " << i << ": ";
		for (int j = 1; j <= BoardSize; j++)
		{
			cout << square_conf[i][j] << " ";
		}
		cout << endl;
	}
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
	for (unsigned i = 0; i < v.size(); i++)
		ostr << v[i] << " " << endl;
	return ostr;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
		return value[i][j];
	else
		throw rangeError("bad value in getCell");
}

void board::setCell(int i, int j, int val)
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize && val >= MinValue && val <= MaxValue)
	{
		if (isBlank(i, j))
		{
			value[i][j] = val;
			updateConflicts();
		}
		else
			cout << "Cell is not blank" << endl;
	}
	else
		throw rangeError("bad value in setCell");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
	if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
		throw rangeError("bad value in setCell");

	return (getCell(i, j) == Blank);
}

void board::print()
// Prints the current board.
{
	for (int i = 1; i <= BoardSize; i++)
	{
		if ((i - 1) % SquareSize == 0)
		{
			cout << " -";
			for (int j = 1; j <= BoardSize; j++)
				cout << "---";
			cout << "-";
			cout << endl;
		}
		for (int j = 1; j <= BoardSize; j++)
		{
			if ((j - 1) % SquareSize == 0)
				cout << "|";
			if (!isBlank(i, j))
				cout << " " << getCell(i, j) << " ";
			else
				cout << "   ";
		}
		cout << "|";
		cout << endl;
	}

	cout << " -";
	for (int j = 1; j <= BoardSize; j++)
		cout << "---";
	cout << "-";
	cout << endl;
}

int main()
{
	ifstream fin;

	// Read the sample grid from the file.
	string fileName = "sudoku2.txt";

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		board b1(SquareSize);

		while (fin && fin.peek() != 'Z')
		{
			b1.initialize(fin);
			b1.print();
			b1.printConflicts();
		}
		system("pause");
	}
	catch (indexRangeError &ex)
	{
		cout << ex.what() << endl;
		system("pause");
		exit(1);
	}

    return 0;
}