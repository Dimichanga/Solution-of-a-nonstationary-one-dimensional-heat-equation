#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class boundary//Класс граничных условий
{
public:
	vector <double> time_table;
	vector <double> temp_table;
	vector <double> time;
	vector <double> temp;
	void init(string file);
};

void boundary::init(string file)//Элементы таблицы объекта класса заполняются
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

class geometry//Класс геометрических размеров
{
public:
	double a;
	double b;
	void init(string file);
};

void geometry::init(string file)//Инициализация геометрических данных из файла
{
	ifstream fingeom(file);
	fingeom >> a;
	fingeom >> b;
	fingeom.close();
}

void init(geometry onedimens, boundary& left, boundary& right, double &tau, double &h, double& initcond, double &coef, int &n, int &N);
void output(string file, boundary left, boundary right, geometry onedimens, double initcond, double coef, vector <vector<double>>& u, int n);
void interpolation(geometry onedimens, boundary& left, boundary& right, double h, double tau, int n, int N);
void conductivity(geometry onedimens, boundary left, boundary right, double tau, double h, double initcond, double coef, vector <vector<double>> &u, int n);

int main()
{
	boundary left, right;
	geometry onedimens;
	double h, tau;
	double initcond, coef;
	int n, N;

	vector <vector<double>> u;

	left.init("Left.txt");//Функции инициализации
	right.init("Right.txt");
	onedimens.init("Geometry.txt");

	init(onedimens, left, right, tau,h,initcond,coef, n,N);
	interpolation(onedimens, left, right, h,tau,n,N);
	conductivity(onedimens, left, right, tau, h, initcond, coef, u, n);
	output("output.txt", left, right, onedimens,initcond, coef, u, n);

	return 0;
}

void init(geometry onedimens, boundary& left, boundary& right, double &tau, double &h, double &initcond,double &coef, int &n, int &N)
{
	ifstream finin("initial_condition.txt");
	finin >> initcond;
	finin >> coef;
	finin.close();

	h = (onedimens.b - onedimens.a) / 10;//Шаг по пространству
	tau = 0.5 * h * h;//Шаг по времени из условия устойчивости
	N = left.time_table.size();
	n = (left.time_table[N - 1] - left.time_table[0]) / tau;//Количество шагов по времени

	left.time.resize(n + 1);//Задаём размер вектора времени по количеству шагов по времени
	left.temp.resize(n + 1);

	right.time.resize(n + 1);//Задаём размер вектора времени по количеству шагов по времени
	right.temp.resize(n + 1);

}

void output(string file, boundary left, boundary right, geometry onedimens, double initcond, double coef, vector <vector<double>>& u, int n) //функция вывода всего
{
	ofstream fout("output.txt");
	double l = onedimens.b - onedimens.a;

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

	fout << "Initial condition: "<<'\n';
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

	fout << "Solution of the equation: " << '\n';
	for (int i = 0; i <=n; i++)
	{
		fout << "Time: " << left.time[i] / 2 << '\t'<<"Temp: ";
		for (int j = 0; j <=l; j++)
			fout << u[i][j] << "\t\t";
		fout << '\n';
	}
}


void interpolation(geometry onedimens, boundary& left, boundary& right, double h, double tau, int n, int N)
{

	int i,j;
	double k;
	for (i = 0; i < left.temp_table.size()-1; i++)
	{
		int n = (left.time_table[i + 1] - left.time_table[i]) / tau;
		k = (left.temp_table[i + 1] - left.temp_table[i]) /n;
		for (j = 0; j <n; j++)
		{
			left.time[j+n*i] = j+n*i;
			left.temp[j+n*i] = left.temp_table[i] + k *j;//Формула интреполяции (линейной)
		}
	}
	left.time[n] = 2*left.time_table[N-1];
	left.temp[n] = left.temp_table[N-1];

	for (i = 0; i < right.temp_table.size() - 1; i++)
	{
		int n = (right.time_table[i + 1] - right.time_table[i]) / tau;
		k = (right.temp_table[i + 1] - right.temp_table[i]) / n;
		for (j = 0; j < n; j++)
		{
			right.time[j + n * i] = j + n * i;
			right.temp[j + n * i] = right.temp_table[i] + k * j;//Формула интреполяции (линейной)
		}
	}
	right.time[n] = 2 * right.time_table[N - 1];
	right.temp[n] = right.temp_table[N - 1];
}

void conductivity(geometry onedimens, boundary left, boundary right, double tau, double h, double initcond, double coef, vector <vector<double>> &u, int n)
{
	double gamma = tau * coef / (h * h);
	double l = onedimens.b - onedimens.a;
	int i,j;

	u.resize(n+1);
	for (i = 0; i <=n; i++) u[i].resize(l + 1);

	u[0][0] = left.temp[0];
	u[0][l] = right.temp[0];

	for (i = 0; i < l; i++)
		u[0][i] = initcond;



	for (i = 0; i <=n; i++)
	{
		u[i][0] = left.temp[i];
		u[i][l] = right.temp[i];
		if (i == n)
		{
			u[i][0] = left.temp[i];
			u[i][l] = right.temp[i];
		}
		else 
			for (j = 1; j < l; j++)
				u[i + 1][j] = gamma * u[i][j - 1] + (1 - 2 * gamma) * u[i][j] + gamma * u[i][j + 1]; //F[i][j] = f(x,t)=0
		
	}
}