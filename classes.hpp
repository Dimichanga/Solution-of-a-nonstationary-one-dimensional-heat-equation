#pragma once 
using namespace std;

extern double h, tau;

class boundary//Class of boundary conditions
{
public:
	vector <double> time_table;
	vector <double> temp_table;
	void init(string file)
	{
		ifstream finbl(file);
		if (!finbl.is_open())
			cout << "File is not open.";
		else
		{
			double buftime, buftemp;
			while (finbl >> buftime && finbl >> buftemp)
			{
				time_table.push_back(buftime);
				temp_table.push_back(buftemp);
			}
			finbl.close();
		}
	}
	double get(double x);
};

struct geometry//Geometric dimension class
{
	double a;
	double b;
	void init(string file)
	{
		ifstream fingeom(file);
		fingeom >> a;
		fingeom >> b;
		fingeom.close();
	}
};

class times
{
public:
	double step, actual, duration, output;
	int n;
};

void init_steps(geometry onedimens, boundary& left, boundary& right)
{
	int numb, n, N;

	short flag = 1;
	while (flag)
	{
		cout << "Enter the number of steps in the space(by default enter 10): " << '\n';
		cin >> numb;
		cout << "Enter time step for the calculation (by default enter 0.5): " << '\n';
		cin >> tau;

		h = (onedimens.b - onedimens.a) / numb;//Step through the space
		if (tau <= 0.5 * h * h) flag = 0;
		else cout << "Error! Stability condition is not met" << "\n\n";
	}

	N = left.time_table.size();
	n = (left.time_table[N - 1] - left.time_table[0]) / tau;//Number of time steps
	cout << "n = " << n << '\n';
}

double boundary::get(double x)
{
	double val;
	int N = time_table.size();
	if (x >= time_table[N - 1])
		val = temp_table[N - 1];
	else
	{
		int i = 0;
		while (time_table[i] < x) i++;
		val = temp_table[i - 1] + ((temp_table[i] - temp_table[i - 1]) / (time_table[i] - time_table[i - 1])) * (x - time_table[i - 1]);
	}
	return val;

}