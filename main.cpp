/*
Copyright 2012-2018 Tamas Bolner

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <cinttypes>
#include <iostream>
#include <Eigen/Dense>
#include "SimplexSolver.h"
#include "exception.h"

using namespace std;
using namespace Eigen;

int main()
{
    MatrixXd constraints(3, 3);
    VectorXd objectiveFunction(2);
    
    try {
        /*
            Maximization problem
        */
        objectiveFunction <<    1,
                                2;

        constraints <<      2,    3,    34,
                            1,    5,    45,
                            1,    0,    15;

        
        SimplexSolver solver1(SIMPLEX_MAXIMIZE, objectiveFunction, constraints);

        if (solver1.hasSolution()) {
            cout << "The maximum is: " << solver1.getOptimum() << endl;
            cout << "The solution is: " << solver1.getSolution().transpose() << endl;
        } else {
            cout << "The linear problem has no solution." << endl;
        }

        cout << endl;
        
        /*
            Minimization problem
        */
        objectiveFunction <<    3,
                                4;

        constraints <<      2,    1,    8,
                            1,    2,    13,
                            1,    5,    16;
        
        SimplexSolver solver2(SIMPLEX_MINIMIZE, objectiveFunction, constraints);

        if (solver2.hasSolution()) {
            cout << "The minimum is: " << solver2.getOptimum() << endl;
            cout << "The solution is: " << solver2.getSolution().transpose() << endl;
        } else {
            cout << "The linear problem has no solution." << endl;
        }
    } catch (const FException &ex) {
        ex.Print();
        return 1;
    }
    
    return 0;
}
