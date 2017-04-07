#include "d_matrix.h"
#include "d_except.h"

using namespace std;

typedef int ValueType; // The type of the value in a cell

class board
	// Stores the entire Sudoku board
{
public:
	board(int);
	void clear();
	void initialize(ifstream &fin);
	void print();
	void updateConflicts(int, int, ValueType);
	void printConflicts();
	bool isBlank(int, int);
	ValueType getCell(int, int);
	void setCell(int, int, int);
	void resetCell(int, int, ValueType);
	long getRecursiveCalls();
	void incrementRecursiveCalls();
	bool isSolved();
	bool isLegal(int, int, ValueType);
	void findMostConstrainedBlankCell(int&, int&);
	void findNextBlankCell(int&, int&);
	matrix<ValueType> getValue();
private:

	// The following matrices go from 1 to BoardSize in each
	// dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)
	long numRecursiveCalls = 0;
	matrix<ValueType> value;
	matrix<bool> row_conf;
	matrix<bool> col_conf;
	matrix<bool> square_conf;
};