#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

const string path = "pos/";
const int num = 60;

int main()
{
	vector<Mat> origin_img;
	for (int index = 0; index < num; index++)
	{
		stringstream ss;
		string index_str;
		ss << index;
		ss >> index_str;
		Mat temp = imread(path + index_str + ".png", 1);
		origin_img.push_back(temp);
	}
	
	//æ˘∫‚ªØ
	/*vector<Mat> equal_img;
	for (int index = 0; index < origin_img.size(); index++)
	{
		Mat temp;
		vector<Mat> splitmat;
		split(origin_img[index], splitmat);
		for (int i = 0; i < origin_img[index].channels(); i++)
		{
			equalizeHist(splitmat[i], splitmat[i]);
		}
		merge(splitmat, temp);
		equal_img.push_back(temp);
	}
	for (int index = 0; index < equal_img.size(); index++)
	{
		stringstream ss;
		string index_str;
		ss << index + num;
		ss >> index_str;
		imwrite(path + index_str + ".png", equal_img[index]);
	}*/

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

	//≈Ú’Õ
	vector<Mat> dilate_img;
	for (int index = 0; index < origin_img.size(); index++)
	{
		Mat temp;
		vector<Mat> splitmat;
		split(origin_img[index], splitmat);
		for (int i = 0; i < origin_img[index].channels(); i++)
		{
			dilate(splitmat[i], splitmat[i], element);
		}
		merge(splitmat, temp);
		dilate_img.push_back(temp);
	}
	for (int index = 0; index < dilate_img.size(); index++)
	{
		stringstream ss;
		string index_str;
		ss << index + num;
		ss >> index_str;
		imwrite(path + index_str + ".png", dilate_img[index]);
	}

	//∏Ø ¥
	/*vector<Mat> erode_img;
	for (int index = 0; index < origin_img.size(); index++)
	{
		Mat temp;
		vector<Mat> splitmat;
		split(origin_img[index], splitmat);
		for (int i = 0; i < origin_img[index].channels(); i++)
		{
			erode(splitmat[i], splitmat[i], element);
		}
		merge(splitmat, temp);
		erode_img.push_back(temp);
	}
	for (int index = 0; index < erode_img.size(); index++)
	{
		stringstream ss;
		string index_str;
		ss << index + num;
		ss >> index_str;
		imwrite(path + index_str + ".png", erode_img[index]);
	}*/
}