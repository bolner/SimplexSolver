# SimplexSolver

A simple implementation of the simplex algorithm, used for solving linear programming problems. One known issue of this implementation is that it doesn't give a proper solution to problems where the origin is outside of the feasable region.

For more information visit: <http://blog.bolner.hu/2012/08/22/simplex/>

## Compiling on Debian / Ubuntu

1. Install g++ and the Eigen library

        apt-get install g++ libeigen3-dev libeigen3-doc

2. Change to the project library and compile

        cd /path/to/the/project/library
        g++ -std=c++11 -I /usr/include/eigen3/ *.cpp -o simplex.exe
