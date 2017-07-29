#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace cv;
using namespace std;

const int MAX_GRAY_VALUE = 256;

void drawHistogram(IplImage* img, string histName)
{
	//get histogram data
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0, 256 };
	const float* ranges[] = { range };
	MatND hist;//hist data
	int channels[] = { 0 };
	Mat gray(img, 0);
	//calculate hist and store the data into hist
	calcHist(&gray, 1, channels, Mat(), // do not use mask   
		hist, 1, hist_size, ranges,
		true, // the histogram is uniform   
		false);
	double max_val = 0;
	minMaxLoc(hist, 0, &max_val, 0, 0);//calculate the max of histogram(max num of occurences)
	//draw histogram
	int scale = 2;//width of hist
	int hist_height = 256;//height of hist
	Mat hist_img = Mat::zeros(hist_height, bins*scale, CV_8UC3);//image of histogram
	//load the data from hist to image
	for (int i = 0; i<bins; i++)
	{
		float bin_val = hist.at<float>(i);
		int intensity = cvRound(bin_val*hist_height / max_val);  //要绘制的高度   
		rectangle(hist_img, Point(i*scale, hist_height - 1),
			Point((i + 1)*scale - 1, hist_height - intensity),
			CV_RGB(255, 255, 255));
	}

	//cvShowImage(histName.c_str(), &IplImage(hist_img));//show
	cvSaveImage(histName.c_str(), &IplImage(hist_img));//save
}

int main()
{
	//load the image
	IplImage* img = cvLoadImage("underexposed.png", 0);

	//draw the histogram of origin image
	drawHistogram(img, "OriginHistogram.png");

	//init the histogram
	double hist[MAX_GRAY_VALUE];
	for (int i = 0; i < MAX_GRAY_VALUE; i++)
	{
		hist[i] = 0;
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

	//save the result img
	cvSaveImage("HEresult.png", img);
	//cvShowImage("HEresult.png", img);

	//draw the histogram of result image
	drawHistogram(img, "ResultHistogram.png");

	cvReleaseImage(&img);
	system("pause");
}