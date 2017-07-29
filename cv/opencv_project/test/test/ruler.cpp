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

/**************** const variable ***************/

const int EDGE_UP = 0;
const int EDGE_BOTTOM = 1;
const int EDGE_LEFT = 2;
const int EDGE_RIGHT = 3;
const int ROTATE_90 = 1;
const int ROTATE_180 = -1;
const int ROTATE_270 = 0;

/**************** help function ***************/

string get_first_name(string pathname)
{
	int position = pathname.find_first_of('.');
	return pathname.substr(0, position);
}

string get_second_name(string pathname)
{
	int position = pathname.find_first_of('.');
	return pathname.substr(position + 1);
}

void traverse_mat(Mat& mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			mat.at<uchar>(i, j) = 255 - mat.at<uchar>(i, j);
		}
	}
}

int locate_edge(Mat& mat, int valid_length,int flag)
{
	int position;

	if (flag == EDGE_UP)//up
	{
		for (position = 0; position < mat.rows - 1; position++)
		{
			int length = 0;
			for (int j = 0; j < mat.cols; j++)
			{
				if (mat.at<uchar>(position, j) == 255)length++;
			}
			if (length >= valid_length)break;
		}
	}
	else if (flag == EDGE_BOTTOM)//bottom
	{
		for (position = mat.rows-1; position >= 0; position--)
		{
			int length = 0;
			for (int j = 0; j < mat.cols; j++)
			{
				if (mat.at<uchar>(position, j) == 255)length++;
			}
			if (length >= valid_length)break;
		}
	}
	else if (flag == EDGE_LEFT)//left
	{
		for (position = 0; position < mat.cols; position++)
		{
			int length = 0;
			for (int i = 0; i < mat.rows; i++)
			{
				if (mat.at<uchar>(i, position) == 255)length++;
			}
			if (length >= valid_length)break;
		}
	}
	else if (flag == EDGE_RIGHT)//right
	{
		for (position = mat.cols - 1; position >= 0; position--)
		{
			int length = 0;
			for (int i = 0; i < mat.rows; i++)
			{
				if (mat.at<uchar>(i, position) == 255)length++;
			}
			if (length >= valid_length)break;
		}
	}
	else
	{
		cout << "[locate_edge]error:invalid flag!" << endl;
		exit(-1);
	}

	return position;
}

Mat rotate_mat(Mat& mat,int flag)
{
	Mat matTranspose(mat.cols, mat.rows, CV_8U);
	transpose(mat, matTranspose);
	if (flag == 1)//˳ʱ��90��
	{
		Mat result(mat.cols, mat.rows, CV_8U);
		flip(matTranspose, result, flag);
		return result;
	}
	else if (flag == -1)//˳ʱ��180��
	{
		Mat result(mat.rows, mat.cols, CV_8U);
		flip(matTranspose, result, flag);
		return result;
	}
	else if (flag == 0)//˳ʱ��270��
	{
		Mat result(mat.cols, mat.rows, CV_8U);
		flip(matTranspose, result, flag);
		return result;
	}
	else
	{
		cout << "[rotate_mat]error:invalid flag!" << endl;
		exit(-1);
	}
}

/**************** operation function ***************/

void init_process(Mat& mat)
{
	//ֱ��ͼ���⻯
	equalizeHist(mat, mat);
	//��ֵ�˲�
	medianBlur(mat, mat, 5);
}

string locate_ruler(const Mat& mat, string firstName, string secondName)
{
	//������
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(13, 13));
	Mat dst;
	morphologyEx(mat, dst, MORPH_OPEN, element);
	//ȥ������
	dst = mat - dst;
	//��ʴ����
	element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	//erode(dst, dst, element);
	morphologyEx(dst, dst, MORPH_OPEN, element);
	element = getStructuringElement(MORPH_ELLIPSE, Size(13, 13));
	morphologyEx(dst, dst, MORPH_CLOSE, element);
	//ͼ���ֵ��
	threshold(dst,dst, 0, 255, CV_THRESH_OTSU);

	namedWindow("ruler", WINDOW_NORMAL);
	imshow("ruler", dst);
	waitKey(0);

	//Canny��Ե���
	Canny(dst, dst, 11, 33);
	//�ղ���
	element = getStructuringElement(MORPH_RECT, Size(61, 61));
	morphologyEx(dst, dst, MORPH_CLOSE, element);
	//��ʴ����
	element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(dst, dst, element);

	//��λruler
	int valid_length = 10;//��Ч����
	int up = 0, bottom = dst.rows - 1, left = 0, right = dst.cols - 1;
	up = locate_edge(dst, valid_length, EDGE_UP);//��λup
	bottom = locate_edge(dst, valid_length, EDGE_BOTTOM);//��λbottom
	left = locate_edge(dst, valid_length, EDGE_LEFT);//��λleft
	right = locate_edge(dst, valid_length, EDGE_RIGHT);//��λright
	int width = right - left;
	int height = bottom - up;
	if (width <= 0 || height <= 0)
	{
		cout << "[locate_ruler]error:width or height is non-positive!" << endl;
		exit(-1);
	}

	//����ruler,����ΪfirstName_ruler.secondName
	Mat ruler(height, width, CV_8U);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ruler.at<uchar>(i, j) = mat.at<uchar>(i + up, j + left);
		}
	}
	string rulerName = firstName + "_ruler." + secondName;
	//ͼ���ֵ��
	threshold(ruler, ruler, 0, 255, CV_THRESH_OTSU);
	//�Ҷ�ȡ��
	traverse_mat(ruler);
	imwrite(rulerName, ruler);

	return rulerName;
}

Mat read_ruler(string rulerName)
{
	Mat ruler = imread(rulerName, 0);
	if (ruler.rows > ruler.cols)return ruler;
	else
	{
		Mat upright_ruler = rotate_mat(ruler, ROTATE_270);
		return upright_ruler;
	}
}

void ruler_indentify(string pathname)
{
	string first_name = get_first_name(pathname);
	string second_name = get_second_name(pathname);

	Mat origin_image = imread(pathname, 0);
	//Ԥ����
	init_process(origin_image);
	//��λruler
	string ruler_name = locate_ruler(origin_image, first_name, second_name);
	//��ȡruler(����ͼ)
	Mat ruler = read_ruler(ruler_name);
}

int main()
{
	string pathname = "waterdepth2.jpg";
	ruler_indentify(pathname);

	system("pause");
	return 0;
}