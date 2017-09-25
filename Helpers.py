from ctypes import *
from ctypes_test import *
from BB import *

# board_y counts from top down

up_mask = c_longlong(0xFF00000000000000)
down_mask = c_longlong(0x00000000000000FF)
left_mask = c_longlong(0x8080808080808080)
right_mask = c_longlong(0x0101010101010101)


def print_board(board):
    pos = 0x8000000000000000
    for i in range(8):
        a = []
        for i in range(8):
            if pos & board != 0:
                a.append('1')
            else:
                a.append('0')
            pos >>= 1
        print(''.join(a))
    print()


def get_bb(x, y):
    return (1 << (7 - x)) << ((7 - y) * 8)


def heuristic(p1, p2):
    return bin(p1).count("1")


def listmoves(a):
    l = []
    up = 0x8000000000000000
    while up != 0:
        ar = up & a
        if ar != 0:
            l.append(ar)
        up >>= 1
    return l

if __name__=='__main__':
    a = Bitboard()
    print_board(a.best_move(8))
    # print_board(a.moves())
