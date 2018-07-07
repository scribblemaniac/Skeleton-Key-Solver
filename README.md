# Skeleton Key Solver

Skeleton Key is an open source mobile puzzle game developed by Insurgent Games and is available on iOS and Android. You can find out more about it [here](https://web.archive.org/web/20160811085306/http://www.insurgentgames.com/skeleton-key/).

This repository contains a program to solve the puzzles featured in the Skeleton Key game. It was independently developed and is not in any way affiliated with Insurgent Games or its creators.

## How to Use

Note: You do not actually need to use this program to find the solutions to the built-in levels of the Skeleton Key game, we have already run this program on all levels and pushed the results to Github. They can be found [here](https://github.com/scribblemaniac/Skeleton-Key-iOS/blob/master/OtherFiles/levels/solutions.ods).

If you still want to use this program, perhaps for custom levels, then you will first need to create a file containing the level data. It should be in the same format as the [levels.dat file](https://github.com/insurgentgames/Skeleton-Key-iOS/blob/master/OtherFiles/levels/levels.dat) used internally by the Skeleton Key game. Information on creating a levels.dat file can be found [here](https://github.com/insurgentgames/Skeleton-Key-iOS/blob/master/OtherFiles/levels/notes.txt).

Once you have the levels.dat file, you will need to build and run PitFinder.cpp with either levels.dat in the current directory or passed in as the first argument. This will produce a levels-processed.dat file in the current directory, or a file at the path specified in the second argument. Usage using g++ as the compiler:

```bash
g++ PitFinder.cpp -o PitFinder.o
./PitFinder.o [input_levels_path [processed_output_path]]
```

The PitFinder application will not actually find the necessary moves to win, it will only preprocess the level to eliminate some obviously bad paths and to convert the symbols to the ones expected by the Solver. The result of successful execution will be another data file which is used as input for the Solver. First build the Solver and then run it with a levels-processed.dat file in the current directory, or with a path to the input file as the first argument. It may take some time to run depending on the level. Usage using g++ as a compiler:

```bash
g++ Solver.cpp -o Solver.o
./Solver.o [input_levels_path [processed_output_path]]
```

The solutions will be output to the console in a human-friendly format with additional statistics on the time it took to solve each level. The output file will contain a machine-readable solution with the level number, max # of moves, # of moves in solution, and solution, each separated by a semicolon.