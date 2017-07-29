#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

int main()
{
	////load the image
	Mat img_obj = imread("SIFT_SURF_1obj.png");
	Mat img_env = imread("SIFT_SURF_1env.png");

	int minHessian = 400;

	vector<KeyPoint> keypoints_obj, keypoints_env;
	Mat descriptors_obj, descriptors_env;

	//SiftFeatureDetector sift_detector(minHessian);
	//sift_detector.detect(img_obj, keypoints_obj);
	//sift_detector.detect(img_env, keypoints_env);

	//SiftDescriptorExtractor sift_extractor;
	//sift_extractor.compute(img_obj, keypoints_obj, descriptors_obj);
	//sift_extractor.compute(img_env, keypoints_env, descriptors_env);

	SurfFeatureDetector surf_detector(minHessian);
	surf_detector.detect(img_obj, keypoints_obj);
	surf_detector.detect(img_env, keypoints_env);

	SurfDescriptorExtractor surf_extractor;
	surf_extractor.compute(img_obj, keypoints_obj, descriptors_obj);
	surf_extractor.compute(img_env, keypoints_env, descriptors_env);

	BruteForceMatcher< L2<float> > matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_obj, descriptors_env, matches);

	Mat img_matches;
	drawMatches(img_obj, keypoints_obj, img_env, keypoints_env, matches, img_matches);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_obj[matches[i].queryIdx].pt);
		scene.push_back(keypoints_env[matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, CV_RANSAC);

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(img_obj.cols, 0);
	obj_corners[2] = cvPoint(img_obj.cols, img_obj.rows);
	obj_corners[3] = cvPoint(0, img_obj.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	line(img_matches, scene_corners[0] + Point2f(img_obj.cols, 0), scene_corners[1] + Point2f(img_obj.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_obj.cols, 0), scene_corners[2] + Point2f(img_obj.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_obj.cols, 0), scene_corners[3] + Point2f(img_obj.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_obj.cols, 0), scene_corners[0] + Point2f(img_obj.cols, 0), Scalar(0, 255, 0), 4);

	//imwrite("SIFT__1result.png", img_matches);
	imwrite("SURF__4result.png", img_matches);
}