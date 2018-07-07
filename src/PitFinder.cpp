#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#define MAP_WIDTH 6
#define MAP_HEIGHT 8

// Recursively backtracks through the level from the position (x,y).
// This function only moves right, left and up. If a key ends up in any
// spot not expored by this function, it is impossible for that key to
// get to a chest.
void fillUp(int y, int x, char map[][MAP_WIDTH]) {
	switch(map[y][x]) {
	case '*':
	case 'a':
	case 'x':
	case 'k':
	case 's':
	case 'O':
	case 'l':
		return;
	case '.':
		map[y][x] = 'a';
		break;
	case '!':
		map[y][x] = 'k';
		break;
	case 'o':
		map[y][x] = 's';
		break;
	case '|':
	case '-':
		map[y][x] = 'l';
		break;
	case '#':
	case '=':
		map[y][x] = 'O';
		break;
	default:
		cout << "Warning: Unknown character '" << map[y][x] << "'" << endl;
	}
	if(x > 0) fillUp(y, x-1, map);
	if(x < MAP_WIDTH - 1) fillUp(y, x+1, map);
	if(y > 0) fillUp(y-1, x, map);
}

// Replaces any characters which shouldn't be in the output with the correct symbols
void cleanup(int level, char map[][MAP_WIDTH]) {
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			switch(map[y][x]) {
			case '!':
				cout << "Warning: Level #" << level << " is impossible" << endl;
				break;
			case 'o':
				map[y][x] = '.';
				break;
			case '|':
			case '-':
				map[y][x] = 'l';
				break;
			case '#':
			case '=':
				map[y][x] = 'O';
				break;
			}
		}
	}
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
	string ifilePath = "levels.dat";
	if(argc > 1) {
		ifilePath = argv[1];
	}
	ifstream ifile(ifilePath);
	if(!ifile.good()) {
		cerr << "Error: Could not open input file" << endl;
		return -1;
	}

	// Open output file
	string ofilePath = "levels-processed.dat";
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
	char crap[256]; // Data at the end of the line which is not used, but is stored to include in the output
	while(!ifile.eof()) {
		// Read level from line in input file
		char tempMap[MAP_WIDTH * MAP_HEIGHT + 1];
		ifile >> levelNum;
		ifile.ignore(1);
		ifile.get(tempMap, MAP_WIDTH * MAP_HEIGHT + 1);
		ifile.ignore(1);
		ifile >> moves;
		ifile.getline(crap, 256);
		if(ifile.eof()) {
			break;
		}
		memcpy(map, tempMap, sizeof(char) * MAP_HEIGHT * MAP_WIDTH);
		ofile << levelNum << ";";
		// Find all chests
		for(int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				if(map[i][j] == 'x') {
					// Begin recursive backtracking through the level from the
					// spaces directly adjacent (but not below) any found chest
					if(j > 0) fillUp(i, j-1, map);
					if(j < MAP_WIDTH - 1) fillUp(i, j+1, map);
					if(i > 0) fillUp(i-1, j, map);
				}
			}
		}
		// Convert any remaining symbols
		cleanup(levelNum, map);

		// Output resulting level data
		for(int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				ofile << map[i][j];
			}
		}
		ofile << ";" << moves << crap << endl;
	}

	// Cleanup
	ifile.close();
	ofile.close();
	return 0;
}
