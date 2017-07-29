#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>  
#include <sstream>
#include <opencv2/ml/ml.hpp>  
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

const int GAP_X = 20;
const int GAP_Y = 140;

void dectorOverlap(vector<Rect> &found);
vector<Rect> deleteFalse(vector<Rect> &found);
vector<Rect> deleteFalseByX(vector<Rect> &found);
vector<Rect> deleteFalseByY(vector<Rect> &found);
bool rectCmpByX(Rect r1, Rect r2);
bool rectCmpByY(Rect r1, Rect r2);
bool groupsCmp(pair<int,int> g1, pair<int,int> g2);

int main()
{
	CascadeClassifier E_cascade;
	E_cascade.load("cascade.xml");

	/* ����ʶ�� */
	Mat img = imread("measure_imgs/4.jpg");
	vector<Rect>  found;

	E_cascade.detectMultiScale(img, found, 1.01, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(0, 0), Size(70, 70));
	
	/* ȥ���ص��ľ��� */
	dectorOverlap(found);

	/* ����ˮ�ߵ���״ȥ��һЩ���� */
	deleteFalse(found);

	for (int i = 0; i<found.size(); i++){
		Rect r = found[i];

		//r.x += cvRound(r.width);
		//r.width = cvRound(r.width);
		//r.y += cvRound(r.height);
		//r.height = cvRound(r.height);
		rectangle(img, r.tl(), r.br(), Scalar(0, 255, 0), 3);
	}
	cv::namedWindow("src", CV_WINDOW_NORMAL);
	cv::imshow("src", img);
	cv::waitKey(0);


}

void dectorOverlap(vector<Rect> &found){

	for (int i = 0; i < found.size(); ++i){
		for (int j = 0; j < found.size(); ++j){
			if (i == j)
				continue;
			if (i == found.size())
				i--;

			Rect rc1 = found[i];
			Rect rc2 = found[j];

			if (rc1.x + rc1.width  > rc2.x &&
				rc2.x + rc2.width  > rc1.x &&
				rc1.y + rc1.height > rc2.y &&
				rc2.y + rc2.height > rc1.y
				)
                found.erase(found.begin() + j);
		}
	}
}

vector<Rect> deleteFalse(vector<Rect> &found){
	vector<Rect> falseRect, falseRect1, falseRect2;
	falseRect1 = deleteFalseByX(found, deleted);
	falseRect2 = deleteFalseByY(found, deleted);
	for (int i = 0; i < falseRect1.size(); i++)falseRect.push_back(falseRect1[i]);
	for (int i = 0; i < falseRect2.size(); i++)falseRect.push_back(falseRect2[i]);
	return falseRect;
}

vector<Rect> deleteFalseByX(vector<Rect> &found)
{
	sort(found, rectCmpByX);//��x������������

	vector<pair<int, int>> groups;//����x�Ĳ�ֵ���з��� pair<position,length>

	int startpos = 0;
	int length = 1;
	for (int i = 0; i < found.size() - 1; i++)
	{
		if ((found[i + 1].x - found[i].x) <= GAP_X)//�ж�i+1�Ƿ��i��һ��
		{
			if (i == found.size() - 2)//found�����һ����֮ǰ��ͬһ��
			{
				length++;
				groups.push_back(pair<int, int>(startpos, length));
			}
			else
			{
				length++;
			}
		}
		else
		{
			groups.push_back(pair<int, int>(startpos, length));
			if (i == found.size() - 2)//found�����һ��rect��Ҫ������Ϊһ��
			{
				groups.push_back(pair<int, int>(groups.size() - 1, 1));
			}
			else
			{
				startpos = i + 1;
				length = 1;
			}
		}
	}

	sort(groups, groupsCmp);//����groups��length���н�������

	vector<Rect> trueRect;
	for (int i = groups[0].first; i < groups[0].first + groups[0].second; i++)//����found��postion��ʼ��length��Rect
	{
		trueRect.push_back(found[i]);
	}

	vector<Rect> falseRect;
	for (int i = 0; i < groups[0].first; i++)
	{
		falseRect.push_back(found[i]);
	}
	for (int i = groups[0].first + groups[0].second; i < found.size(); i++)
	{
		falseRect.push_back(found[i]);
	}

	found = trueRect;
	return falseRect;
}

vector<Rect> deleteFalseByY(vector<Rect> &found, vector<Rect> &deleted)
{
	sort(found, rectCmpByY);//��y������������

	vector<pair<int, int>> groups;//����y�Ĳ�ֵ���з��� pair<position,length>

	int startpos = 0;
	int length = 1;
	for (int i = 0; i < found.size() - 1; i++)
	{
		if ((found[i + 1].y - found[i].y) <= GAP_Y)//�ж�i+1�Ƿ��i��һ��
		{
			if (i == found.size() - 2)//found�����һ����֮ǰ��ͬһ��
			{
				length++;
				groups.push_back(pair<int, int>(startpos, length));
			}
			else
			{
				length++;
			}
		}
		else
		{
			groups.push_back(pair<int, int>(startpos, length));
			if (i == found.size() - 2)//found�����һ��rect��Ҫ������Ϊһ��
			{
				groups.push_back(pair<int, int>(groups.size() - 1, 1));
			}
			else
			{
				startpos = i + 1;
				length = 1;
			}
		}
	}

	sort(groups, groupsCmp);//����groups��length���н�������

	vector<Rect> trueRect;
	for (int i = groups[0].first; i < groups[0].first + groups[0].second; i++)//����found��postion��ʼ��length��Rect
	{
		trueRect.push_back(found[i]);
	}

	vector<Rect> falseRect;
	for (int i = 0; i < groups[0].first; i++)
	{
		falseRect.push_back(found[i]);
	}
	for (int i = groups[0].first + groups[0].second; i < found.size(); i++)
	{
		falseRect.push_back(found[i]);
	}

	found = trueRect;
	return falseRect;
}

bool rectCmpByX(Rect r1, Rect r2)
{
	return r1.x < r2.x;
}

bool rectCmpByY(Rect r1, Rect r2)
{
	return r1.y < r2.y;
}

bool groupsCmp(pair<int, int> g1, pair<int, int> g2)
{
	return g1.second > g2.second;
}