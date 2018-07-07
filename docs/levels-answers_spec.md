# Specifications for levels-answers.txt

Each line represent a level from the input file for Solver. The lines follow this format:
```
<level_number>;<max_moves>;<solution_moves>;[solution]
```

`level_number`: The level number for the current line

`max_moves`: The maximum number of moves tested for (provided in input)

`solution_moves`: Number of moves used in solution. -1 if a solution was not found. If deep mode is disabled, this should be the minimum number of moves that the level can be solved in

`solution`: A solution to the puzzle. The '<','v', and '>' characters are used to indicate the direction to move, and should be read from left to right.
