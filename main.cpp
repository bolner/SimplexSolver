#include <iostream>
#include <Eigen/Dense>
#include "SimplexSolver.h"
#include "exception.h"

using namespace std;
using namespace Eigen;

int main()
{
	SimplexSolver *solver1 = NULL, *solver2 = NULL;
	MatrixXd constraints(3, 3);
	VectorXd objectiveFunction(2);
	
	try {
		/*
			Maximization problem
		*/
		objectiveFunction <<	1,
								2;

		constraints <<		2,	3,	34,
							1,	5,	45,
							1,	0,	15;

		
		solver1 = new SimplexSolver(SIMPLEX_MAXIMIZE, objectiveFunction, constraints);

		if (solver1->hasSolution()) {
			cout << "The maximum is: " << solver1->getOptimum() << endl;
			cout << "The solution is: " << solver1->getSolution().transpose() << endl;
		} else {
			cout << "The linear problem has no solution." << endl;
		}

		cout << endl;
		
		/*
			Minimization problem
		*/
		objectiveFunction <<	3,
								4;

		constraints <<		2,	1,	8,
							1,	2,	13,
							1,	5,	16;
		
		solver2 = new SimplexSolver(SIMPLEX_MINIMIZE, objectiveFunction, constraints);

		if (solver2->hasSolution()) {
			cout << "The minimum is: " << solver2->getOptimum() << endl;
			cout << "The solution is: " << solver2->getSolution().transpose() << endl;
		} else {
			cout << "The linear problem has no solution." << endl;
		}
	}
	catch (FException *ex) {
		ex->Print();
	}
	
	delete solver1;
	delete solver2;
	
	return 0;
}

