#include<iostream>  
#include<vector>  
#include<algorithm>  
#include<opencv2\core\core.hpp>  
#include<opencv2\highgui\highgui.hpp>  
#include<opencv2\imgproc\imgproc.hpp>  
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;
int main(int argc, char*argv[])
{
	Mat image = imread("fog.jpg", 1);
	CV_Assert(!image.empty() && image.channels() == 3);
	//ͼƬ�Ĺ�һ��  
	Mat fImage;
	image.convertTo(fImage, CV_32FC3, 1.0 / 255, 0);
	//�涨patch�Ĵ�С,�Ҿ�Ϊ����  
	int hPatch = 15;
	int vPatch = 15;
	//����һ����ͼƬ��ӱ߽�  
	Mat fImageBorder;
	copyMakeBorder(fImage, fImageBorder, vPatch / 2, vPatch / 2, hPatch / 2, hPatch / 2, BORDER_REPLICATE);
	//����ͨ��  
	vector<Mat> fImageBorderVector(3);
	split(fImageBorder, fImageBorderVector);
	//����darkChannel  
	Mat darkChannel(image.rows, image.cols, CV_32FC1);
	double minTemp, minPixel;
	//����darkChannel�Ķ���  
	for (unsigned int r = 0; r < darkChannel.rows; r++)
	{
		for (unsigned int c = 0; c < darkChannel.cols; c++)
		{
			minPixel = 1.0;
			for (vector<Mat>::iterator it = fImageBorderVector.begin(); it != fImageBorderVector.end(); it++)
			{
				Mat roi(*it, Rect(c, r, hPatch, vPatch));
				minMaxLoc(roi, &minTemp);
				minPixel = min(minPixel, minTemp);
			}
			darkChannel.at<float>(r, c) = float(minPixel);
		}
	}

	/*��2������� A(global atmospheric light)*/
	//2.1 �����darkChannel��,ǰtop������ֵ,������ȡֵΪ0.1%  
	float top = 0.001;
	float numberTop = top*darkChannel.rows*darkChannel.cols;
	Mat darkChannelVector;
	darkChannelVector = darkChannel.reshape(1, 1);
	Mat_<int> darkChannelVectorIndex;
	sortIdx(darkChannelVector, darkChannelVectorIndex, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	//��������  
	Mat mask(darkChannelVectorIndex.rows, darkChannelVectorIndex.cols, CV_8UC1);//ע��mask�����ͱ�����CV_8UC1  
	for (unsigned int r = 0; r < darkChannelVectorIndex.rows; r++)
	{
		for (unsigned int c = 0; c < darkChannelVectorIndex.cols; c++)
		{
			if (darkChannelVectorIndex.at<int>(r, c) <= numberTop)
				mask.at<uchar>(r, c) = 1;
			else
				mask.at<uchar>(r, c) = 0;
		}
	}
	Mat darkChannelIndex = mask.reshape(1, darkChannel.rows);
	vector<double> A(3);//�ֱ��ȡA_b,A_g,A_r  
	vector<double>::iterator itA = A.begin();
	vector<Mat>::iterator it = fImageBorderVector.begin();
	//2.2�����������t(x)ʱ�����õ����µľ������������ǰ���  
	vector<Mat> fImageBorderVectorA(3);
	vector<Mat>::iterator itAA = fImageBorderVectorA.begin();
	for (; it != fImageBorderVector.end() && itA != A.end() && itAA != fImageBorderVectorA.end(); it++, itA++, itAA++)
	{
		Mat roi(*it, Rect(hPatch / 2, vPatch / 2, darkChannel.cols, darkChannel.rows));
		minMaxLoc(roi, 0, &(*itA), 0, 0, darkChannelIndex);//  
		(*itAA) = (*it) / (*itA); //[ע�⣺����ط��г��ţ�����û���ж��Ƿ����0]  
	}

	/*����������t(x)*/
	Mat darkChannelA(darkChannel.rows, darkChannel.cols, CV_32FC1);
	float omega = 0.95;//0<w<=1,������ȡֵΪ0.95  
	//�������darkChannel��ʱ��,������  
	for (unsigned int r = 0; r < darkChannel.rows; r++)
	{
		for (unsigned int c = 0; c < darkChannel.cols; c++)
		{
			minPixel = 1.0;
			for (itAA = fImageBorderVectorA.begin(); itAA != fImageBorderVectorA.end(); itAA++)
			{
				Mat roi(*itAA, Rect(c, r, hPatch, vPatch));
				minMaxLoc(roi, &minTemp);
				minPixel = min(minPixel, minTemp);
			}
			darkChannelA.at<float>(r, c) = float(minPixel);
		}
	}
	Mat tx = 1.0 - omega*darkChannelA;

	/*���Ĳ������ǿ�����J(x)*/
	float t0 = 0.1;//������ȡt0 = 0.1  
	Mat jx(image.rows, image.cols, CV_32FC3);
	for (size_t r = 0; r < jx.rows; r++)
	{
		for (size_t c = 0; c<jx.cols; c++)
		{
			jx.at<Vec3f>(r, c) = Vec3f((fImage.at<Vec3f>(r, c)[0] - A[0]) / max(tx.at<float>(r, c), t0) + A[0], (fImage.at<Vec3f>(r, c)[1] - A[1]) / max(tx.at<float>(r, c), t0) + A[1], (fImage.at<Vec3f>(r, c)[2] - A[2]) / max(tx.at<float>(r, c), t0) + A[2]);
		}
	}
	namedWindow("fog_jx", 1);
	imshow("fog_jx", jx);
	Mat jx8U;
	jx.convertTo(jx8U, CV_8UC3, 255, 0);
	imwrite("fog_jx.jpg", jx8U);

	namedWindow("darkChannel", 1);
	imshow("darkChannel", darkChannel);
	Mat darkChannel8U;
	darkChannel.convertTo(darkChannel8U, CV_8UC1, 255, 0);
	imwrite("fog_darkChannel.jpg", darkChannel8U);
	return 0;
}