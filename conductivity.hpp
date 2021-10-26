using namespace std;

void conductivity(string file1, string file2, geometry onedimens, boundary left, boundary right, steps step, vector<double>& u, vector<double>& u_)
{
	double l = onedimens.b - onedimens.a;
	double initcond, coef;
	
	ifstream finin(file1);
	finin >> initcond;
	finin >> coef;
	finin.close();

	ofstream fout(file2);
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


	double gamma = step.tau * coef / (step.h * step.h);
	int i, j;

	u.resize(l + 1);
	u_.resize(l + 1);

	u[0] = left.temp[0];
	u[l] = right.temp[0];
	for (i = 1; i < l; i++)
		u[i] = initcond;

	fout << "Solution of the equation: " << '\n';

	for (i = 1; i <= step.n; i++)
	{
		fout << "Time: " << left.time[i - 1] / 2 << "\t\t" << "Temp: ";
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

	fout << "Time: " << left.time[step.n] / 2 << "\t\t" << "Temp: ";
	for (int j = 0; j <= l; j++)
		fout << u[j] << "\t\t";
	fout.close();
}