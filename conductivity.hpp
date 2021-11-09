using namespace std;

extern double h, tau;
extern int numb;

void conductivity(string file1, string file2, geometry onedimens, boundary left, boundary right, vector<double>& u, vector<double>& u_, times& time)
{
	double l = onedimens.b - onedimens.a;
	double initcond, coef;
	int Nl = left.time_table.size();
	int Nr = right.time_table.size();
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

	//set time class and define the duration by last value of left and right boundary condition
	if (left.time_table[Nl - 1] > right.time_table[Nr - 1]) time.duration = left.time_table[Nl - 1];
	else time.duration = right.time_table[Nr - 1];
	time.actual = 0;
	time.step = tau;
	cout << "Enter time step for the output (by default enter 0.5): ";
	cin >> time.output;
	while (time.output < time.step)
	{
		cout << "Error! the step for output cannot be less than the step for calculation" << '\n';
		cout << "Enter time step for the output (by default enter 0.5): ";
		cin >> time.output;
	}
	time.n = time.output / time.step;
	double gamma = time.step * coef / (h * h);
	int i, j;

	u.resize(numb + 1);
	u_.resize(numb + 1);

	//set the initial values
	u[0] = left.temp_table[0];
	u[l] = right.temp_table[0];
	for (i = 1; i < numb; i++)
		u[i] = initcond;
	fout << "Solution of the equation: " << '\n';

	//output the initial values
	fout << "Time: " << time.actual << "\t\t" << "Temp: ";
	for (int j = 0; j <= numb; j++)
	{
		fout << u[j] << "\t\t";
	}
	fout << '\n';

	//solution and output of the thermal conductivity equation
	while (time.actual < time.duration)
	{
		if (k == time.n)
		{
			fout << "Time: " << time.actual << "\t\t" << "Temp: ";
			for (int j = 0; j <= numb; j++)
			{
				fout << u[j] << "\t\t";
				u_[j] = u[j];
			}
			fout << '\n';
			k = 0;
		}

		for (int j = 0; j <= numb; j++)
			u_[j] = u[j];

		time.actual += time.step;
		u[0] = left.get(time.actual);
		u[numb] = right.get(time.actual);
		for (int j = 1; j < numb; j++)
			u[j] = gamma * u_[j - 1] + (1 - 2 * gamma) * u_[j] + gamma * u_[j + 1]; //F[i][j] = f(x,t)=0
		k++;
	}

	fout << "Time: " << time.actual << "\t\t" << "Temp: ";
	for (int j = 0; j <= numb; j++)
		fout << u[j] << "\t\t";
	fout.close();
}