#include <cstdint>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <algorithm>
#include <ctime>

using namespace std;
using namespace Eigen;

#define UP_MASK 0xFF00000000000000
#define DOWN_MASK 0x00000000000000FF
#define LEFT_MASK 0x8080808080808080
#define RIGHT_MASK 0x0101010101010101

const uint64_t start = 0x8000000000000000;
const uint64_t m1 = 0x5555555555555555; //binary: 0101...
const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
// const uint64_t m8 = 0x00ff00ff00ff00ff;  //binary:  8 zeros,  8 ones ...
// const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
// const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
// const uint64_t hff = 0xffffffffffffffff; //binary: all ones
// const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
MatrixXf Wh(130,100);//it = MatrixXf::Random(130, 100); // (D,H)
VectorXf Wo(100); //= VectorXf::Random(100);      // (H,1)
uint64_t moved;
int maxDepth;
// void updateWeights(float learningRate, MatrixXf data, VectorXf rewards){

//     MatrixXf h = data * Wh;
//     MatrixXf mask = (h.array()>0);
//     h = h.cwiseProduct(mask);

//     VectorXf outs = h * Wo;

//     VectorXf errs = outs - rewards;

//     MatrixXf dh = (errs * Wo.transpose()).cwiseProduct(mask);
//     MatrixXf dWo = h.transpose() * errs;

//     MatrixXf dWh = data.transpose() * dh;

//     Wh -= learningRate * dWh;
//     Wo -= learningRate * dWo;
// }

uint64_t shift_up(uint64_t board)
{
    return (board & ~UP_MASK) << 8;
}

uint64_t shift_down(uint64_t board)
{
    return (board & ~DOWN_MASK) >> 8;
}

uint64_t shift_left(uint64_t board)
{
    return (board & ~LEFT_MASK) << 1;
}

uint64_t shift_right(uint64_t board)
{
    return (board & ~RIGHT_MASK) >> 1;
}

uint64_t shift_ur(uint64_t board)
{
    return (board & (~RIGHT_MASK) & (~UP_MASK)) << 7;
}

uint64_t shift_ul(uint64_t board)
{
    return (board & (~LEFT_MASK) & (~UP_MASK)) << 9;
}

uint64_t shift_dr(uint64_t board)
{
    return (board & (~RIGHT_MASK) & (~DOWN_MASK)) >> 9;
}

uint64_t shift_dl(uint64_t board)
{
    return (board & (~LEFT_MASK) & (~DOWN_MASK)) >> 7;
}

uint64_t up_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_up(p1) & p2)) != 0)
    {
        res |= (shift_up(p1) & empty);
    }
    return res;
}

uint64_t down_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_down(p1) & p2)) != 0)
    {
        res |= (shift_down(p1) & empty);
    }
    return res;
}

uint64_t left_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_left(p1) & p2)) != 0)
    {
        res |= (shift_left(p1) & empty);
    }
    return res;
}

uint64_t right_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_right(p1) & p2)) != 0)
    {
        res |= (shift_right(p1) & empty);
    }
    return res;
}

uint64_t ul_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_ul(p1) & p2)) != 0)
    {
        res |= (shift_ul(p1) & empty);
    }
    return res;
}

uint64_t ur_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_ur(p1) & p2)) != 0)
    {
        res |= (shift_ur(p1) & empty);
    }
    return res;
}

uint64_t dl_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_dl(p1) & p2)) != 0)
    {
        res |= (shift_dl(p1) & empty);
    }
    return res;
}

uint64_t dr_moves(uint64_t p1, uint64_t p2)
{
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    while ((p1 = (shift_dr(p1) & p2)) != 0)
    {
        res |= (shift_dr(p1) & empty);
    }
    return res;
}

extern "C" uint64_t moves(uint64_t p1, uint64_t p2)
{
    // could just call thos functions above but I have this so i'll keep it
    uint64_t res = 0;
    uint64_t empty = ~p1 & ~p2;
    uint64_t tmp = p1;
    while ((tmp = (shift_up(tmp) & p2)) != 0)
        res |= (shift_up(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_down(tmp) & p2)) != 0)
        res |= (shift_down(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_left(tmp) & p2)) != 0)
        res |= (shift_left(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_right(tmp) & p2)) != 0)
        res |= (shift_right(tmp) & empty);
    tmp = p1;

    while ((tmp = (shift_ul(tmp) & p2)) != 0)
        res |= (shift_ul(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_ur(tmp) & p2)) != 0)
        res |= (shift_ur(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_dl(tmp) & p2)) != 0)
        res |= (shift_dl(tmp) & empty);
    tmp = p1;
    while ((tmp = (shift_dr(tmp) & p2)) != 0)
        res |= (shift_dr(tmp) & empty);
    return res;
}

extern "C" int board_x(uint64_t move)
{
    if (move == 0)
    {
        return -1;
    }
    uint64_t mask = LEFT_MASK;
    int i = 0;
    while ((move & mask) == 0)
    {
        i++;
        mask >>= 1;
    }
    return i;
}

extern "C" int board_y(uint64_t move)
{
    if (move == 0)
    {
        return -1;
    }
    uint64_t mask = UP_MASK;
    int i = 0;
    while ((move & mask) == 0)
    {
        i++;
        mask >>= 8;
    }
    return i;
}

extern "C" uint64_t move_player(uint64_t move, uint64_t p1, uint64_t p2)
{
    uint64_t full = 0;
    uint64_t empty = ~(p1 | move | p2);
    int upbound = board_y(move);
    int leftbound = board_x(move);
    int rightbound = 8 - leftbound;
    int downbound = 8 - upbound;

    int k = 0;
    uint64_t moveShift = move;
    bool found = false;
    // move down
    while (k < downbound && !found)
    {
        moveShift |= shift_down(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }

    //move up
    moveShift = move;
    k = 0;
    found = false;
    while (k < upbound && !found)
    {
        moveShift |= shift_up(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }

    //move left
    moveShift = move;
    k = 0;
    found = false;
    while (k < leftbound && !found)
    {
        moveShift |= shift_left(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    //move right
    moveShift = move;
    k = 0;
    found = false;
    while (k < rightbound && !found)
    {
        moveShift |= shift_right(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    //move up-left
    moveShift = move;
    k = 0;
    found = false;
    while (k < (leftbound > upbound ? leftbound : upbound) && !found)
    {
        moveShift |= shift_ul(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    //move up-right
    moveShift = move;
    k = 0;
    found = false;
    while (k < (rightbound > upbound ? rightbound : upbound) && !found)
    {
        moveShift |= shift_ur(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    //move down-left
    moveShift = move;
    k = 0;
    found = false;
    while (k < (leftbound > downbound ? leftbound : downbound) && !found)
    {
        moveShift |= shift_dl(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    //move down-right
    moveShift = move;
    k = 0;
    found = false;
    while (k < (rightbound > upbound ? rightbound : upbound) && !found)
    {
        moveShift |= shift_dr(moveShift);
        if ((moveShift & empty) != 0)
        {
            found = true;
        }
        else if ((moveShift & p1) != 0)
        {
            if (k == 0)
            {
                found = true;
            }
            else
            {
                found = true;
                full |= moveShift;
            }
        }
        k++;
    }
    return ((full & p2) | p1 | move);
}

extern "C" int bitCount(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    x += x >> 8;                    //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;                   //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;                   //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
}

extern "C" int endofGame(uint64_t p1, uint64_t p2)
{
    uint64_t p1moves = moves(p1, p2);
    uint64_t p2moves = moves(p2, p1);

    if ((p1moves == 0) && (p2moves == 0))
    {
        unsigned int p1ct = bitCount(p1);
        unsigned int p2ct = bitCount(p2);

        if (p1ct > p2ct)
        {
            return 1;
        }
        else if (p2ct > p1ct)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        return 0;
    }
}

vector<uint64_t> toList(uint64_t board)
{
    vector<uint64_t> out;
    uint64_t pos = start;
    while (pos > 0)
    {
        uint64_t tmp = pos & board;
        if (tmp != 0)
        {
            out.push_back(tmp);
        }
        pos >>=1;
    }
    return out;
}

VectorXi toVector(uint64_t board)
{
    VectorXi out(64);
    uint64_t pos = 0x8000000000000000;
    for (int i = 0; i < 64; i++)
    {
        out(i) = (pos & board) != 0;
        pos >>= 1;
    }
    return out;
}
VectorXi doubleToVector(uint64_t p1, uint64_t p2)
{
    VectorXi out(130);
    uint64_t pos = start;
    for (int i = 0; i < 64; i++)
    {
        out(i) = (pos & p1) != 0;
        out(i + 64) = (pos & p2) != 0;
        pos >>= 1;
    }
    out(128) = bitCount(moves(p1, p2));
    out(129) = bitCount(moves(p2, p1));
    return out;
}

float heuristic(uint64_t p1, uint64_t p2, bool add_noise)
{
    VectorXf input = doubleToVector(p1, p2).cast<float>();
    VectorXf h0 = Wh.transpose() * input;
    VectorXf h = (h0.array() * (h0.array() > 0).cast<float>()).matrix();
    if (add_noise){}
    return h.dot(Wo);
}

void setWs(float *hPointer, float *oPointer)
{
    Wh = Map<Matrix<float, 100, 130> >(hPointer).transpose();
    Wo = Map<Matrix<float, 100, 1>  >(hPointer);
}

double alphaBeta(uint64_t p1, uint64_t p2, int depth, double alpha, double beta, bool maxer, bool add_noise)
{
    int eog = endofGame(p1, p2);
    if (eog == 3)
    {
        return 0.0;
    }
    else if (eog == 2)
    {
        return -10000000000.0;
    }
    else if (eog == 1)
    {
        return 10000000000.0;
    }
    if (depth == 0)
    {
        return heuristic(p1, p2, add_noise);
    }
    uint64_t mv = moves(p1, p2);
    // cout << mv;
    if (mv == 0)
    {
        return alphaBeta(p2, p1, depth, alpha, beta, !maxer, add_noise);
    }
    vector<uint64_t> a = toList(mv);
    if (maxer)
    {
        double v = -INFINITY;
        for (int i = 0; i < a.size(); i++)
        {
            uint64_t move = a[i];
            uint64_t newp1 = move_player(move, p1, p2);
            uint64_t newp2 = p2 & ~p1;
            double y = alphaBeta(newp2, newp1, depth - 1, alpha, beta, false, add_noise);
            if (y > v && depth == maxDepth)
            {
                moved = move;
            }
            v = max(y, v);
            alpha = max(alpha, v);
            if (beta <= alpha)
            {
                break;
            }
        }
        return v;
    }
    else
    {
        double v = INFINITY;
        for (int i = 0; i < a.size(); i++)
        {
            uint64_t move = a[i];
            uint64_t newp1 = move_player(move, p1, p2);
            uint64_t newp2 = p2 & ~p1;
            double y = alphaBeta(newp2, newp1, depth - 1, alpha, beta, true, add_noise);
            v = min(y, v);
            beta = min(beta, v);
            if (beta <= alpha)
            {
                break;
            }
        }
        return v;
    }
}

uint64_t bestMove(uint64_t p1, uint64_t p2, int max_depth, bool add_noise)
{
    moved = 0;
    maxDepth = max_depth;
    alphaBeta(p1, p2, maxDepth, -INFINITY, INFINITY, true, add_noise);
    return moved;
}

extern "C" uint64_t doEverything(uint64_t p1, uint64_t p2, int max_depth, bool add_noise, float *hPointer, float *oPointer){
    setWs(hPointer, oPointer);
    return bestMove(p1, p2, max_depth, add_noise);
}

int main(){
    uint64_t p1 = 0x0000000810000000;
    uint64_t p2 = 0x0000001008000000;
    clock_t begin = clock();    
    cout << bestMove(p1, p2, 8, false);
    clock_t end = clock();
    cout<<"\n";
    cout<<double(end - begin) / CLOCKS_PER_SEC;    
    return 0;
}