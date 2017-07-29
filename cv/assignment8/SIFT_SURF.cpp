#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

int main()
{
	//load the image
	IplImage* img = cvLoadImage("SIFT_SURF.png", 0);
	Mat featureImgSIFT(img, true);
	Mat featureImgSURF(img, true);

	// �����������
	vector<KeyPoint>keypoints;
	// ����SIFT���������
	SiftFeatureDetector sift(
		0.03,  // ��������ֵ
		10.);  // ���ڽ���
	// ���SIFT����ֵ
	sift.detect(img, keypoints);
	// ����SIFT����ͼ
	drawKeypoints(featureImgSIFT, // ԭʼͼ��
		keypoints, // �����������
		featureImgSIFT, // ����ͼ��
		cv::Scalar(255, 255, 255), // ���������ɫ
		cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // ��־λ
	cvSaveImage("featureImgSIFT.png", &IplImage(featureImgSIFT));

	// �����������
	vector<KeyPoint>keypoints2;
	// ����SURF���������
	SurfFeatureDetector surf(2500); 
	// ���SURF����ֵ
	surf.detect(img, keypoints2);
	// ����SIFT����ͼ
	drawKeypoints(featureImgSURF, // ԭʼͼ��
		keypoints2, // �����������
		featureImgSURF, // ����ͼ��
		cv::Scalar(255, 255, 255), // ���������ɫ
		cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // ��־λ
	cvSaveImage("featureImgSURF.png", &IplImage(featureImgSURF));


	cvReleaseImage(&img);
	featureImgSIFT.release();
	featureImgSURF.release();
}