#include "Init.h"

void onMouseForLine(int event, int x, int y, int flags, void*){
	if (event == CV_EVENT_LBUTTONDOWN)//�������
	{
		startPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, startPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//�ڴ�������ʾ����
		circle(image, startPos, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//��Բ
		showImage = image;
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�ֱ��  
	{
		Mat tmpImage;
		image.copyTo(tmpImage);
		curPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(tmpImage, temp, curPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);
		line(tmpImage, startPos, curPos, Scalar(0, 255, 0, 255), 2, 8, 0);//����ʱͼ����ʵʱ��ʾ����϶�ʱ�γɵ�ֱ��
		showImage = tmpImage;
	}
	else if (event == CV_EVENT_LBUTTONUP)//����ɿ�
	{
		endPos = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, endPos, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//�ڴ�������ʾ����
		circle(image, endPos, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//��Բ
		line(image, startPos, endPos, Scalar(0, 255, 0, 255), 2, 8, 0);//ֱ��  
		getLine = true;
		showImage = image;
	}
}
void onMouseForRuler(int event, int x, int y, int flags, void*){
	if (event == CV_EVENT_LBUTTONDOWN)//�������
	{
		rectStart = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, rectStart, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//�ڴ�������ʾ����
		circle(image, rectStart, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//��Բ
		showImage = image;
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�ֱ��  
	{
		Mat tmpImage;
		image.copyTo(tmpImage);
		rectCur = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(tmpImage, temp, rectCur, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);
		rectangle(tmpImage, Rect(rectStart, rectCur), Scalar(0, 255, 0, 255), 2, 8, 0);//����ʱͼ����ʵʱ��ʾ����϶�ʱ�γɵľ���  
		showImage = tmpImage;
	}
	else if (event == CV_EVENT_LBUTTONUP)//����ɿ�
	{
		rectEnd = Point(x, y);
		stringstream ss;
		string temp;
		ss << '(' << x << ',' << y << ')';
		ss >> temp;
		putText(image, temp, rectEnd, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255), 4, 8);//�ڴ�������ʾ����
		circle(image, rectEnd, 4, Scalar(0, 255, 0, 255), CV_FILLED, CV_AA, 0);//��Բ
		rectangle(image, Rect(rectStart, rectEnd), Scalar(0, 255, 0, 255), 2, 8, 0);//����  
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
	cout << "\tͼ��" << originImgname << endl;
	cout << "\t�Ƿ���Ҫˮ������Ϣ��(y/n)";
	char lineValid;
	cin >> lineValid;
	cout << "\tˮ��������";
	int rulerCount;
	cin >> rulerCount;
	cout << "\t�Ƿ����������Ϣ��(y/n)";
	char arg;
	cin >> arg;
	int argValid = (arg == 'y') ? 1 : 0;
	cout << "\t�Ƿ�����ɵ�������Ϣ��(y/n)";
	char dynArg;
	cin >> dynArg;
	int dynArgValid = (dynArg == 'y') ? 1 : 0;
	//�����ʽ index lineneed rulercount
	if (lineValid == 'n')fs << i << ' ' << 0 << ' ' << rulerCount << ' ' << argValid << ' ' << dynArgValid << endl;
	else fs << i << ' ' << 1 << ' ' << rulerCount << ' ' << argValid << ' ' << dynArgValid << endl;

	if (lineValid == 'y')
	{
		cout << "\t���ʶˮ���ߣ�" << endl;
		while (!saveLine)
		{
			setMouseCallback("origin", onMouseForLine, 0);
			imshow("origin", showImage);
			waitKey(1);
			if (getLine)
			{
				cout << "\tֱ���ѱ��棡" << endl;
				saveLine = true;
				getLine = false;
				setMouseCallback("origin", NULL, 0);
				//�����ʽ�����Ͻǵ�x1 ���Ͻǵ�y1 ���½ǵ�x2 ���½ǵ�y2 б��k
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
		cout << "\t��ÿ�����ӣ����ȱ�ʶˮ�ߴ���λ�ã��ٱ�ʶˮ���ڲ���" << endl;
		while (!saveRuler)
		{
			setMouseCallback("origin", onMouseForRuler, 0);
			imshow("origin", showImage);
			waitKey(1);
			if (getRuler)
			{
				//�����ʽ�����Ͻǵ�x1 ���Ͻǵ�y1 ���width �߶�height ��ֵthreshold
				if (rulerIndex % 2 == 1)//����λ��
				{
					cout << "\t����λ�� " << (rulerIndex + 1) / 2 << " �ѱ��棡" << endl;
					int x1 = (rectStart.x < rectEnd.x) ? rectStart.x : rectEnd.x;
					int y1 = (rectStart.y < rectEnd.y) ? rectStart.y : rectEnd.y;
					int x2 = (rectStart.x > rectEnd.x) ? rectStart.x : rectEnd.x;
					int y2 = (rectStart.y > rectEnd.y) ? rectStart.y : rectEnd.y;
					fs << x1 << ' ' << y1 << ' ' << x2 - x1 << ' ' << y2 - y1 << ' ';
				}
				else
				{
					cout << "\tˮ���ڲ�" << rulerIndex / 2 << "�ѱ���" << endl;
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
		cout << "\t�����������" << endl;
		cout << "\t�������ˮ�ߵ�ˮƽ���룺";
		double L1;
		cin >> L1;
		cout << "\tˮ�ߵ�����ˮƽ���룺";
		double L2;
		cin >> L2;
		cout << "\t�������ˮ�߶���ˮƽ�н�(�Ƕ���)��";
		double angle;
		cin >> angle;
		fs << L1 << ' ' << L2 << ' ' << angle * 2 * PI / 360 << endl;
		cout << endl;
	}

	if (dynArgValid == 1)
	{
		cout << "\t�����������" << endl;
		float accuracy;
		cout << "\t����ʶ�𾫶ȣ�";
		cin >> accuracy;
		double cannyT1;
		double cannyT2;
		cout << "\tcanny��ֵ1��";
		cin >> cannyT1;
		cout << "\tcanny��ֵ2��";
		cin >> cannyT2;
		double houghT;
		double minLineLength;
		double minLineGap;
		cout << "\t���� �ж�ֱ�ߵ�������ֵ��";
		cin >> houghT;
		cout << "\t���� �߶γ�����ֵ��";
		cin >> minLineLength;
		cout << "\t���� �߶����������֮�����ֵ��";
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
		if (lineValid == 1)//��ˮ������Ϣ
		{
			tempdata.lineValid = true;

			//ˮ������Ϣ
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

		//������Ϣ
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

		if (argValid == 1)//�в�����Ϣ
		{
			tempdata.argValid = true;
			//������Ϣ
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

		if (dynArgValid == 1)//�пɵ�������Ϣ
		{
			tempdata.dynArgValid = true;
			//�ɵ�������Ϣ
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