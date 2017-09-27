#include <Eigen/Dense>
#include <ctime>
#include <iostream>
using namespace std;
using namespace Eigen;

int main(){
    
    MatrixXf a = MatrixXf::Random(1000, 1000);
    MatrixXf b = MatrixXf::Random(1000, 1000);
    clock_t begin = clock();        
    MatrixXf c = a*b;
    clock_t end = clock();    
    cout << double(end - begin) / CLOCKS_PER_SEC;
    return 0;
}