# Five-in-a-Row

## Depencdencies
Requires C++20
* GLFW
* GLAD
* GLM

## Description
A Five-in-a-Row game where you play against an AI. The AI implements a minimax search with alpha-beta pruning and other optimization strategies. A lot of the code is at run compile-time for optimal speed at run-time, including unrolling of the depth-first search recursion. The minimax goal function (the leaf nodes of the searched tree) is parallelized, but the search is necessarily serial to take advantage of the alpha-beta pruning. The implementation also predictively sorts it's children at certain depths to maximize the utility of the alhpa-beta pruning.

The solution comes with a unit testing project, which has been used throughout development of the code to ensure correct behavior and validation of the code units. This means that some parts of the code are heavily templated, so as to be testable with supplementary arguments, the behavior of which need not itself be tested. This gets especially ugly where a function of static linkage needs to manage a threadpool (or any resource), but it's a price worth paying.

The goal function is quite clever. It essentially scores the board based on your progress toward a game winning state, minus the opponent's equivalent progress. To do so, it iterates through all possible Five-in-a-Rows (which have been deduced at compile-time) in parallel, and if one contains only your pieces, the count of those pieces is transformed through a scoring map, the score of which is added to the final sum. I have not yet made sufficient testing to determine the optimal scoring map. However, cubing the count or returning infinity if it's 5 has shown to be very good, indeed.

It's fun, it's fast and it's difficult (I certainly cannot beat it).

## How to play
The goal of the game is to get a sequence of five of your pieces along a cardinal axis or diagonal. You take turns placing your pieces (blue) with the AI's pieces (red).

To place a piece, left-click with your cursor on the cell upon which you wish to place your piece. The game draws a ghost piece on the cell if you are able to place it there with a left-click.

To undo the last move, simply press the Z-key. To let the AI make your move for you, simply press the H-key. To reset the board (and change the starting side), press the R-key. Note that input is ignored while the AI is "thinking".