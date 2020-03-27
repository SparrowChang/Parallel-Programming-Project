// mandelbrot.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <omp.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <mpi.h>
using namespace std;

#define IMAGE_WIDTH			4096
#define IMAGE_HEIGHT		4096
#define	SEND_RECV_CNT		12

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
	double range1;
	double range2;
	
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
	
	MPI_Request Request[SEND_RECV_CNT];
	MPI_Status Status[SEND_RECV_CNT];	
	MPI_Init(&argc, &argv);

	int i32TotalRankCnt, i32RankId;
    MPI_Comm_size(MPI_COMM_WORLD, &i32TotalRankCnt);
    MPI_Comm_rank(MPI_COMM_WORLD, &i32RankId);	
	int const i32ElePerBlock = IMAGE_WIDTH*IMAGE_HEIGHT/i32TotalRankCnt;
	int const i32BlockCnt = IMAGE_WIDTH/i32TotalRankCnt;
	int const i32LoopStart = i32RankId*i32BlockCnt;
	int const i32LoopEnd = i32BlockCnt+i32LoopStart;
	int i32Prev = 0;
	int i32Next = 0;
	int i32InitCnt = 1;	
	int i32OffsetBack = i32Prev*i32BlockCnt;
	int i32StFor = i32RankId*i32BlockCnt;	
	int i, ii, y, x;
	double temp;
	double zr = 0.0, zi = 0.0;
	
	for (i=i32LoopStart ; i<i32LoopEnd ; i++)
	{
		for (x = 0; x < imageWidth; x++)
		{
			dfArrCr[i][x] =  x * (range1 / imageWidth) + minR;
			dfArrCi[i][x] =  i * (range2 / imageHeight) + minI;
			
			ii = 0;
			zr = 0.0;
			zi = 0.0;
			while (ii< maxN && zr*zr+zi*zi <4.0)
			{
				temp = zr * zr - zi * zi + dfArrCr[i][x];
				zi = 2.0*zr*zi + dfArrCi[i][x];
				zr = temp;
				ii++;
			}
			i32ArrpN[i][x] = ii;
			
			r[i][x] = (((int)(i32ArrpN[i][x]*sinf(i32ArrpN[i][x]))) % 256);
			g[i][x] = ((i32ArrpN[i][x]*2) % 256);
			b[i][x] = (i32ArrpN[i][x] % 256);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	i32InitCnt = 1;
	while( i32InitCnt<i32TotalRankCnt )
	{
		i32Prev = i32RankId-i32InitCnt;
		i32Next = i32RankId+i32InitCnt;
		while( i32Next>=i32TotalRankCnt ) i32Next -= i32TotalRankCnt;
		while( i32Prev<0 ) i32Prev += i32TotalRankCnt;
		
		i32OffsetBack = i32Prev*i32BlockCnt;
		i32StFor = i32RankId*i32BlockCnt;
	
		MPI_Irecv(&dfArrCr[i32OffsetBack][0] , i32ElePerBlock , MPI_DOUBLE , i32Prev , 0 ,MPI_COMM_WORLD, &Request[0] );
		MPI_Isend(&dfArrCr[i32StFor][0] , i32ElePerBlock , MPI_DOUBLE , i32Next , 0 ,MPI_COMM_WORLD, &Request[1] );		

		MPI_Irecv(&dfArrCi[i32OffsetBack][0] , i32ElePerBlock , MPI_DOUBLE , i32Prev , 0 ,MPI_COMM_WORLD, &Request[2] );
		MPI_Isend(&dfArrCi[i32StFor][0] , i32ElePerBlock , MPI_DOUBLE , i32Next , 0 ,MPI_COMM_WORLD, &Request[3] );	

		MPI_Irecv(&i32ArrpN[i32OffsetBack][0] , i32ElePerBlock , MPI_INT , i32Prev , 0 ,MPI_COMM_WORLD, &Request[4] );
		MPI_Isend(&i32ArrpN[i32StFor][0] , i32ElePerBlock , MPI_INT , i32Next , 0 ,MPI_COMM_WORLD, &Request[5] );	

		MPI_Irecv(&r[i32OffsetBack][0] , i32ElePerBlock , MPI_INT , i32Prev , 0 ,MPI_COMM_WORLD, &Request[6] );
		MPI_Isend(&r[i32StFor][0] , i32ElePerBlock , MPI_INT , i32Next , 0 ,MPI_COMM_WORLD, &Request[7] );	

		MPI_Irecv(&g[i32OffsetBack][0] , i32ElePerBlock , MPI_INT , i32Prev , 0 ,MPI_COMM_WORLD, &Request[8] );
		MPI_Isend(&g[i32StFor][0] , i32ElePerBlock , MPI_INT , i32Next , 0 ,MPI_COMM_WORLD, &Request[9] );	

		MPI_Irecv(&b[i32OffsetBack][0] , i32ElePerBlock , MPI_INT , i32Prev , 0 ,MPI_COMM_WORLD, &Request[10] );
		MPI_Isend(&b[i32StFor][0] , i32ElePerBlock , MPI_INT , i32Next , 0 ,MPI_COMM_WORLD, &Request[11] );	
		
		MPI_Waitall(SEND_RECV_CNT, Request, Status);
		i32InitCnt++;
	}
	MPI_Barrier(MPI_COMM_WORLD);

	if( i32RankId==0 )
	{
		// ofstream fout(to_string(imageWidth)+"_"
				// + to_string(imageHeight)+"_"
				// + to_string(maxN)+"_"
				// + to_string(minR)+"_"
				// + to_string(maxR)+"_"
				// + to_string(minI)+"_"
				// + to_string(maxI)+".ppm");
		// fout << "P3" << endl;
		// fout << imageWidth << " " << imageHeight << endl;
		// fout << "256" << endl;	
		
		// for (y = 0; y < imageHeight; y++)
		// {
			// for (x = 0; x < imageWidth; x++)
			// {
				// fout << r[y][x] << " " << g[y][x] << " " << b[y][x] << " ";
			// }
			// fout << endl;
		// }
		// fout.close();
		cout << "Finished!" << endl;		
	}
	MPI_Finalize();
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
