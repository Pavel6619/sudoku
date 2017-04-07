#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include "board.h"
#include <list>
#include <fstream>
#include <cstdlib>
#include <string>

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

bool board::isSolved(){
	for(int i = 1; i <= BoardSize; i++){
		for(int j = 1; j <= BoardSize; j++){
			if(isBlank(i,j)){
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
				//updateConflicts(i, j, value[i][j]);
			
		}
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
	// Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
	// coordinates of the square that i,j is in.  

	return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

void board::updateConflicts(int i, int j, ValueType val)
{
			if (!isBlank(i, j))
			{
				row_conf[i][val] = true;
				col_conf[j][val] = true;
				square_conf[squareNumber(i, j)][val] = true;
			}
			else
			{
				row_conf[i][val] = false;
				col_conf[j][val] = false;
				square_conf[squareNumber(i, j)][val] = false;
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
			updateConflicts(i, j, val);
		}
		else
			cout << "Cell is not blank" << endl;
	}
	else
		throw rangeError("bad value in setCell");
}

void board::resetCell(int row, int col, ValueType val) {
	value[row][col] = Blank;
	updateConflicts(row, col, val);
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
	if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
		throw rangeError("bad value in setCell");

	return (getCell(i, j) == Blank);
}

bool board::isLegal(int i, int j, ValueType val)
{
	int sq = squareNumber(i, j);
	return !(row_conf[i][val] || col_conf[j][val] || square_conf[sq][val]);
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

void board::findMostConstrainedBlankCell(int& i, int& j)
{
	int most_constrained_sum = 0;
	int gen_sum = 0;
	for (int k = 1; k <= BoardSize; k++)
	{
		for (int m = 1; m <= BoardSize; m++)
		{
			if (isBlank(k, m))
			{
				for (int n = 1; n <= BoardSize; n++)
				{
					//Increment the general sum for a given cell if there is a conflict for this digit
					gen_sum += (row_conf[k][n] || col_conf[m][n] || square_conf[squareNumber(k, m)][n]);
				}
				if (gen_sum > most_constrained_sum)
				{
					most_constrained_sum = gen_sum;
					i = k;
					j = m;
				}
				gen_sum = 0;
			}
		}
	}
}

void board::findNextBlankCell(int& i, int& j)
{
	for (int k = 1; k <= BoardSize; k++)
	{
		for (int m = 1; m <= BoardSize; m++)
		{
			if (isBlank(k, m))
			{
				i = k;
				j = m;
				return;
			}
		}
	}
}

matrix<ValueType> board::getValue()
{
	return value;
}

void board::incrementRecursiveCalls()
{
	numRecursiveCalls++;
}

long board::getRecursiveCalls()
{
	return numRecursiveCalls;
}

bool solve(board& b)
{
	b.incrementRecursiveCalls();
	int i, j;
	if (b.isSolved())
	{
		b.print();
		cout << "Solved" << endl;
		return true;
	}
	else
	{
		//b.findNextBlankCell(i, j);
		b.findMostConstrainedBlankCell(i, j);
		for (int k = 1; k <= 9; k++)
		{
			if (b.isLegal(i, j, k))
			{
				b.setCell(i, j, k);
				if (!solve(b))
					b.resetCell(i, j, k);
			}
		}
		return false;
	}
}

int main()
{
	ifstream fin;
    string fileName;
    int numCalls[3];

    /*cout << "Sudoku board 1, 2, or 3?";

    do{
		cin >> choice;
		if((choice.find_first_not_of( "0123456789" ) == string::npos)){
			c = atoi(choice.c_str());
			switch(c) {
				case 1:
					fileName = "sudoku1.txt";
					break;
				case 2:
					fileName = "sudoku2.txt";
					break;
				case 3:
					fileName = "sudoku3.txt";
					break;
				default:
					cout << endl << "enter a number between 1 and 3";
			}
		}
		else{
			cin.clear();
			c = 4;
			cout << endl << "enter a number between 1 and 3";
		}

    }while(c < 1 || c > 3);*/

    for(int c = 1; c <= 3; c++){
        switch(c) {
            case 1:
                fileName = "sudoku1.txt";
                break;
            case 2:
                fileName = "sudoku2.txt";
                break;
            case 3:
                fileName = "sudoku3.txt";
                break;
        }
        fin.open(fileName.c_str());
        if (!fin)
        {
            cerr << "Cannot open " << fileName << endl;
            exit(1);
        }

        try
        {
            board b1(SquareSize);
            int i = 1, j = 1;
            while (fin && fin.peek() != 'Z')
            {
                b1.initialize(fin);
                b1.print();
                solve(b1);
            }
            numCalls[c-1] = b1.getRecursiveCalls();
            cout << "Total recursive calls: " << b1.getRecursiveCalls() << endl;
        }
        catch (indexRangeError &ex)
        {
            cout << ex.what() << endl;
            system("pause");
            exit(1);
        }

        fin.close();
    }

    int averageNumCalls = (numCalls[0] + numCalls[1] + numCalls[2]) / 3;
    cout << endl << "Average number of recursive calls: " << averageNumCalls << endl;
	system("pause");
    return 0;
}