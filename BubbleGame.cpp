#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <algorithm>
#include <random>

using gameBoard = std::array<int, 132>;
using boardCoords = std::array<int, 2>;
using indexVec = std::vector<int>;
using boardMatrix = std::vector<std::vector<int>>;
using iVec = std::vector<int>;

/**
 * Convert a 'flat' vector index to a game board coordinate
 */
boardCoords indexToCoord(int index){
	int rowCoord = index / 11;
	return {rowCoord, index - rowCoord*11};
}

/**
 * Converts a board coordinate in the 'flat' index for the corresponding board vector
 */
int coordToIndex(boardCoords coords){
	return coords[0]*11 + coords[1];
}

/**
 * Function to print out the game board
 */
void printBoard(gameBoard& board){
	for(int i=0; i<12; ++i){
		for(int j=0; j<11; ++j){
			int index = coordToIndex(boardCoords{i,j});
			std::cout << board[index] << " ";
		}
		std::cout << "\n";
	}
}

/**
 * Load the game board from the file that the Python script saves
 */
gameBoard loadBoard(){
	gameBoard inputBoard;
	int inputInt;
	std::ifstream fst("boarddata.txt");

	for(int i=0; i<132; ++i){
		fst >> inputInt;
		inputBoard[i] = inputInt;
	}
	return inputBoard;
}

/**
 * Recursion algorithm to find neighbors with same value
 */
void findConnectedRecursion(int targetVal, int index, gameBoard& board, gameBoard& connectedBoard){
	// First check if entrance is the correct value
	if(board[index] == targetVal){
		// Check if already visited
		if(connectedBoard[index] != 1){
			// Mark as visited
			connectedBoard[index] = 1;

			// Now check the neighbors
			boardCoords coords = indexToCoord(index);
			int iy = coords[0];
			int ix = coords[1];
			if(iy > 0){findConnectedRecursion(targetVal, coordToIndex(boardCoords{iy-1, ix}), board, connectedBoard);}
			if(iy < 11){findConnectedRecursion(targetVal, coordToIndex(boardCoords{iy+1, ix}), board, connectedBoard);}
			if(ix > 0){findConnectedRecursion(targetVal, coordToIndex(boardCoords{iy, ix-1}), board, connectedBoard);}
			if(ix < 10){findConnectedRecursion(targetVal, coordToIndex(boardCoords{iy, ix+1}), board, connectedBoard);}
		}
	}
}

/**
 * Starter for the find neighbor recursion algorithm
 */
indexVec findConnected(gameBoard& board, int index){
	int targetVal = board[index];
	gameBoard connectedBoard = {};

	// Recursion to find the neighbors
	findConnectedRecursion(targetVal, index, board, connectedBoard);

	// Find the indices that were connected
	indexVec connectedIndices;
	for(int i=0; i<132; ++i){
		if(connectedBoard[i] == 1){
			connectedIndices.push_back(i);
		}
	}
	return connectedIndices;
}

/**
 * Returns the sum of a vector
 */
int sumVec(std::vector<int> vec){
	int res = 0;
	for(int i : vec){
		res += i;
	}
	return res;
}

/**
 * Swap two elements in a vector
 */
void swapElements(indexVec& vec, int i, int j){
	int temp = vec[i];
	vec[i] = vec[j];
	vec[j] = temp;
}

/**
 * Move the 0's in a vector to the front, leaving the order of the other numbers unchanged
 */
void moveZeroToFront(indexVec& vec){
	int j=11;
	for(int i=11; i>=0; --i){
		if(vec[i] != 0){
			swapElements(vec, i, j);
			--j;
		}
	}
}

/**
 * Update the game board by moving zeroes to the top, and complete zero columns to the left
 */
gameBoard updateBoard(gameBoard& board){
	// Find all non-empty columns
	gameBoard updatedBoard = {};
	std::vector<indexVec> colVec;
	for (int iCol=0; iCol<11; ++iCol){
		indexVec col;
		for(int iRow=0; iRow<12; ++iRow){
			col.push_back(board[iRow*11+iCol]);
		}

		// If sum not zero, move all 0's to the front and save the column
		if(sumVec(col) != 0){
			moveZeroToFront(col);
			colVec.push_back(col);
		}
	}

	// Now add back the columns
	int nrNonZero = colVec.size();
	int colVecIndex;
	colVecIndex = 0;
	for (int iCol=11-nrNonZero; iCol<11; ++iCol){
		for(int iRow=0; iRow<12; ++iRow){
			updatedBoard[iRow*11 + iCol] = colVec[colVecIndex][iRow];
		}
		++colVecIndex;
	}
	return updatedBoard;
}

gameBoard setBoardVals(gameBoard board, iVec indicesVec, int targetVal){
	for (int index : indicesVec){
		board[index] = targetVal;
	}
	return board;
}

void setBoardValsInPlace(gameBoard& board, iVec indicesVec, int targetVal){
	for (int index : indicesVec){
			board[index] = targetVal;
	}
	return;
}

iVec solutionClickVec = {-1};
//int deepCounter = 0;

void findSolutionRecursive(gameBoard& boardIn, iVec clickListInt, int bubblesPopped, int& deepCounter){
	gameBoard visitedBoard = {};  // Keep track of which groups have been visited

	// Main loop over all elements of the board
	for(int i=0; i<132; ++i){
		// DeepCounter test
		if(bubblesPopped > 30){
			++deepCounter;
		}else{
			deepCounter = 0;
		}

		if(deepCounter > 20000){return;}

		// First look if solution is found - break all other recursions
		if (solutionClickVec[0] != -1){
			return;
		}

		// Check value of bubblesPopped and see if done
		if(bubblesPopped == 132){
			std::cout << "Solution found!\n";
			solutionClickVec = clickListInt;
			return;
		}

		// Check if 0 or visited
		if(boardIn[i] == 0 || visitedBoard[i] == 1){continue;}
		// Check if connected
		indexVec connected = findConnected(boardIn, i);
		// Check if it by itself
		if(connected.size() == 1){continue;}
		// It is in a group - pop the elements, update the board and move along
		setBoardValsInPlace(visitedBoard, connected, 1);
		gameBoard newBoard = setBoardVals(boardIn, connected, 0);
		newBoard = updateBoard(newBoard);
		iVec newClickVec = clickListInt;
		newClickVec.push_back(i);

		/*
		boardCoords coord = indexToCoord(i);
		std::cout << i << ", (" << coord[0] << "," << coord[1] << ")\n";
		std::cout << "Total popped: " << bubblesPopped+connected.size() <<
				", deepCounter: " << deepCounter << "\n";
		printBoard(newBoard);
		std::cout << "\n";
		*/

		findSolutionRecursive(newBoard, newClickVec, bubblesPopped+connected.size(), deepCounter);
	}
}

void findSolutionParallel(gameBoard& inputBoard){
	// First find all possible clicks for the original board
	gameBoard visitedBoard = {};
	iVec possibleClicks;
	for(int i=0; i<132; ++i){
		// Check if in group already found
		if(visitedBoard[i] == 1){continue;}

		//Else check if group and add the index
		indexVec connected = findConnected(inputBoard, i);
		if(connected.size() != 1){
			setBoardValsInPlace(visitedBoard, connected, 1);
			possibleClicks.push_back(i);
		}
	}

	// Shuffle the possible clicks, except the first one
	auto rd = std::random_device {};
	auto rng = std::default_random_engine { rd() };
	std::shuffle(std::begin(possibleClicks)+1, std::end(possibleClicks), rng);
	int clickVecSize = possibleClicks.size();

	// Now solve them in parallel!
	#pragma omp parallel for num_threads(3)
	for(auto i=0; i<clickVecSize; ++i){
		int index = possibleClicks[i];
		int deepCounter = 0;
		indexVec connected = findConnected(inputBoard, index);
		gameBoard newBoard = setBoardVals(inputBoard, connected, 0);
		newBoard = updateBoard(newBoard);
		iVec newClickVec = {index};
		findSolutionRecursive(newBoard, newClickVec, connected.size(), deepCounter);
	}
}


int main(int argc, char **argv) {
	gameBoard board = loadBoard();
	printBoard(board);
	std::cout << "\n";

	iVec clickListIn;
	//findSolutionRecursive(board, clickListIn, 0);
	findSolutionParallel(board);

	/*
	for(int i : solutionClickVec){
		boardCoords coord = indexToCoord(i);
		std::cout << i << ", (" << coord[0] << "," << coord[1] << ")\n";
		indexVec connected = findConnected(board, i);
		board = setBoardVals(board, connected, 0);
		board = updateBoard(board);
		printBoard(board);
		std::cout << "\n";
	}
	*/

	std::ofstream ost{"solution.txt"};
	for(int i : solutionClickVec){
		boardCoords dims = indexToCoord(i);
		ost << dims[0] << " " << dims[1] << "\n";
	}

	return 0;
}
