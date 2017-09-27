# othello-python

This project attempts to learn a heuristic function for board position in Othello (Reversi). It is by no means efficient, as Python is used for the search, but it does work to some extent. If the weight initialization is lucky, it will learn to play acceptably mediocrely, but will not place a very high value on corner positions. This was written with a minimal (read: no) understanding of reinforcement learning, so there are certainly major flaws in my reasoning. I attempted to base this off of a [blog post](karpathy.github.io/2016/05/31/rl/) about playing Pong using Policy Gradients. I may implement MCTS in the future. There is likely not enough complexity in the network to capture all intricacies of the game, but from experience with a hardcoded heuristic, it seems reasonable to expect fairly logical play.

## Usage
Run `BB.py` with the desired parameters to train (or at least attempt to train) the model.
To play the game, run `rest_test.py` with the path to the saved weights specified, and then open `web/interface.html`.
To compile, run `g++ -I /path/to/eigen3 -O3 -shared -o testlib1.so -fpic bitboard.cpp`.

## What the Files Do
* `BB.py` contains the core game functionality. The Bitboard class represents the actual board, Learner is the class that contains the components for learning.
* `testlib.c` contains all of the state transition operations, and it is connected to the other files through `ctypes_test.py` (specify the .so file that you compile it to)
* `bitboard.cpp` contains all of the code in `testlib.c`, but also has code for Minimax search, and can take weights from the Python files for the heuristic 
* `rest_test.py` contains the code for the Flask REST API
* `web/` contains the components used for the interface
