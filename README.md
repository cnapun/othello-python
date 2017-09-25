# othello-python

## Usage
Run `BB.py` with the desired parameters to train (or at least attempt to train) the model.
To play the game, run `rest_test.py` with the path to the saved weights specified, and then open `web/interface.html`.

## What the Files Do
* `BB.py` contains the core game functionality. The Bitboard class represents the actual board, Learner is the class that contains the components for learning.
* `testlib.c` contains all of the state transition operations, and it is connected to the other files through `ctypes_test.py`
* `rest_test.py` contains the code for the Flask REST API
* `web/` contains the components used for the interface
