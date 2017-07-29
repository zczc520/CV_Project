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
const int originStart = 1;//从1开始
const int originNum = 44;//到15结束
const string resultFile = "result_files/";
const double PI = 3.14159;

const bool isInit = true;//是否需要初始化

//获得水岸线
bool getLine = false;
Point startPos;
Point curPos;
Point endPos;
//获得水尺
bool getRuler = false;
int rulerIndex = 0;
Point rectStart;
Point rectCur;
Point rectEnd;

Mat image;
Mat showImage;

struct Ruler
{
	Point rulerp;//左上角点
	int width;
	int height;
	int threshold;//阈值s
};

//data.txt格式
//index lineValid rulerCount argValid dynArgValid
//linex1 liney1 linex2 liney2 linek
//rulerx rulery width height threshold
//...
//rulerx rulery width height threshold
//L1 L2 angle
//accuracy cannyThreshold1 cannyThreshold2 houghThreshold minLineLength minLineGap
struct Data
{
	//水岸线
	bool lineValid;//是否含有水岸线信息
	Point linep1;//左上角点
	Point linep2;//右下角点
	double linek;//斜率
	//尺子
	vector<Ruler> rulerVec;//多个尺子
	//固定参数
	bool argValid;//是否含有参数信息
	double L1;//尺到摄像机的水平距离
	double L2;//尺到岸的水平距离
	double angle;//摄像机到尺顶端的夹角
	//可调参数
	bool dynArgValid;//是否含有可调参数信息
	float accuracy;//精度
	double cannyT1;//canny阈值1
	double cannyT2;//canny阈值2
	double houghT;//判断直线点数的阈值
	double minLineLength;//线段长度阈值
	double minLineGap;//线段上最近两点之间的阈值
};

void onMouseForLine(int event, int x, int y, int flags, void*);
void onMouseForRuler(int event, int x, int y, int flags, void*);

void initHelper(string originImgname, fstream& fs, int i);
void initAll();

void getDataHelper(fstream& fs, map<int, Data>& result);
map<int, Data> getData();


#endif INIT_H