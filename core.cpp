#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "classes.hpp"
#include "conductivity.hpp"
using namespace std;

double h, tau;

void main()
{
	boundary left, right;
	geometry onedimens;
	times time;

	vector<double> u;
	vector<double> u_;

	left.init("Left.txt");//Initialization functions
	right.init("Right.txt");
	onedimens.init("Geometry.txt");

	init_steps(onedimens, left, right);
	conductivity("initial_condition.txt" , "output.txt", onedimens, left, right, u, u_,time);
}