#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

const int MAX_GRAY_VALUE = 256;
const double PI = 3.1415926;

class MyComplex
{
public:
	double real;
	double imag;

	MyComplex(double real,double imag)
	{
		this->real = real;
		this->imag = imag;
	}
};

MyComplex TriFThelper(const vector<vector<double>>& gray_value, int u, int v)
{
	double real = 0, imag = 0;
	int height = gray_value.size();
	int width = gray_value[0].size();
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			real = real + gray_value[row][col] * cos(2 * PI*(u*row / (double)height + v*col / (double)width));
			imag = imag - gray_value[row][col] * sin(2 * PI*(u*row / (double)height + v*col / (double)width));
		}
	}
	//real = real / (double)(height * width);
	//imag = imag / (double)(height * width);
	return MyComplex(real, imag);
}

void TriFourierTransform(vector<vector<MyComplex>>& frequency_value, const vector<vector<double>>& gray_value)
{
	int height = gray_value.size();
	int width = gray_value[0].size();
	for (int row = 0; row < height; row++)
	{
		vector<MyComplex> tempv;
		for (int col = 0; col < width; col++)
		{
			tempv.push_back(TriFThelper(gray_value, row, col));
		}
		frequency_value.push_back(tempv);
	}
}

int main()
{
	//load the image
	IplImage* img = cvLoadImage("fourier.png", 0);

	//get gray value of image
	vector<vector<double>> gray_value;
	for (int row = 0; row < img->height; row++)
	{
		uchar* ptr = (uchar*)img->imageData + row*img->widthStep;
		vector<double> tempv;
		for (int col = 0; col < img->width; col++)
		{
			tempv.push_back(ptr[col] * pow(-1, row + col));//原点移到图像中心
		}
		gray_value.push_back(tempv);
	}

	//Fourier Transform
	vector<vector<MyComplex>> frequency_value;
	TriFourierTransform(frequency_value, gray_value);

	//最大频率和最小频率
	int max_frequency = -1;
	int min_frequency = -1;

	//Spectrum
	IplImage* spectrum = cvCloneImage(img);
	for (int row = 0; row < spectrum->height; row++)
	{
		uchar* ptr = (uchar*)spectrum->imageData + row*spectrum->widthStep;
		for (int col = 0; col < spectrum->width; col++)
		{
			int temp_frequency = sqrt(pow(frequency_value[row][col].real, 2) + pow(frequency_value[row][col].imag, 2));
			if (max_frequency == -1 || temp_frequency > max_frequency)max_frequency = temp_frequency;
			if (min_frequency == -1 || temp_frequency < min_frequency)min_frequency = temp_frequency;
			ptr[col] = temp_frequency;
		}
	}
	cvSaveImage("Spectrum.png", spectrum);
	cvReleaseImage(&spectrum);

	//Phase Spectrum
	IplImage* phaseSpectrum = cvCloneImage(img);
	for (int row = 0; row < phaseSpectrum->height; row++)
	{
		uchar* ptr = (uchar*)phaseSpectrum->imageData + row*phaseSpectrum->widthStep;
		for (int col = 0; col < phaseSpectrum->width; col++)
		{
			ptr[col] = atan(frequency_value[row][col].imag / frequency_value[row][col].real);
		}
	}
	cvSaveImage("PhaseSpectrum.png", phaseSpectrum);
	cvReleaseImage(&phaseSpectrum);

	int max = 0, min = 0;
	cout << "max frequency: " << max_frequency << endl << "min frequency: " << min_frequency << endl;
	cout << "请输入保留的频率域：" << endl << "min: ";
	cin >> min;
	cout << "max: ";
	cin >> max;
	
	//滤波
	IplImage* fourierResult = cvCloneImage(img);
	cvSaveImage("FourierResultBefore.png", fourierResult);
	for (int row = 0; row < fourierResult->height; row++)
	{
		uchar* ptr = (uchar*)fourierResult->imageData + row*fourierResult->widthStep;
		for (int col = 0; col < fourierResult->width; col++)
		{
			int temp_frequency = sqrt(pow(frequency_value[row][col].real, 2) + pow(frequency_value[row][col].imag, 2));
			if (temp_frequency >= min && temp_frequency <= max)
			{
				//do nothing
			}
			else
			{
				ptr[col] = 0;//置为黑色
			}
		}
	}
	cvSaveImage("FourierResult.png", fourierResult);
	cvReleaseImage(&fourierResult);


	cvReleaseImage(&img);
	system("pause");
	return 0;
}