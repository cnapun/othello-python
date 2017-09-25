import ctypes
import os

_file = 'testlib.so'
_path = os.path.join(*(os.path.split(__file__)[:-1] + (_file,)))
_mod = ctypes.cdll.LoadLibrary(_path)

up_moves = _mod.up_moves
up_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
up_moves.restype = ctypes.c_uint64

down_moves = _mod.down_moves
down_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
down_moves.restype = ctypes.c_uint64

left_moves = _mod.left_moves
left_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
left_moves.restype = ctypes.c_uint64

right_moves = _mod.right_moves
right_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
right_moves.restype = ctypes.c_uint64

ur_moves = _mod.ur_moves
ur_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
ur_moves.restype = ctypes.c_uint64

ul_moves = _mod.ul_moves
ul_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
ul_moves.restype = ctypes.c_uint64

dr_moves = _mod.dr_moves
dr_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
dr_moves.restype = ctypes.c_uint64

dl_moves = _mod.dl_moves
dl_moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
dl_moves.restype = ctypes.c_uint64

moves = _mod.moves
moves.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
moves.restype = ctypes.c_uint64

board_y = _mod.board_y
board_y.argtype = ctypes.c_uint64
board_y.restype = ctypes.c_int

board_x = _mod.board_x
board_x.argtype = ctypes.c_uint64
board_x.restype = ctypes.c_int

move_player = _mod.move_player
move_player.argtypes = (ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint64)
move_player.restype = ctypes.c_uint64

bc = _mod.bitCount
bc.argtype = ctypes.c_uint64
bc.restype = ctypes.c_int

end_of_game = _mod.endofGame
end_of_game.argtypes = (ctypes.c_uint64, ctypes.c_uint64)
end_of_game.restype = ctypes.c_int
