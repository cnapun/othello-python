from ctypes_test import *
from ctypes import *
import numpy as np
import os
import sys
import time


class Bitboard:
    def __init__(self, maxdepth=6, heuristic=None):
        self.p1 = 0x0000000810000000
        self.p2 = 0x0000001008000000
        self.maxdepth = maxdepth
        self.p1turn = True
        self.heuristic = heuristic or Bitboard.heuristic

    def __str__(self):
        pos = 0x8000000000000000
        a = []
        for _ in range(8):
            b = []
            for _ in range(8):
                if pos & self.p1:
                    b.append('1')
                elif pos & self.p2:
                    b.append('2')
                else:
                    b.append('0')
                pos >>= 1
            a.append(''.join(b))
            a.append('\n')
        return ''.join(a)

    @staticmethod
    def print_board(board):
        pos = 0x8000000000000000
        for _ in range(8):
            a = []
            for _ in range(8):
                if pos & board != 0:
                    a.append('1')
                else:
                    a.append('0')
                pos >>= 1
            print(''.join(a))
        print()

    def moves(self):
        if self.p1turn:
            return moves(self.p1, self.p2)
        else:
            return moves(self.p2, self.p1)

    def make_move(self, move):
        pot = self.moves()
        if self.p1turn:
            if pot & move:
                self.p1 = move_player(move, self.p1, self.p2)
                self.p2 = self.p2 & ~self.p1
            else:
                print(self)
                self.print_board(move)
                raise Exception('Illegal Move')
            p2_canmove = moves(self.p2, self.p1) != 0
            if p2_canmove:
                self.p1turn = False
        else:
            if pot & move:
                self.p2 = move_player(move, self.p2, self.p1)
                self.p1 = self.p1 & ~self.p2
            else:
                print(self)
                self.print_board(move)
                raise Exception('Illegal Move')
            p1_canmove = moves(self.p1, self.p2) != 0
            if p1_canmove:
                self.p1turn = True
                # p2_canmove = moves(self.p2, self.p1)!=0
        # print(self.heuristic(self.p1, self.p2))
        # sys.stdout.flush()
        return end_of_game(self.p1, self.p2)

    @staticmethod
    def heuristic(p1, p2):
        return np.random.rand()
        # return bin(p1).count("1")

    @staticmethod
    def to_array(board):
        pos = 0x8000000000000000
        out = np.zeros((64), dtype=np.bool)
        for i in range(64):
            out[i] = (pos & board) != 0
            pos >>= 1
        return out

    @staticmethod
    def listmoves(a):
        l = []
        up = 0x8000000000000000
        while up != 0:
            ar = up & a
            if ar != 0:
                l.append(ar)
            up >>= 1
        return l

    def best_move(self, maxdepth=None, add_noise=True):
        self.maxdepth = maxdepth or self.maxdepth

        if self.p1turn:
            self._alphabeta(self.p1, self.p2, self.maxdepth, -
                            float('inf'), float('inf'), True, add_noise)
        else:
            self._alphabeta(self.p2, self.p1, self.maxdepth, -
                            float('inf'), float('inf'), True, add_noise)
        return self.moved

    def _alphabeta(self, p1, p2, depth, alpha, beta, maxer, add_noise):
        eog = end_of_game(p1, p2)
        if eog == 3:
            return 0
        elif eog == 2:
            return -10000000000
        elif eog == 1:
            return 10000000000
        if depth == 0:
            return self.heuristic(p1, p2, add_noise)

        mv = moves(p1, p2)

        if mv == 0:
            return self._alphabeta(p2, p1, depth, alpha, beta, not maxer, add_noise)

        a = self.listmoves(mv)
        if maxer:
            v = -float('inf')
            for move in a:
                newp1 = move_player(move, p1, p2)
                newp2 = p2 & ~newp1
                # nd = depth-1 if len(a)>2 else depth
                nd = depth - 1
                y = self._alphabeta(newp2, newp1, nd, alpha,
                                    beta, False, add_noise)
                if y > v and depth == self.maxdepth:
                    self.moved = move
                v = max(v, y)
                alpha = max(alpha, v)
                if beta <= alpha:
                    break
            return v
        else:
            v = float('inf')
            for move in a:
                newp1 = move_player(move, p1, p2)
                newp2 = p2 & ~newp1
                # nd = depth-1 if len(a)>2 else depth
                nd = depth - 1
                y = self._alphabeta(newp2, newp1, nd, alpha,
                                    beta, True, add_noise)
                v = min(v, y)
                beta = min(beta, v)
                if beta <= alpha:
                    break
            return v


class Learner:
    def __init__(self, max_depth=4, hidden=100):
        self.max_depth = max_depth
        self.reward = None
        self.n_hidden = hidden
        self.wh = np.random.randn(64 * 2 + 2, self.n_hidden) / np.sqrt(128)
        self.wo = np.random.randn(self.n_hidden) / np.sqrt(self.n_hidden)
        self.opt_cache = [np.zeros_like(self.wh), np.zeros_like(self.wo)]

    def heuristic(self, p1, p2, add_noise=True):
        if self.wh.shape[0] == 130:
            x = np.concatenate(
                (Bitboard.to_array(p1), Bitboard.to_array(p2), [bc(moves(p1, p2))], [bc(moves(p2, p1))]))
        elif self.wh.shape[0] == 129:
            x = np.concatenate(
                (Bitboard.to_array(p1), Bitboard.to_array(p2), [bc(moves(p1, p2))]))
        else:
            x = np.concatenate(
                (Bitboard.to_array(p1), Bitboard.to_array(p2)))
        h = x @ self.wh
        if add_noise:
            h += 0.07 * np.random.randn(*h.shape)
        h = np.maximum(h, 0)
        pred = self.wo @ h
        return pred

    def update(self, data, rewards, lr=1e-4):
        data = data.reshape(len(data), -1)
        # forward
        h = data @ self.wh
        mask = h > 0
        h *= mask

        h += 0.07 * np.random.randn(*h.shape)
        preds = h @ self.wo

        # backward
        dwo = h.T @ (preds - rewards)/len(rewards)
        dwh = data.T @ (np.outer(rewards, self.wo) * mask)
        if np.abs(dwo).sum() > 1000000:
            self.save_weights('bad_weights')
            raise Exception()
        self.opt_cache[0] *= 0.9
        self.opt_cache[0] += 0.1 * dwh**2
        self.opt_cache[1] *= 0.9
        self.opt_cache[1] += 0.1 * dwo**2
        self.wh -= lr * dwh / np.sqrt(self.opt_cache[0] + 1e-8)
        self.wo -= lr * dwo / np.sqrt(self.opt_cache[1] + 1e-8)

    def save_weights(self, path):
        if not os.path.exists(path):
            os.mkdir(path)
        self.wh.tofile('%s/wh' % path)
        self.wo.tofile('%s/wo' % path)

    def load_weights(self, path):
        self.wh = np.fromfile('%s/wh' % path)
        if len(self.wh) % 128 == 0:
            self.wh.shape = (128, -1)
        elif len(self.wh) % 129 == 0:
            self.wh.shape = (129, -1)
        else:
            self.wh.shape = (130, -1)
        self.wo = np.fromfile('%s/wo' % path).reshape(self.wh.shape[1])

    def move(self, bb, add_noise=True):
        move = bb.best_move(self.max_depth, add_noise)
        eg = bb.make_move(move)

        if bb.p1turn:
            self.p1_moves.append((bb.p1, bb.p2))
        else:
            self.p2_moves.append((bb.p2, bb.p1))

        if eg == 3:
            self.reward = (0, 0)
        elif eg == 2:
            self.reward = (-1, 1)
        elif eg == 1:
            self.reward = (1, -1)

    def play_a_game(self):
        bb = Bitboard(self.max_depth, heuristic=self.heuristic)
        self.p1_moves = []
        self.p2_moves = []
        self.reward = None
        ct = 0
        while self.reward is None:
            ct += 1
            self.move(bb)
        return bb

    def play_and_update(self, lr=3e-4):
        bb = self.play_a_game()
        if self.reward == (0, 0):
            return bb
        data, rewards = self.prepare_numpy()
        self.update(data, rewards, lr)
        return bb

    def prepare_numpy(self):
        training_data = np.array([(Bitboard.to_array(p1).reshape(8, 8), Bitboard.to_array(p2).reshape(8, 8)) for p1, p2 in self.p1_moves] + [
                                 (Bitboard.to_array(p2).reshape(8, 8), Bitboard.to_array(p1).reshape(8, 8)) for p2, p1 in self.p2_moves])
        training_data = np.concatenate(
            [np.rot90(training_data, k, (-2, -1)) for k in range(4)], 0).reshape(-1, 8 * 8 * 2)

        mcs = np.array([(bc(moves(p1, p2)), bc(moves(p2, p1))) for p1, p2 in self.p1_moves] +
                       [(bc(moves(p2, p1)), bc(moves(p1, p2))) for p2, p1 in self.p2_moves])
        mcs = np.tile(mcs, (4, 1))

        training_data = np.append(training_data, mcs, 1)
        p1_rewards = self.reward[0] * \
            (0.99**np.arange(len(self.p1_moves)))[::-1]
        p2_rewards = self.reward[1] * \
            (0.99**np.arange(len(self.p2_moves)))[::-1]
        rewards = np.append(p1_rewards, p2_rewards)
        sd = rewards.std() 
        mean = rewards.mean()
        if sd < 1e-6:
            sd = 100000000
        rewards = np.tile(rewards, 4)
        return training_data, (rewards - mean) / sd


def tohex(val, nbits=64):
    return hex((val + (1 << nbits)) % (1 << nbits))


def get_bb(x, y):
    return (1 << (7 - x)) << ((7 - y) * 8)


def get_bb_from_list(coord_list):
    if type(coord_list[0]) == tuple:
        b = 0
        for x, y in coord_list:
            b |= get_bb(x, y)
        return b
    else:
        b = 0
        for d in coord_list:
            b |= get_bb(d['x'], d['y'])
        return b


def get_coord(board):
    return (board_x(board), board_y(board))


if __name__ == '__main__':
    # l = Learner()
    # l.load_weights('weights')
    # bb = Bitboard(4, heuristic=l.heuristic)

    a = Learner(2, 100)
    # a.load_weights('weights_reproduce_100')
    # a.play_and_update()
    print('weights2_reg_100')

    for i in range(1, 100000 + 1):
        start = time.time()
        bb = a.play_and_update()
        end = time.time()
        if i % 100 == 0:
            print(f'Iteration {i}: {end-start:.3f}s')
            # assert False
            # sys.stdout.flush()
            a.save_weights('weights2_reg_100')
    # a.save_weights('weights')
