#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <utility>
#include <cmath>
#include <unordered_set>
#include <stack>
using namespace std;

#define MAP_WIDTH 6
#define MAP_HEIGHT 8

/*
 * Deep mode searches for any possible solution with the desired number of moves,
 * not necessarily the best solution. It does a depth-first rather than a breadth-first
 * search and does not use the tried set optimization. The purpose of it is to
 * find any possible solution with minimal memory consumption and few optimizations
 * to minimize accidentally skipping over the solution
 */
//#define DEEP_MODE

struct MapPair {
	string currentMoves;
	char map[MAP_HEIGHT][MAP_WIDTH];
};

#ifdef DEEP_MODE
stack<MapPair> moveStructure;
#else
queue<MapPair> moveStructure;
// Set to hold all level configurations that have already been tried
unordered_set<string> tried;
#endif
char tempMap[MAP_WIDTH * MAP_HEIGHT + 1];

void printMap(char map[][MAP_WIDTH]) {
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			cout << map[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

/* Attempts to move whatever is in position (fromX,fromY) to (destX,destY)
 * Only moves keys and ignores other blocks. Handles moving from or to open doors,
 * onto and off switches, into chests, into obstacles.
 *
 * Returns false if the moves causes a key to go into a "dead space", true otherwise.
 * hasMoved is set to true if any of the keys actually changed position.
 * hasSwitched is set to true is a key moves onto a switch space.
 */
bool moveTo(int fromY, int fromX, int destY, int destX, char map[][MAP_WIDTH], bool &hasMoved, bool &hasSwitched) {
	char replaceChar = 'a';
	switch(map[fromY][fromX]) {
	case 'k':
		replaceChar = 'a';
		break;
	case 'S':
		replaceChar = 's';
		break;
	case '#':
		replaceChar = 'O';
		break;
	default:
		return true;
	}

	switch(map[destY][destX]) {
	case '.':
		return false;
	case 'a':
		map[destY][destX] = 'k';
		break;
	case 's':
		map[destY][destX] = 'S';
		hasSwitched = true;
		break;
	case 'O':
		map[destY][destX] = '#';
		break;
	case 'x':
		map[destY][destX] = 'X';
		break;
	default:
		return true;
	}

	hasMoved = true;
	map[fromY][fromX] = replaceChar;
	return true;
}

/* Closes all open doors that do not have a key in them,
 * and opens all closed doors. Should only be run after the keys
 * have been moved.
 */
void switchAll(char map[][MAP_WIDTH]) {
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if(map[i][j] == 'O') map[i][j] = 'l';
			else if(map[i][j] == 'l') map[i][j] = 'O';
		}
	}
}

// Moves the keys to the left
bool moveLeft(char map[][MAP_WIDTH], bool &hasSwitched) {
	bool hasMoved = false;
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 1; j < MAP_WIDTH; j++) {
			if(!moveTo(i, j, i, j-1, map, hasMoved, hasSwitched)) return false;
		}
	}
	return hasMoved;
}

// Moves the keys to the right
bool moveRight(char map[][MAP_WIDTH], bool &hasSwitched) {
	bool hasMoved = false;
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = MAP_WIDTH - 2; j >= 0; j--) {
			if(!moveTo(i, j, i, j+1, map, hasMoved, hasSwitched)) return false;
		}
	}
	return hasMoved;
}

// Moves the keys down
bool moveDown(char map[][MAP_WIDTH], bool &hasSwitched) {
	bool hasMoved = false;
	for(int i = MAP_HEIGHT - 2; i >= 0; i--) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if(!moveTo(i, j, i+1, j, map, hasMoved, hasSwitched)) return false;
		}
	}
	return hasMoved;
}

/* Checks if the current move is solved, returns true if it is and false if it isn't.
 * If it isn't solved, then it will check if moving left, right, or down is valid
 * and add those moves to the move structure to eventually be checked.
 * checkMove is not directly called on these to allow for the flexibility of
 * breadth- or depth-first search.
 */
bool checkMove(string currentMoves, int maxMoves, char map[][MAP_WIDTH]) {
	//cout << "Checking " << currentMoves << endl;
	bool isSolved = true;
	for(int i = 0; i < MAP_HEIGHT && isSolved; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if(map[i][j] == 'x') {
				isSolved = false;
				break;
			}
			//cout << map[i][j];
		}
	}
	//cout << endl;
	if(isSolved) return true;
	if(maxMoves >= 0 && currentMoves.length() >= maxMoves) return false;

	MapPair leftMap;
	leftMap.currentMoves = currentMoves + '<';
	memcpy(leftMap.map, map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
	bool hasSwitched = false;
	string tempMapStr;
	if(moveLeft(leftMap.map, hasSwitched)) {
		if(hasSwitched) switchAll(leftMap.map);
		memcpy(tempMap, leftMap.map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
		tempMapStr = string(tempMap);
#ifdef DEEP_MODE
		moveStructure.push(leftMap);
#else
		if(!tried.count(tempMapStr)) {
			moveStructure.push(leftMap);
			tried.insert(tempMapStr);
		}
#endif
	}

	MapPair rightMap;
	rightMap.currentMoves = currentMoves + '>';
	memcpy(rightMap.map, map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
	hasSwitched = false;
	if(moveRight(rightMap.map, hasSwitched)) {
		if(hasSwitched) switchAll(rightMap.map);
		memcpy(tempMap, rightMap.map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
		tempMapStr = string(tempMap);
#ifdef DEEP_MODE
		moveStructure.push(rightMap);
#else
		if(!tried.count(tempMapStr)) {
			moveStructure.push(rightMap);
			tried.insert(tempMapStr);
		}
#endif
	}

	MapPair downMap;
	downMap.currentMoves = currentMoves + 'v';
	memcpy(downMap.map, map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
	hasSwitched = false;
	if(moveDown(downMap.map, hasSwitched)) {
		if(hasSwitched) switchAll(downMap.map);
		memcpy(tempMap, downMap.map, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
		tempMapStr = string(tempMap);
#ifdef DEEP_MODE
		moveStructure.push(rightMap);
#else
		if(!tried.count(tempMapStr)) {
			moveStructure.push(downMap);
			tried.insert(tempMapStr);
		}
#endif
	}

	return false;
}

int main(int argc, char* argv[]) {
	// Check for help flag
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-h") || strcmp(argv[i], "--help")) {
			cout << "Usage: " << argv[0] << " [input_levels_path [processed_output_path]]" << endl;
			return 0;
		}
	}

	// Open input file
	string ifilePath = "levels-processed.dat";
	if(argc > 1) {
		ifilePath = argv[1];
	}
	ifstream ifile(ifilePath);
	if(!ifile.good()) {
		cerr << "Error: Could not open input file" << endl;
		return -1;
	}

	// Open output file
	string ofilePath = "levels-answers.txt";
	if(argc > 2) {
		ofilePath = argv[2];
	}
	ofstream ofile(ofilePath);
	if(!ofile.good()) {
		cerr << "Error: Could not open output file" << endl;
		return -1;
	}


	int levelNum, moves;
	char map[MAP_HEIGHT][MAP_WIDTH];
	while(!ifile.eof()) {
		// Read level from line in input file
		ifile >> levelNum;
		ifile.ignore(1);
		ifile.get(tempMap, MAP_WIDTH * MAP_HEIGHT + 1);
		ifile.ignore(1);
		ifile >> moves;
		ifile.ignore(256, '\n');
		if(ifile.eof()) {
			break;
		}
		// If you want to skip or filter levels, uncomment the lines below and modify as needed
		/*if(levelNum != 78 || moves >= 25) {
			cout << "Skipping level #" << levelNum << ". Probably too hard." << endl;
			continue;
		}*/
		cout << "Solving level #" << levelNum << endl;
#ifdef DEEP_MODE
		if(moves < 0) {
			cerr << "Error: cannot use a negative number of moves with deep mode" << endl;
			return -1;
		}
#endif
		memcpy(map, tempMap, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
		ofile << levelNum << ";" << moves << ";";

		// Adds the initial board configuration
		checkMove("", moves, map);
		string solution = "";
		// For operating stats
		float boardsChecked = 0;
		clock_t startTime = clock();
		// While there are still moves to test
		while(!moveStructure.empty()) {
			boardsChecked++;
			// Pick one possible move and check it, adding new moves if necessary
#ifdef DEEP_MODE
			MapPair currentMove = moveStructure.top();
#else
			MapPair currentMove = moveStructure.front();
#endif
			moveStructure.pop();
			if(checkMove(currentMove.currentMoves, moves, currentMove.map)) {
				solution = currentMove.currentMoves;
				break;
			}
		}
		// If we've found a solution, clear the rest of the queue/stack
		while(!moveStructure.empty()) moveStructure.pop();
		// and clear tried if necessary
#ifndef DEEP_MODE
		tried.clear();
#endif
		clock_t endTime = clock();

		// Output results
		if(solution.empty()) {
			cout << "Could not find a solution for level #" << levelNum << " after " << ((endTime - startTime) / (float)CLOCKS_PER_SEC) << " seconds" << endl;
			ofile << "-1;" << endl;
		}
		else {
			cout << "Found solution for level #" << levelNum << " after " << ((endTime - startTime) / (float)CLOCKS_PER_SEC) << " seconds:" << solution << endl;
			ofile << solution.length() << ";" << solution << endl;
		}
	}

	// Cleanup
	ifile.close();
	ofile.close();
	return 0;
}
