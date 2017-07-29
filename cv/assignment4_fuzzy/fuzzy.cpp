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

const double P1 = 0.3964;
const double P2 = 0.2;

const string filename = "fuzzy.png";
const string resultname = "fuzzyresult.png";

vector<double> getHist(IplImage* img)
{
	//init the histogram
	vector<double> hist;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		hist.push_back(0);
	}

	//get the data of histogram
	for (int row = 0; row < img->height; row++)
	{
		uchar* ptr = (uchar*)img->imageData + row*img->widthStep;
		for (int col = 0; col < img->width; col++)
		{
			int temp_gray_value = ptr[col];
			hist[temp_gray_value]++;
		}
	}

	return hist;
}

void histogramEqualization(IplImage* img)
{
	//init the histogram
	vector<double> hist = getHist(img);

	//normalization
	int totalpoints = img->height * img->width;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		hist[i] = hist[i] / totalpoints;
	}

	//sum the normalized histogram
	double sum_hist[MAX_GRAY_VALUE];
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		if (i == 0)
		{
			sum_hist[i] = hist[0];
		}
		else
		{
			sum_hist[i] = sum_hist[i - 1] + hist[i];
		}
	}

	//find the max gray and the min gray
	int max_gray = 0;
	int min_gray = 255;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		if (hist[i] > 0 && i > max_gray)max_gray = i;//find the max index that hist[index] > 0
		if (hist[i] > 0 && i < min_gray)min_gray = i;//find the min index that hist[index] < 0
	}

	//equalize the origin img
	for (int row = 0; row < img->height; row++)
	{
		uchar* ptr = (uchar*)img->imageData + row*img->widthStep;
		for (int col = 0; col < img->width; col++)
		{
			int result_gray_value = sum_hist[ptr[col]] * (max_gray - min_gray) + min_gray;
			ptr[col] = result_gray_value;
		}
	}
}

double fdmhelper(int a, int c,int x)
{
	double b = (a + c) / 2;

	if (x <= a)return 0;
	else if (x > a && x <= b)
	{
		double temp = (x - a) / (double)(c - a);
		return 2 * temp*temp;
	}
	else if (x > b && x < c)
	{
		double temp = (x - c) / (double)(c - a);
		return 1 - 2 * temp*temp;
	}
	else return 1;
}

double fdm(int a, int c)
{
	int n = MAX_GRAY_VALUE;
	double meanU = fdmhelper(a, c, (a+c)/2);

	double sum = 0;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		double ui = fdmhelper(a, c, i);
		sum += abs(ui - meanU);
	}

	return 2 * sum / (double)n;
}

int main()
{
	//load the image
	IplImage* img = cvLoadImage(filename.c_str(), 0);

	vector<double> hist = getHist(img);

	//if we need to do histogram equalization
	int pixelB = 0;
	int pixelW = 0;
	for (int i = 0; i < 128; i++)
	{
		pixelB += hist[i];
	}
	pixelW = img->height*img->width - pixelB;
	double Pmin = P2 * img->height * img->width;
	if (pixelB < Pmin || pixelW < Pmin)
	{
		histogramEqualization(img);//modify img->imageData by HE
	}

	//reset the hist because data has been changed
	hist = getHist(img);
	//recalculate because data has been changed
	pixelB = 0;
	pixelW = 0;
	for (int i = 0; i < 128; i++)
	{
		pixelB += hist[i];
		pixelW += hist[i + 128];
	}
	int minPixelB = P1*pixelB;
	int minPixelW = P1*pixelW;

	//calculate xl and xr
	int xl = 0;
	int xr = 128;
	int tempSum = 0;
	for (; xl < 128; xl++)
	{
		tempSum += hist[xl];
		if (tempSum >= minPixelB)
		{
			break;
		}
	}
	for (tempSum = 0; xr < MAX_GRAY_VALUE; xr++)
	{
		tempSum += hist[xr];
		if (tempSum >= minPixelW)
		{
			break;
		}
	}

	//calculate meanGray
	/*int sumGray = 0;
	int sumPoint = 0;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		sumGray += i*hist[i];
		sumPoint += hist[i];
	}
	int meanGray = sumGray / sumPoint;*/

	//calculate factor
	int xmin, xmax;
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		if (hist[i]>0)
		{
			xmin = i;
			break;
		}
	}
	for (int i = MAX_GRAY_VALUE - 1; i >= 0; i--)
	{
		if (hist[i] > 0)
		{
			xmax = i;
			break;
		}
	}
	double factor = fdm(xr, xmax) / fdm(xmin, xl);
	
	//calculate the threshold
	int threshold;
	for (int i = xl + 1; i < xr; i++)
	{
		double left = fdm(xl, i);
		double right = fdm(i, xr);
		if (left * factor > right)
		{
			continue;
		}
		else
		{
			threshold = i;
			break;
		}
	}
	cout << "Threshold is: " << threshold << endl;

	//thresholding image
	for (int row = 0; row < img->height; row++)
	{
		uchar* ptr = (uchar*)img->imageData + row*img->widthStep;
		for (int col = 0; col < img->width; col++)
		{
			if (ptr[col] > threshold)
			{
				ptr[col] = 255;
			}
			else
			{
				ptr[col] = 0;
			}
		}
	}

	//save the result image
	cvSaveImage(resultname.c_str(), img);

	cvReleaseImage(&img);
	system("pause");
}