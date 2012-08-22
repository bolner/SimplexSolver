/*
    Simple Simplex Solver Class
    Copyright (C) 2012  Tamas Bolner
	For more information, visit: http://blog.bolner.hu/2012/08/22/simplex/
	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
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
	__int64 numberOfVariables;

	__int64 findPivot_min(__int64 column);
	bool simplexAlgorithm(__int64 variableNum);
	__int64 getPivotRow(__int64 column);

protected:

public:
	SimplexSolver(int mode, const VectorXd &objectiveFunction, const MatrixXd &constraints);
	bool hasSolution();
	double getOptimum();
	VectorXd getSolution();
};
