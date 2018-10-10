close all;clear;clc
addpath('C:\Users\George\Desktop\TexRelated\matlab2tikz-master\matlab2tikz-master\src')

C_1 = textscan(fopen(''), '%f %f %f %f %f %f %f %f');
C_2 = textscan(fopen('C:\Users\George\Desktop\Parallel_and_distributed\diades_data\diades_data\openmp_data_rec_20.txt'), '%f %f %f %f %f %f %f %f');
C_3 = textscan(fopen('C:\Users\George\Desktop\Parallel_and_distributed\diades_data\diades_data\openmp_data_rec_24.txt'), '%f %f %f %f %f %f %f %f');

time16(1,1) = mean(C_1{1});
time16(2,1) = mean(C_1{2});
time16(3,1) = mean(C_1{3});
time16(4,1) = mean(C_1{4});
time16(5,1) = mean(C_1{5});
time16(6,1) = mean(C_1{6});
time16(7,1) = mean(C_1{7});
time16(8,1) = mean(C_1{8});

time20(1,1) = mean(C_2{1});
time20(2,1) = mean(C_2{2});
time20(3,1) = mean(C_2{3});
time20(4,1) = mean(C_2{4});
time20(5,1) = mean(C_2{5});
time20(6,1) = mean(C_2{6});
time20(7,1) = mean(C_2{7});
time20(8,1) = mean(C_2{8});

time24(1,1) = mean(C_3{1});
time24(2,1) = mean(C_3{2});
time24(3,1) = mean(C_3{3});
time24(4,1) = mean(C_3{4});
time24(5,1) = mean(C_3{5});
time24(6,1) = mean(C_3{6});
time24(7,1) = mean(C_3{7});
time24(8,1) = mean(C_3{8});



x = 1:8;

 plot(x,time16)

 xlabel('number of threads = 2\^x')
 ylabel('mean Time')
 title('mean time for input of 2\^16')
 matlab2tikz('openmp_rec_16.tex');
 
 plot(x,time20)

 xlabel('number of threads = 2\^x')
 ylabel('mean Time')
 title('mean time for input of 2\^20')
 matlab2tikz('openmp_rec_20.tex');
 
 plot(x,time24)

 xlabel('number of threads = 2\^x')
 ylabel('mean Time')
 title('mean time for input of 2\^24')
 matlab2tikz('openmp_rec_24.tex');
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 