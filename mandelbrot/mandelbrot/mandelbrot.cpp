// mandelbrot.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
using namespace std;

/*
while( x*x + y*y < 2.2 AND iteration < max_iteration )
{
	xtemp = x*x - y*y +x0;
	y = 2*x*y + y0;
	x = xtemp;
	iteration = iteration +1;
}
*/

int findMandelbrot(double cr, double ci, int max_iterations)
{
	int i = 0;
	double zr = 0.0, zi = 0.0;
	while (i< max_iterations && zr*zr+zi*zi <4.0)
	{
		double temp = zr * zr - zi * zi + cr;
		zi = 2.0*zr*zi + ci;
		zr = temp;
		i++;
	}
	return i;
}

double mapToReal(int x, int imageWidth, double minR, double maxR)
{
	double range = maxR - minR;
	return x * (range / imageWidth) + minR;
}

double mapToImaginary(int y, int imageHeight, double minI, double maxI)
{
	double range = maxI - minI;
	return y * (range / imageHeight) + minI;
}

int main()
{
	clock_t tStart = clock();
	ifstream fin("input.txt");
	int imageWidth, imageHeight, maxN;
	double minR, maxR, minI, maxI;
	if (!fin)
	{
		cout << "Could not open file!" << endl;
		cin.ignore();
		return 0;
	}
	fin >> imageWidth >> imageHeight >> maxN;
	fin >> minR >> maxR >> minI >> maxI;
	fin.close();

	ofstream fout(to_string(imageWidth)+"_"
			+ to_string(imageHeight)+"_"
			+ to_string(maxN)+"_"
			+ to_string(minR)+"_"
			+ to_string(maxR)+"_"
			+ to_string(minI)+"_"
			+ to_string(maxI)+".ppm");
	fout << "P3" << endl;
	fout << imageWidth << " " << imageHeight << endl;
	fout << "256" << endl;

	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			double cr = mapToReal(x, imageWidth, minR, maxR);
			double ci = mapToImaginary(y, imageHeight, minI, maxI);
			int n = findMandelbrot(cr, ci, maxN);

			int r = (((int)(n*sinf(n))) % 256);
			int g = ((n*2) % 256);
			int b = (n % 256);

			fout << r << " " << g << " " << b << " ";
		}
		fout << endl;
	}
	fout.close();
	cout << "Finished!" << endl;
	// printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的秘訣: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
