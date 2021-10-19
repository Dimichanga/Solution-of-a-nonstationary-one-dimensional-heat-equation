#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

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

};

void init(geometry onedimens, boundary& left, boundary& right, double& tau, double& h, double& initcond, double& coef, int& n, int& N);
void conductivity(string file, geometry onedimens, boundary left, boundary right, double tau, double h, double initcond, double coef, vector<double>& u, vector<double>& u_, int n);
void interpolation(geometry onedimens, boundary& left, boundary& right, double h, double tau, int n, int N);
void conductivity(geometry onedimens, boundary left, boundary right, double tau, double h, double initcond, double coef, vector<double>& u, vector<double>& u_, int n);

int main()
{

	boundary left, right;
	geometry onedimens;
	double h, tau;
	double initcond, coef;
	int n, N;

	vector<double> u;
	vector<double> u_;

	left.init("Left.txt");//Initialization functions
	right.init("Right.txt");
	onedimens.init("Geometry.txt");

	init(onedimens, left, right, tau,h,initcond,coef, n,N);
	interpolation(onedimens, left, right, h,tau,n,N);
	conductivity("output.txt", onedimens, left, right, tau, h, initcond, coef, u, u_, n);

	return 0;
}

void init(geometry onedimens, boundary& left, boundary& right, double& tau, double& h, double& initcond, double& coef, int& n, int& N)
{
	ifstream finin("initial_condition.txt");
	finin >> initcond;
	finin >> coef;
	finin.close();

	h = (onedimens.b - onedimens.a) / 10;//Step through the space
	tau = 0.5 * h * h;//Time step from the stability condition
	N = left.time_table.size();
	n = (left.time_table[N - 1] - left.time_table[0]) / tau;//Number of time steps

	left.time.resize(n + 1);//Setting the size of the time vector by the number of time steps
	left.temp.resize(n + 1);

	right.time.resize(n + 1);//Setting the size of the time vector by the number of time steps
	right.temp.resize(n + 1);

}


void interpolation(geometry onedimens, boundary& left, boundary& right, double h, double tau, int n, int N)
{

	int i, j;
	double k;
	for (i = 0; i < left.temp_table.size() - 1; i++)
	{
		int n = (left.time_table[i + 1] - left.time_table[i]) / tau;
		k = (left.temp_table[i + 1] - left.temp_table[i]) / n;
		for (j = 0; j < n; j++)
		{
			left.time[j + n * i] = j + n * i;
			left.temp[j + n * i] = left.temp_table[i] + k * j;//‘ормула интрепол€ции (линейной)
		}
	}
	left.time[n] = 2 * left.time_table[N - 1];
	left.temp[n] = left.temp_table[N - 1];

	for (i = 0; i < right.temp_table.size() - 1; i++)
	{
		int n = (right.time_table[i + 1] - right.time_table[i]) / tau;
		k = (right.temp_table[i + 1] - right.temp_table[i]) / n;
		for (j = 0; j < n; j++)
		{
			right.time[j + n * i] = j + n * i;
			right.temp[j + n * i] = right.temp_table[i] + k * j;//‘ормула интрепол€ции (линейной)
		}
	}
	right.time[n] = 2 * right.time_table[N - 1];
	right.temp[n] = right.temp_table[N - 1];
}

void conductivity(string file, geometry onedimens, boundary left, boundary right, double tau, double h, double initcond, double coef, vector<double>& u, vector<double>& u_, int n)
{
	double l = onedimens.b - onedimens.a;

	ofstream fout(file);
	fout << "Left boundary: " << '\n';
	for (int i = 0; i < left.time_table.size(); i++)
	{
		fout << left.time_table[i] << "\t\t";
		fout << left.temp_table[i] << "\n";
	}
	fout << "\n\n";

	fout << "Right boundary: " << '\n';
	for (int i = 0; i < right.time_table.size(); i++)
	{
		fout << right.time_table[i] << "\t\t";
		fout << right.temp_table[i] << "\n";
	}
	fout << "\n\n";

	fout << "Initial condition: " << '\n';
	fout << initcond << "\n\n";
	fout << "Coefficient of thermal conductivity: " << '\n';
	fout << coef << "\n\n";

	fout << "Geometry: " << '\n';
	fout << onedimens.a << "\t\t" << onedimens.b << "\n";
	fout << "\n\n";

	fout << "Interpolation left boundary conditions: ";
	fout << left.time.size() << '\n';
	for (int i = 0; i < left.time.size(); i++)
		fout << "time: " << left.time[i] / 2 << "\t\t" << "temp: " << left.temp[i] << '\n';
	fout << "\n\n";

	fout << "Interpolation right boundary conditions: ";
	fout << right.time.size() << '\n';
	for (int i = 0; i < right.time.size(); i++)
		fout << "time: " << right.time[i] / 2 << "\t\t" << "temp: " << right.temp[i] << '\n';
	fout << "\n\n";


	double gamma = tau * coef / (h * h);
	int i, j;

	u.resize(l + 1);
	u_.resize(l + 1);

	u[0] = left.temp[0];
	u[l] = right.temp[0];
	for (i = 1; i < l; i++)
		u[i] = initcond;

	fout << "Solution of the equation: " << '\n';

	for (i = 1; i <=n; i++)
	{
		fout << "Time: " << left.time[i-1] / 2 <<"\t\t" << "Temp: ";
		for (int j = 0; j <= l; j++)
		{
			fout << u[j] << "\t\t";
			u_[j] = u[j];
		}
		fout << '\n';
		u[0] = left.temp[i];
		u[l] = right.temp[i];
		for (int j = 1; j < l; j++)
			u[j] = gamma * u_[j - 1] + (1 - 2 * gamma) * u_[j] + gamma * u_[j + 1]; //F[i][j] = f(x,t)=0
	}

	fout << "Time: " << left.time[n] / 2 << "\t\t" << "Temp: ";
	for (int j = 0; j <= l; j++)
		fout << u[j] << "\t\t";
	fout.close();
}