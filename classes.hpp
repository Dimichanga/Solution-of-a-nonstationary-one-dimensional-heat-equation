#pragma once 
using namespace std;

class geometry;
class steps;

class boundary//Class of boundary conditions
{
public:
	vector <double> time_table;
	vector <double> temp_table;
	vector <double> time;
	vector <double> temp;
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
	friend void init_steps(geometry onedimens, boundary& left, boundary& right, steps& step);
	friend void interpolation(boundary& left, boundary& right, steps step);
};
	

class geometry//Geometric dimension class
{
public:
	double a;
	double b;
	void init(string file)
	{
		ifstream fingeom(file);
		fingeom >> a;
		fingeom >> b;
		fingeom.close();
	}
	friend void init_steps(geometry onedimens, boundary& left, boundary& right, steps& step);
	friend void interpolation(boundary& left, boundary& right, steps step);
};

class steps
{
public:
	double h, tau;
	int n, N;
	friend void init_steps(geometry onedimens, boundary& left, boundary& right, steps& step);
	friend void interpolation(boundary& left, boundary& right, steps step);
};

void init_steps(geometry onedimens, boundary &left, boundary &right, steps &step)
{
	int numb;
	cout << "Enter the number of steps in the space(by default enter 10): "<<'\n';
	cin >> numb;

	step.h = (onedimens.b - onedimens.a) / numb;//Step through the space
	step.tau = 0.5 * step.h * step.h;//Time step from the stability condition
	step.N = left.time_table.size();
	step.n = (left.time_table[step.N - 1] - left.time_table[0]) / step.tau;//Number of time steps
	cout << "tau = " << step.tau << '\n';
	cout << "n = " << step.n << '\n';

	left.time.resize(step.n + 1);//Setting the size of the time vector by the number of time steps
	left.temp.resize(step.n + 1);

	right.time.resize(step.n + 1);//Setting the size of the time vector by the number of time steps
	right.temp.resize(step.n + 1);
}

void interpolation(boundary& left, boundary& right, steps step)
{
	int i, j;
	for (i = 0; i < left.temp_table.size() - 1; i++)
	{
		int n = (left.time_table[i + 1] - left.time_table[i]) / step.tau;
		for (j = 0; j < n; j++)
		{
			left.time[j + n * i] = j + n * i;
			left.temp[j + n * i] = left.temp_table[i] + step.tau * j;//Interpolation formula (linear)
		}
	}
	left.time[step.n] = 2 * left.time_table[step.N - 1];
	left.temp[step.n] = left.temp_table[step.N - 1];

	for (i = 0; i < right.temp_table.size() - 1; i++)
	{
		int n = (right.time_table[i + 1] - right.time_table[i]) / step.tau;
		for (j = 0; j < n; j++)
		{
			right.time[j + n * i] = j + n * i;
			right.temp[j + n * i] = right.temp_table[i] + step.tau * j;//Interpolation formula (linear)
		}
	}
	right.time[step.n] = 2 * right.time_table[step.N - 1];
	right.temp[step.n] = right.temp_table[step.N - 1];
}