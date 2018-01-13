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
#include <Eigen/Dense>
#include "SimplexSolver.h"
#include "exception.h"

using namespace Eigen;

/**
 * Constructor
 * 
 * @param int mode This can be one of these: SIMPLEX_MINIMIZE, SIMPLEX_MAXIMIZE
 * @param const VectorXd &objectiveFunction The coefficients of the objective function.
 * @param const MatrixXd &constraints Full matrix for the constraints. Contains also the righthand-side values.
 * @returns SimplexSolver
 */
SimplexSolver::SimplexSolver(int mode, const VectorXd &objectiveFunction, const MatrixXd &constraints) {
    int64_t constantColumn, temp;
    this->foundSolution = false;
    this->optimum = 0;
    this->numberOfVariables = objectiveFunction.rows();
    
    /*
        Validate input parameters
    */
    if (mode != SIMPLEX_MINIMIZE && mode != SIMPLEX_MAXIMIZE) {
        throw FException("SimplexSolver: invalid value for the 'mode' parameter.");
    }

    if (objectiveFunction.rows() < 1) {
        throw FException("SimplexSolver: The coefficient vector of the objective function must contain at least one row.");
    }

    if (constraints.rows() < 1) {
        throw FException("SimplexSolver: The constraint matrix must contain at least one row.");
    }
    
    if (constraints.cols() != objectiveFunction.rows() + 1) {
        throw FException("SimplexSolver: The constraint matrix has %d columns, but should have %d, because the coefficient vector of the objective function has %d rows.",
            constraints.cols(), objectiveFunction.rows() + 1, objectiveFunction.rows());
    }
    
    for (int i = 0; i < this->numberOfVariables; i++) {
        if (objectiveFunction(i) == 0) {
            throw FException("SimplexSolver: One of the coefficients of the objective function is zero.");
        }
    }

    temp = constraints.cols() - 1;
    for (int i = 0; i < constraints.rows(); i++) {
        if (constraints(i, temp) < 0) {
            throw FException("SimplexSolver: All righthand-side coefficients of the constraint matrix must be non-negative.");
        }
    }
    
    /*
        Build tableau
    */
    if (mode == SIMPLEX_MAXIMIZE) {
        // Maximize
        this->tableau.resize(constraints.rows() + 1, this->numberOfVariables + constraints.rows() + 1);

        this->tableau <<    -objectiveFunction.transpose(),                   MatrixXd::Zero(1, constraints.rows() + 1),
                            constraints.leftCols(this->numberOfVariables),    MatrixXd::Identity(constraints.rows(), constraints.rows()), constraints.rightCols(1);
    } else {
        // Minimize: construct the Dual problem
        this->tableau.resize(this->numberOfVariables + 1, this->numberOfVariables + constraints.rows() + 1);

        this->tableau <<    -constraints.rightCols(1).transpose(),                        MatrixXd::Zero(1, this->numberOfVariables + 1),
                            constraints.leftCols(this->numberOfVariables).transpose(),    MatrixXd::Identity(this->numberOfVariables, this->numberOfVariables), objectiveFunction;
    }
    
    /*
        Simplex algorithm
    */
    if (mode == SIMPLEX_MAXIMIZE) {
        // Maximize original problem
        if (!this->simplexAlgorithm(this->numberOfVariables)) {
            return;    // No solution
        }
    } else {
        // Maximize the dual of the minimization problem
        if (!this->simplexAlgorithm(constraints.rows())) {
            return;    // No solution
        }
    }
    
    /*
        Fetch solution
    */
    constantColumn = this->tableau.cols() - 1;
    this->solution.resize(this->numberOfVariables);

    if (mode == SIMPLEX_MAXIMIZE) {
        // Maximize
        for (int i = 0; i < this->numberOfVariables; i++) {
            temp = this->getPivotRow(i);
            
            if (temp > 0) {
                // Basic variable
                this->solution(i) = this->tableau(temp, constantColumn);
            } else {
                // Non-basic variable
                this->solution(i) = 0;
            }
        }
        this->foundSolution = true;
        this->optimum = this->tableau(0, constantColumn);
    } else {
        // Minimize
        for (int i = 0; i < this->numberOfVariables; i++) {
            this->solution(i) = this->tableau(0, constraints.rows() + i);
        }
        this->foundSolution = true;
        this->optimum = this->tableau(0, constantColumn);
    }
}

/**
 * Returns true if a solution has been found.
 * Return false otherwise.
 *
 * @returns boolean
 */
bool SimplexSolver::hasSolution() {
    return this->foundSolution;
}

/**
 * Returns the maximum/minimum value of
 * the objective function.
 * 
 * @returns double
 */
double SimplexSolver::getOptimum() {
    return this->optimum;
}

/**
 * Returns a vector with the variable
 * values for the solution.
 *
 * return VectorXd
 */
VectorXd SimplexSolver::getSolution() {
    return this->solution;
}

/**
 * Searches for the pivot row in the given column, by calculating the ratios.
 * Tries to find smallest non-negative ratio.
 * Returns -1 if all possible pivots are 0 or if the ratios are negative.
 * Deals with cases like this:  0/negative < 0/positive
 * 
 * @param int64_t column
 * @returns int64_t Returns the number of the pivot row, or -1 if found none.
 */
int64_t SimplexSolver::findPivot_min(int64_t column) {
    int64_t minIndex = -1;
    int64_t constantColumn = this->tableau.cols() - 1;
    double minRatio = 0;
    double minConstant = 0;    // For the "0/negative < 0/positive" difference the constants have to be tracked also.
    double ratio;
    int64_t rowNum = this->tableau.rows();
    
    for (int i = 1; i < rowNum; i++) {
        if (this->tableau(i, column) == 0) {
            continue;
        }

        ratio = this->tableau(i, constantColumn) / this->tableau(i, column);
        if (ratio < 0) {
            //The ratio must be non-negative
            continue;
        }

        if (minIndex == -1) {
            // First pivot candidate
            minIndex = i;
            minRatio = ratio;
            minConstant = this->tableau(i, constantColumn);
        } else {
            if (ratio == 0 && ratio == minRatio) {
                // 0/negative < 0/positive
                if (this->tableau(i, constantColumn) < minConstant) {
                    minIndex = i;
                    minRatio = ratio;
                    minConstant = this->tableau(i, constantColumn);
                }
            } else if (ratio < minRatio) {
                minIndex = i;
                minRatio = ratio;
                minConstant = this->tableau(i, constantColumn);
            }
        }
    }

    return minIndex;
}

/**
 * Iterates through the this->tableau matrix to solve the problem.
 * 
 * @param int64_t variableNum The number of variables (dimensions). (different for the minimization problem)
 * @returns bool Returns true if a solution has been found. Returns false otherwise.
 */
bool SimplexSolver::simplexAlgorithm(int64_t variableNum) {
    MatrixXd::Index pivotColumn;
    int64_t pivotRow;
    
    while (true) {
        /*
            Find pivot column, check for halt condition
        */
        this->tableau.row(0).leftCols(variableNum).minCoeff(&pivotColumn);
        if (this->tableau(0, pivotColumn) >= 0) {
            //Found no negative coefficient
            break;
        }
        
        /*
            Find pivot row
        */
        pivotRow = this->findPivot_min(pivotColumn);
        if (pivotRow == -1) {
            //no solution
            return false;
        }
        
        /*
            Do pivot operation
        */
        this->tableau.row(pivotRow) /= this->tableau(pivotRow, pivotColumn);
        this->tableau(pivotRow, pivotColumn) = 1;    // For possible precision issues
        for (int i = 0; i < this->tableau.rows(); i++) {
            if (i == pivotRow) continue;
            
            this->tableau.row(i) -= this->tableau.row(pivotRow) * this->tableau(i, pivotColumn);
            this->tableau(i, pivotColumn) = 0;    // For possible precision issues
        }
    }

    return true;
}

/**
 * If the given column has only one coefficient with value 1 (except in topmost row), and all other
 * coefficients are zero, then returns the row of the non-zero value.
 * Otherwise return -1.
 * This method is used in the final step of maximization, when we read
 * the solution from the tableau.
 * 
 * @param int64_t column
 * @returns int64_t
 */
int64_t SimplexSolver::getPivotRow(int64_t column) {
    int64_t one_row = -1;

    for (int i = 1; i < this->tableau.rows(); i++) {
        if (this->tableau(i, column) == 1) {
            if (one_row >= 0) {
                return -1;
            } else {
                one_row = i;
                continue;
            }
        } else if (this->tableau(i, column) != 0) {
            return -1;
        }
    }

    return one_row;
}
