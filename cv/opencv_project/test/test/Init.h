#ifndef INIT_H
#define INIT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

const string originFile = "test_images/";
const int originStart = 1;//��1��ʼ
const int originNum = 44;//��15����
const string resultFile = "result_files/";
const double PI = 3.14159;

const bool isInit = true;//�Ƿ���Ҫ��ʼ��

//���ˮ����
bool getLine = false;
Point startPos;
Point curPos;
Point endPos;
//���ˮ��
bool getRuler = false;
int rulerIndex = 0;
Point rectStart;
Point rectCur;
Point rectEnd;

Mat image;
Mat showImage;

struct Ruler
{
	Point rulerp;//���Ͻǵ�
	int width;
	int height;
	int threshold;//��ֵs
};

//data.txt��ʽ
//index lineValid rulerCount argValid dynArgValid
//linex1 liney1 linex2 liney2 linek
//rulerx rulery width height threshold
//...
//rulerx rulery width height threshold
//L1 L2 angle
//accuracy cannyThreshold1 cannyThreshold2 houghThreshold minLineLength minLineGap
struct Data
{
	//ˮ����
	bool lineValid;//�Ƿ���ˮ������Ϣ
	Point linep1;//���Ͻǵ�
	Point linep2;//���½ǵ�
	double linek;//б��
	//����
	vector<Ruler> rulerVec;//�������
	//�̶�����
	bool argValid;//�Ƿ��в�����Ϣ
	double L1;//�ߵ��������ˮƽ����
	double L2;//�ߵ�����ˮƽ����
	double angle;//��������߶��˵ļн�
	//�ɵ�����
	bool dynArgValid;//�Ƿ��пɵ�������Ϣ
	float accuracy;//����
	double cannyT1;//canny��ֵ1
	double cannyT2;//canny��ֵ2
	double houghT;//�ж�ֱ�ߵ�������ֵ
	double minLineLength;//�߶γ�����ֵ
	double minLineGap;//�߶����������֮�����ֵ
};

void onMouseForLine(int event, int x, int y, int flags, void*);
void onMouseForRuler(int event, int x, int y, int flags, void*);

void initHelper(string originImgname, fstream& fs, int i);
void initAll();

void getDataHelper(fstream& fs, map<int, Data>& result);
map<int, Data> getData();


#endif INIT_H