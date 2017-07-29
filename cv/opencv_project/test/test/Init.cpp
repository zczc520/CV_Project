#include "Init.h"

void onMouseForLine(int event, int x, int y, int flags, void*){
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下
	{
		startPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, startPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//在窗口上显示坐标
		circle(image, startPos, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//划圆
		showImage = image;
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划直线  
	{
		Mat tmpImage;
		image.copyTo(tmpImage);
		curPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(tmpImage, temp, curPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);
		line(tmpImage, startPos, curPos, Scalar(0, 255, 0, 255), 2, 8, 0);//在临时图像上实时显示鼠标拖动时形成的直线
		showImage = tmpImage;
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开
	{
		endPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, endPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//在窗口上显示坐标
		circle(image, endPos, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//划圆
		line(image, startPos, endPos, Scalar(0, 255, 0, 255), 2, 8, 0);//直线  
		getLine = true;
		showImage = image;
	}
}
void onMouseForRuler(int event, int x, int y, int flags, void*){
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下
	{
		rectStart = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, rectStart, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//在窗口上显示坐标
		circle(image, rectStart, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//划圆
		showImage = image;
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划直线  
	{
		Mat tmpImage;
		image.copyTo(tmpImage);
		rectCur = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(tmpImage, temp, rectCur, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);
		rectangle(tmpImage, Rect(rectStart, rectCur), Scalar(0, 255, 0, 255), 2, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
		showImage = tmpImage;
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开
	{
		rectEnd = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, rectEnd, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//在窗口上显示坐标
		circle(image, rectEnd, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//划圆
		rectangle(image, Rect(rectStart, rectEnd), Scalar(0, 255, 0, 255), 2, 8, 0);//矩形  
		getRuler = true;
		rulerIndex++;
		showImage = image;
	}
}

void initHelper(string originImgname, fstream& fs, int i)
{
	bool saveLine = false;
	bool saveRuler = false;
	Mat originImage = imread(originImgname.c_str(), 1);
	originImage.copyTo(image);
	showImage = image;
	cvNamedWindow("origin", CV_WINDOW_NORMAL);
	imshow("origin", showImage);
	waitKey(1);

	cout << endl;
	cout << "\t图像：" << originImgname << endl;
	cout << "\t是否需要水岸线信息？(y/n)";
	char lineValid;
	cin >> lineValid;
	cout << "\t水尺数量：";
	int rulerCount;
	cin >> rulerCount;
	cout << "\t是否输入参数信息？(y/n)";
	char arg;
	cin >> arg;
	int argValid = (arg == 'y') ? 1 : 0;
	cout << "\t是否输入可调参数信息？(y/n)";
	char dynArg;
	cin >> dynArg;
	int dynArgValid = (dynArg == 'y') ? 1 : 0;
	//保存格式 index lineneed rulercount
	if (lineValid == 'n')fs << i << ' ' << 0 << ' ' << rulerCount << ' ' << argValid << ' ' << dynArgValid << endl;
	else fs << i << ' ' << 1 << ' ' << rulerCount << ' ' << argValid << ' ' << dynArgValid << endl;

	if (lineValid == 'y')
	{
		cout << "\t请标识水岸线！" << endl;
		while (!saveLine)
		{
			setMouseCallback("origin", onMouseForLine, 0);
			imshow("origin", showImage);
			waitKey(1);
			if (getLine)
			{
				cout << "\t直线已保存！" << endl;
				saveLine = true;
				getLine = false;
				setMouseCallback("origin", NULL, 0);
				//保存格式：左上角点x1 左上角点y1 右下角点x2 右下角点y2 斜率k
				int x1 = (startPos.x < endPos.x) ? startPos.x : endPos.x;
				int y1 = (startPos.y < endPos.y) ? startPos.y : endPos.y;
				int x2 = (startPos.x > endPos.x) ? startPos.x : endPos.x;
				int y2 = (startPos.y > endPos.y) ? startPos.y : endPos.y;
				double k = (double)(y2 - y1) / (double)(x2 - x1);
				fs << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2 << ' ' << k << endl;
			}
		}
	}//end of lineneed

	if (rulerCount != 0)
	{
		originImage.copyTo(image);
		showImage = image;
		cout << "\t对每个尺子，请先标识水尺大致位置，再标识水尺内部！" << endl;
		while (!saveRuler)
		{
			setMouseCallback("origin", onMouseForRuler, 0);
			imshow("origin", showImage);
			waitKey(1);
			if (getRuler)
			{
				//保存格式：左上角点x1 左上角点y1 宽度width 高度height 阈值threshold
				if (rulerIndex % 2 == 1)//大致位置
				{
					cout << "\t大致位置 " << (rulerIndex + 1) / 2 << " 已保存！" << endl;
					int x1 = (rectStart.x < rectEnd.x) ? rectStart.x : rectEnd.x;
					int y1 = (rectStart.y < rectEnd.y) ? rectStart.y : rectEnd.y;
					int x2 = (rectStart.x > rectEnd.x) ? rectStart.x : rectEnd.x;
					int y2 = (rectStart.y > rectEnd.y) ? rectStart.y : rectEnd.y;
					fs << x1 << ' ' << y1 << ' ' << x2 - x1 << ' ' << y2 - y1 << ' ';
				}
				else
				{
					cout << "\t水尺内部" << rulerIndex / 2 << "已保存" << endl;
					int x1 = (rectStart.x < rectEnd.x) ? rectStart.x : rectEnd.x;
					int y1 = (rectStart.y < rectEnd.y) ? rectStart.y : rectEnd.y;
					int x2 = (rectStart.x > rectEnd.x) ? rectStart.x : rectEnd.x;
					int y2 = (rectStart.y > rectEnd.y) ? rectStart.y : rectEnd.y;
					Mat temp = imread(originImgname, 0);
					Mat ruler;
					temp(Rect(Point(x1, y1), Point(x2, y2))).copyTo(ruler);
					int value = threshold(ruler, ruler, 0, 255, THRESH_OTSU);
					fs << value << endl;
				}
				getRuler = false;
				if (rulerIndex == 2 * rulerCount)
				{
					setMouseCallback("origin", NULL, 0);
					saveRuler = true;
					rulerIndex = 0;
				}
			}
		}
	}//end of rulerCount

	if (argValid == 1)
	{
		cout << "\t请输入参数！" << endl;
		cout << "\t摄像机到水尺的水平距离：";
		double L1;
		cin >> L1;
		cout << "\t水尺到岸的水平距离：";
		double L2;
		cin >> L2;
		cout << "\t摄像机与水尺顶端水平夹角(角度制)：";
		double angle;
		cin >> angle;
		fs << L1 << ' ' << L2 << ' ' << angle * 2 * PI / 360 << endl;
		cout << endl;
	}

	if (dynArgValid == 1)
	{
		cout << "\t请输入参数！" << endl;
		float accuracy;
		cout << "\t机器识别精度：";
		cin >> accuracy;
		double cannyT1;
		double cannyT2;
		cout << "\tcanny阈值1：";
		cin >> cannyT1;
		cout << "\tcanny阈值2：";
		cin >> cannyT2;
		double houghT;
		double minLineLength;
		double minLineGap;
		cout << "\t霍夫 判断直线点数的阈值：";
		cin >> houghT;
		cout << "\t霍夫 线段长度阈值：";
		cin >> minLineLength;
		cout << "\t霍夫 线段上最近两点之间的阈值：";
		cin >> minLineGap;
		fs << accuracy << ' ' << cannyT1 << ' ' << cannyT2 << ' ' << houghT << ' ' << minLineLength << ' ' << minLineGap << ' ' << endl;
	}
}
void initAll(){
	string resultFilename = resultFile + "data.txt";
	fstream fs;
	fs.open(resultFilename, ios::out);
	for (int i = originStart; i <= originNum; i++)
	{
		stringstream ss;
		string originImgname;
		ss << originFile << i << ".jpg";
		ss >> originImgname;

		initHelper(originImgname, fs, i);
	}
	fs.close();
}

void getDataHelper(fstream& fs, map<int, Data>& result)
{
	string linestr;
	while (getline(fs, linestr))
	{
		Data tempdata;

		stringstream ss(linestr);
		int index;
		int lineValid;
		int rulerCount;
		int argValid;
		int dynArgValid;
		ss >> index >> lineValid >> rulerCount >> argValid >> dynArgValid;
		if (lineValid == 1)//有水岸线信息
		{
			tempdata.lineValid = true;

			//水岸线信息
			string tempstr;
			getline(fs, tempstr);
			int x1, y1, x2, y2;
			double k;
			stringstream tempss(tempstr);
			tempss >> x1 >> y1 >> x2 >> y2 >> k;
			tempdata.linek = k;
			tempdata.linep1 = Point(x1, y1);
			tempdata.linep2 = Point(x2, y2);
		}
		else tempdata.lineValid = false;

		//尺子信息
		tempdata.rulerVec.resize(rulerCount);
		for (int i = 0; i < rulerCount; i++)
		{
			string tempstr;
			getline(fs, tempstr);
			int x, y, width, height, value;
			stringstream tempss(tempstr);
			tempss >> x >> y >> width >> height >> value;
			tempdata.rulerVec[i].rulerp = Point(x, y);
			tempdata.rulerVec[i].width = width;
			tempdata.rulerVec[i].height = height;
			tempdata.rulerVec[i].threshold = value;
		}

		if (argValid == 1)//有参数信息
		{
			tempdata.argValid = true;
			//参数信息
			string tempstr;
			getline(fs, tempstr);
			double L1, L2, angle;
			stringstream tempss(tempstr);
			tempss >> L1 >> L2 >> angle;
			tempdata.L1 = L1;
			tempdata.L2 = L2;
			tempdata.angle = angle;
		}
		else tempdata.argValid = false;

		if (dynArgValid == 1)//有可调参数信息
		{
			tempdata.dynArgValid = true;
			//可调参数信息
			string tempstr;
			getline(fs, tempstr);
			double accuracy, cannyT1, cannyT2, houghT, minLineLength, minLineGap;
			stringstream tempss(tempstr);
			tempss >> accuracy >> cannyT1 >> cannyT2 >> houghT >> minLineLength >> minLineGap;
			tempdata.accuracy = accuracy;
			tempdata.cannyT1 = cannyT1; tempdata.cannyT2 = cannyT2;
			tempdata.houghT = houghT; tempdata.minLineLength = minLineLength;
			tempdata.minLineGap = minLineGap;
		}
		else tempdata.dynArgValid = false;

		result.insert(map<int, Data>::value_type(index, tempdata));
	}//end of while
}
map<int, Data> getData()
{
	map<int, Data> result;

	string resultFilename = resultFile + "data.txt";
	fstream fs;
	fs.open(resultFilename, ios::in);
	getDataHelper(fs, result);
	fs.close();

	return result;
}

int main()
{
	if (isInit)
	{
		initAll();
	}
	map<int, Data> result = getData();
}