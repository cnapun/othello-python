from BB import *
from ctypes_test import *

l = Learner()
bb = Bitboard(4, heuristic=l.heuristic)

print(bb.best_move(2))
print(do_everything(bb.p1, bb.p2, 2, False, l.wh.astype(np.float32), l.wo.astype(np.float32)))
