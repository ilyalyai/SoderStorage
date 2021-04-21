#include "lab7_lib.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	system("cls");
	cout << "Выберите задание:" << endl << "1- Скелетизация" << endl << "2- Срединные прямые" << endl <<
			"3- Текстуры монеток" << endl << "0- Выход" << endl;
	while (true)
	{
		char b = _getch();
		switch (b)
		{
		case '0':
			return 0;
		case '1':
			Skeletonization();
			break;
		case '2':
			VideoMedian();
			break;
		case '3':
			TossACoin();
			break;
		case '4':
			return 0;
		default:
			break;
		}
	}
}
