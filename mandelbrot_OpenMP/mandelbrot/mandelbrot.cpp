// mandelbrot.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <omp.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
using namespace std;

#define IMAGE_WIDTH			4096
#define IMAGE_HEIGHT		4096
#define THREAD_CNT			8
// #define THREAD_SCHEDULE		static
#define THREAD_SCHEDULE		dynamic

double dfArrCr[IMAGE_HEIGHT][IMAGE_WIDTH];
double dfArrCi[IMAGE_HEIGHT][IMAGE_WIDTH];
int i32ArrpN[IMAGE_HEIGHT][IMAGE_WIDTH];
int r[IMAGE_HEIGHT][IMAGE_WIDTH];
int g[IMAGE_HEIGHT][IMAGE_WIDTH];
int b[IMAGE_HEIGHT][IMAGE_WIDTH];

int main(int argc, char *argv[])
{
	clock_t tStart = clock();
	ifstream fin("input.txt");
	int imageWidth, imageHeight, maxN;
	double minR, maxR, minI, maxI;
	int i, y, x;
	double zr = 0.0, zi = 0.0;
	double temp;
	double range1;
	double range2;
	int i32ThreadNum = atoi(argv[1]);
	
	if (!fin)
	{
		cout << "Could not open file!" << endl;
		cin.ignore();
		return 0;
	}
	fin >> imageWidth >> imageHeight >> maxN;
	fin >> minR >> maxR >> minI >> maxI;
	fin.close();
	
	range1 = maxR - minR;
	range2 = maxI - minI;	
#if 1
	if( (dfArrCr!=nullptr)&&(dfArrCi!=nullptr)&&(i32ArrpN!=nullptr) )
	{
		#pragma omp parallel num_threads(i32ThreadNum) private(y,x)
		{
			#pragma omp for schedule(THREAD_SCHEDULE)
			for (y = 0; y < imageHeight; y++)
			{
				for (x = 0; x < imageWidth; x++)
				{
					dfArrCr[y][x] =  x * (range1 / imageWidth) + minR;
					dfArrCi[y][x] =  y * (range2 / imageHeight) + minI;
				}
			}
		}
		
		#pragma omp parallel num_threads(i32ThreadNum)  private(x, y, zr, zi, temp, i)
		{
			#pragma omp for schedule(THREAD_SCHEDULE)
			for (y = 0; y < imageHeight; y++)
			{
				for (x = 0; x < imageWidth; x++)
				{
					zr = 0.0;
					zi = 0.0;
					for (i=0 ; (i< maxN)&&(zr*zr+zi*zi <4.0) ; i++)
					{
						temp = zr * zr - zi * zi + dfArrCr[y][x];
						zi = 2.0*zr*zi + dfArrCi[y][x];
						zr = temp;
					}
					i32ArrpN[y][x] = i;
				}
			}
		}

		#pragma omp parallel num_threads(i32ThreadNum)  private(x, y)
		{
			#pragma omp for schedule(THREAD_SCHEDULE)
			for (y = 0; y < imageHeight; y++)
			{
				for (x = 0; x < imageWidth; x++)
				{
					r[y][x] = (((int)(i32ArrpN[y][x]*sinf(i32ArrpN[y][x]))) % 256);
					g[y][x] = ((i32ArrpN[y][x]*2) % 256);
					b[y][x] = (i32ArrpN[y][x] % 256);
				}
			}
		}

#if 1
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
		
		for (y = 0; y < imageHeight; y++)
		{
			for (x = 0; x < imageWidth; x++)
			{
				fout << r[y][x] << " " << g[y][x] << " " << b[y][x] << " ";
			}
			fout << endl;
		}
		fout.close();
#endif		
	}
#endif
	cout << "Finished!" << endl;
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
