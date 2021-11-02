using namespace std;

extern double h, tau;

void conductivity(string file1, string file2, geometry onedimens, boundary left, boundary right, vector<double>& u, vector<double>& u_, times &time)
{
	double l = onedimens.b - onedimens.a;
	double initcond, coef;
	int N = left.time_table.size();
	short k = 0;
	
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


	time.duration = left.time_table[N - 1];
	time.actual = 0;
	time.step = tau;
	cout << "Enter time step for the output (by default enter 0.5): ";
	cin >> time.output;
	while (time.output<time.step)
	{
		cout << "Error! the step for output cannot be less than the step for calculation" << '\n';
		cout << "Enter time step for the output (by default enter 0.5): ";
		cin >> time.output;
	}
	time.n = time.output / time.step;
	double gamma = time.step * coef / (h * h);
	int i, j;

	u.resize(l + 1);
	u_.resize(l + 1);

	u[0] = left.temp_table[0];
	u[l] = right.temp_table[0];
	for (i = 1; i < l; i++)
		u[i] = initcond;
	fout << "Solution of the equation: " << '\n';
	if (time.n != 1)
	{
		fout << "Time: " << time.actual << "\t\t" << "Temp: ";
		for (int j = 0; j <= l; j++)
		{
			fout << u[j] << "\t\t";
		}
		fout << '\n';

	}
	while (time.actual < time.duration)
	{
		k++;
		if ((k-1) % time.n==0)
		{
			fout << "Time: " << time.actual << "\t\t" << "Temp: ";
			for (int j = 0; j <= l; j++)
			{
				fout << u[j] << "\t\t";
				u_[j] = u[j];
			}
			fout << '\n';
		}
		for (int j = 0; j <= l; j++)
			u_[j] = u[j];
		time.actual += time.step;
		i = 0;
		while (left.time_table[i] < time.actual) i++;
		u[0] = left.temp_table[i - 1] + ((left.temp_table[i] - left.temp_table[i - 1]) / (left.time_table[i] - left.time_table[i - 1])) * (time.actual - left.time_table[i - 1]);
		i = 0;
		while (right.time_table[i] < time.actual) i++;
		u[l] = right.temp_table[i - 1] + ((right.temp_table[i] - right.temp_table[i - 1]) / (right.time_table[i] - right.time_table[i - 1])) * (time.actual - right.time_table[i - 1]);
		for (int j = 1; j < l; j++)
			u[j] = gamma * u_[j - 1] + (1 - 2 * gamma) * u_[j] + gamma * u_[j + 1]; //F[i][j] = f(x,t)=0
	}

	fout << "Time: " << time.actual<< "\t\t" << "Temp: ";
	for (int j = 0; j <= l; j++)
		fout << u[j] << "\t\t";
	fout.close();
}