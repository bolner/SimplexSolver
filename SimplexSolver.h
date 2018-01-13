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

#pragma once

#include <cinttypes>
#include <Eigen/Dense>

using namespace Eigen;

#define SIMPLEX_MINIMIZE 1
#define SIMPLEX_MAXIMIZE 2

class SimplexSolver {
private:
    MatrixXd tableau;
    bool foundSolution;
    double optimum;
    VectorXd solution;
    int64_t numberOfVariables;

    int64_t findPivot_min(int64_t column);
    bool simplexAlgorithm(int64_t variableNum);
    int64_t getPivotRow(int64_t column);

protected:

public:
    SimplexSolver(int mode, const VectorXd &objectiveFunction, const MatrixXd &constraints);
    bool hasSolution();
    double getOptimum();
    VectorXd getSolution();
};
