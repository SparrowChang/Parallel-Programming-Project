1.要直接使用原始程式產生圖檔的話請到:\Code\mandelbrot\Debug中,打開cmd.exe並輸入Test.bat,等待出現"Finish!"後則會依照"input.txt"的設定值的檔名(如512 512 1024 -1.5 0.5 -1.0 1.0 = 512_512_1024_-1.500000_0.500000_-1.000000_1.000000.ppm)產生出.ppm的圖檔;MPI以及OpenMP也是一樣,不過差別在於先不要動"input.txt"的設定值以免無法執行成功,這與程式的寫法有關.

2.請先確認有"Visual Studio 2017"或者相容的版本才可以打開程式並且編譯,安裝可參考"Tools"資料夾.

3.請先確認有"IrfanView"才可以開啟圖檔,安裝可參考"Tools"資料夾.

4.請先安裝HpcClient_x64以及mpi_x64才能編譯MPI的程式.可參考"https://www.youtube.com/watch?v=jUQYiEgcEvE"

5.若無法編譯OpenMP的程式請參考"https://cg2010studio.com/2011/10/20/openmp-visual-studio-%E4%BD%BF%E7%94%A8-openmp/"去改Property.