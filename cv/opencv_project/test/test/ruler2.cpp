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

const int EDGE_LENGTH = 10;//�߽糤�ȣ����ڶ�λ���±߽�
const double EDGE_FACTOR = 0.1;//�߽����ӣ����ڶ�λ���ұ߽�
const int EDGE_LRGAP = 25;//�߽��������ڶ�λ���ұ߽�
const int EDGE_UBGAP = 50;//�߽��������ڶ�λ���±߽�
const double CENTER_PROPORTION = 0.4;//����ռ��
const int ROTATE_90 = 1;
const int ROTATE_180 = -1;
const int ROTATE_270 = 0;
const int MAX_GRAY = 255;

/**************** basic function ***************/

void traverse_mat(Mat& mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			mat.at<uchar>(i, j) = MAX_GRAY - mat.at<uchar>(i, j);
		}
	}
}

Mat rotate_mat(Mat& mat, int flag)
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

int my_compare(pair<int,int> a, pair<int,int> b)
{
	return a.second > b.second;//��������
}

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

void removebg(Mat& mat)
{
	//ȥ��ͼƬ��Ե�ĸ���(�������Ҹ�10%)
	int above = mat.rows*0.1;
	int bottom = mat.rows*0.9;
	int left = mat.cols*0.1;
	int right = mat.cols*0.9;
	for (int i = 0; i < above; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			mat.at<uchar>(i, j) = 0;
		}
	}
	for (int i = bottom; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			mat.at<uchar>(i, j) = 0;
		}
	}
	for (int j = 0; j < left; j++)
	{
		for (int i = 0; i < mat.rows; i++)
		{
			mat.at<uchar>(i, j) = 0;
		}
	}
	for (int j = right; j < mat.cols; j++)
	{
		for (int i = 0; i < mat.rows; i++)
		{
			mat.at<uchar>(i, j) = 0;
		}
	}

	//ֱ��ͼ���⻯
	equalizeHist(mat, mat);
	//��ֵ�˲�
	medianBlur(mat, mat, 5);
	//������
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));
	Mat dst;
	morphologyEx(mat, dst, MORPH_OPEN, element);
	//ȥ������
	mat = mat - dst;
}

void locate_ruler(Mat mat, int& up, int& bottom, int& left, int&right)
{
	//����ÿһ�еĻҶȺ�,first��ʾ�кţ�second��ʾ�ҶȺ�
	vector<pair<int, int>> col_sum;
	//��ʼ��col_sum
	for (int j = 0; j < mat.cols; j++)
	{
		col_sum.push_back(pair<int, int>(j, 0));
	}

	//����col_sum
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			if (mat.at<uchar>(i, j) == MAX_GRAY)
				col_sum[j].second++;
		}
	}

	//���ҶȺͽ�������
	sort(col_sum.begin(), col_sum.end(), my_compare);

	//����edge factorѡ��ҶȺͽϴ���к�
	vector<int> larger_col;
	int border = col_sum.size()*EDGE_FACTOR;
	for (int i = 0; i < border; i++)
	{
		larger_col.push_back(col_sum[i].first);
	}

	//����center proportionѡ����Ч�к�
	vector<int> valid_col;
	double low_proportion = 0.5*(1 - CENTER_PROPORTION);
	double high_proportion = low_proportion + CENTER_PROPORTION;
	int low = mat.cols*low_proportion;
	int high = mat.cols*high_proportion;
	for (int i = 0; i < larger_col.size(); i++)
	{
		if (larger_col[i] >= low && larger_col[i] <= high)
		{
			valid_col.push_back(larger_col[i]);
		}
	}
	//����(Ĭ��)����ȷ�����ұ߽�
	sort(valid_col.begin(), valid_col.end());

	//��λ��
	int mid_col_index = valid_col.size() / 2;
	//��λ�ұ߽�
	right = valid_col[valid_col.size() - 1];
	for (int i = mid_col_index; i < valid_col.size(); i++)
	{
		if ((valid_col[i] - valid_col[i - 1]) >= EDGE_LRGAP)
		{
			right = valid_col[i - 1];
			break;
		}
	}
	//��λ��߽�
	left = valid_col[0];
	for (int i = mid_col_index; i > 0; i--)
	{
		if ((valid_col[i] - valid_col[i - 1]) >= EDGE_LRGAP)
		{
			left = valid_col[i];
			break;
		}
	}
	
	//�лҶȺʹ��ڵ���EDGE_LENGTH���к�
	vector<int> larger_row;
	for (int i = 0; i < mat.rows; i++)
	{
		int row_sum = 0;
		for (int j = left; j <= right; j++)
		{
			if (mat.at<uchar>(i, j) == MAX_GRAY)
			{
				row_sum++;
			}
		}
		if (row_sum >= EDGE_LENGTH)
		{
			larger_row.push_back(i);
		}
	}
	
	//��λ��
	int mid_row_index = larger_row.size() / 2;
	//��λ�ϱ߽�
	up = larger_row[0];
	for (int i = mid_row_index; i > 0; i--)
	{
		if ((larger_row[i] - larger_row[i - 1]) >= EDGE_UBGAP)
		{
			up = larger_row[i];
			break;
		}
	}
	//��λ�±߽�
	bottom = larger_row[larger_row.size() - 1];
	for (int i = mid_row_index; i < larger_row.size(); i++)
	{
		if ((larger_row[i] - larger_row[i - 1]) >= EDGE_UBGAP)
		{
			bottom = larger_row[i - 1];
			break;
		}
	}
}

/**************** operation function ***************/

int get_elines(string pathname) {
	//Mat mat = imread(pathname, 1);
	//��RGBͼ����о��⻯
	//vector<Mat> splitmat;
	//split(mat, splitmat);
	//for (int i = 0; i < mat.channels(); i++)
	//{
	//	equalizeHist(splitmat[i], splitmat[i]);
	//}
	//merge(splitmat, mat);
	////Threshold the image
	//Mat ruler;
	//inRange(mat, Scalar(0, 0, 0), Scalar(105, 28, 25), ruler); 

	Mat ruler = imread(pathname, 0);

	int check_line1 = (ruler.cols) * 2 / 3;
	int check_line2 = (ruler.cols) * 4 / 5;

	bool flag_line = false;//false means black
	int step = (check_line2 - check_line1) / 7 + 1;
	vector<int>line_number;
	for (int j = check_line1; j <= check_line2; j = j + step) {
		int count_eline = 0;
		for (int i = 0; i < ruler.rows; i++)
		{
			if ((!flag_line) && ruler.at<uchar>(i, j) == 255) {
				count_eline++;
				flag_line = true;
			}
			else if (flag_line && ruler.at<uchar>(i, j) == 0) {
				//if (ruler.at<uchar>(i - 2, j) == 0)continue;
				flag_line = false;
			}
		}
		line_number.push_back(count_eline);
	}
	sort(line_number.begin(), line_number.end());
	cout << "the line in the right is:" << line_number[line_number.size() / 2] << endl;
	return line_number[line_number.size() / 2];
}

string morphology_process(string pathname)
{
	//�Ҷ�ͼ
	Mat mat = imread(pathname, 0);
	//ȥ����������
	removebg(mat);
	
	//��ʴ����
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	erode(mat, mat, element);
	//ͼ���ֵ��
	threshold(mat, mat, 0, MAX_GRAY, CV_THRESH_OTSU);
	//���θ�ʴ����
	//erode(mat, mat, element);
	erode(mat, mat, element);
	//Canny��Ե���
	Canny(mat, mat, 11, 33);
	//�ղ���
	element = getStructuringElement(MORPH_RECT, Size(19, 19));
	morphologyEx(mat, mat, MORPH_CLOSE, element);

	/*namedWindow("aa", WINDOW_AUTOSIZE);
	imshow("aa", mat);
	waitKey(0);*/

	//��λruler
	int up, bottom, left, right;
	locate_ruler(mat, up, bottom, left, right);
	up -= 10;
	bottom += 10;
	left -= 10;
	//right += 30;

	
	//����ͼƬ
	Mat origin_img = imread(pathname, 1);
	vector<Mat> origin_img_rgb;
	Mat ruler;
	vector<Mat> ruler_rgb;
	//����ruler
	int height = bottom - up + 1;//bottom&up : 0 - rows-1//[up,bottom]
	int width = right - left + 1;//left&right: 0 - cols-1//[left,right]
	split(origin_img, origin_img_rgb);
	for (int index = 0; index < origin_img.channels(); index++)
	{
		Mat ruler_single_temp(height, width, CV_8U);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				ruler_single_temp.at<uchar>(i, j) = origin_img_rgb[index].at<uchar>(i + up, j + left);
			}
		}
		ruler_rgb.push_back(ruler_single_temp);
	}
	merge(ruler_rgb, ruler);
	//int height = bottom - up + 1;//bottom&up : 0 - rows-1//[up,bottom]
	//int width = right - left + 1;//left&right: 0 - cols-1//[left,right]
	//Mat ruler(height, width, CV_8U);
	//for (int i = 0; i < height; i++)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		ruler.at<uchar>(i, j) = mat.at<uchar>(i + up, j + left);
	//	}
	//}

	//����ruler��ͼƬΪfirstName_ruler.secondName
	string ruler_name = get_first_name(pathname) + "_ruler." + get_second_name(pathname);
	//ͼ���ֵ��
	//threshold(ruler, ruler, 0, MAX_GRAY, CV_THRESH_OTSU);
	//�Ҷ�ȡ��
	//traverse_mat(ruler);
	imwrite(ruler_name, ruler);

	return ruler_name;
}

void color_process(string pathname)
{
	namedWindow("Control", CV_WINDOW_NORMAL); //create a window called "Control"

	int iLowR = 0;
	int iHighR = 105;
	int iLowG = 0;
	int iHighG = 28;
	int iLowB = 0;
	int iHighB = 25;

	cvCreateTrackbar("LowR", "Control", &iLowR, 255); //Red (0 - 255)
	cvCreateTrackbar("HighR", "Control", &iHighR, 255);
	cvCreateTrackbar("LowG", "Control", &iLowG, 255); //Green (0 - 255)
	cvCreateTrackbar("HighG", "Control", &iHighG, 255);
	cvCreateTrackbar("LowB", "Control", &iLowB, 255); //Blue (0 - 255)
	cvCreateTrackbar("HighB", "Control", &iHighB, 255);

	Mat mat = imread(pathname,1);
	//��RGBͼ����о��⻯
	vector<Mat> splitmat;
	split(mat, splitmat);
	for (int i = 0; i < mat.channels(); i++)
	{
		equalizeHist(splitmat[i], splitmat[i]);
	}
	merge(splitmat, mat);

	while (true)
	{
		//ʶ�������ͼ��
		Mat thresholdmat;
		inRange(mat, Scalar(iLowR, iLowG, iLowB), Scalar(iHighR, iHighG, iHighB), thresholdmat); //Threshold the image
		//������ (ȥ��һЩ���)
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(thresholdmat, thresholdmat, MORPH_OPEN, element);
		//�ղ��� (����һЩ��ͨ��)
		morphologyEx(thresholdmat, thresholdmat, MORPH_CLOSE, element);

		namedWindow("Thresholded Image before", CV_WINDOW_NORMAL); 
		imshow("Thresholded Image before", thresholdmat); //show the thresholded image

		//�����������ĸ����
		removebg(thresholdmat);	

		namedWindow("Thresholded Image after", CV_WINDOW_NORMAL);
		imshow("Thresholded Image after", thresholdmat); //show the thresholded image
		namedWindow("Original", CV_WINDOW_NORMAL);
		imshow("Original", mat); //show the original image

		waitKey(100);
	}
}

void ruler_indentify(string pathname)
{
	string first_name = get_first_name(pathname);
	string second_name = get_second_name(pathname);

	string ruler_name = morphology_process(pathname);
	int lines = get_elines(ruler_name);
	cout << "elies: " << lines << endl;
	//color_process(pathname);
}

/**************** main function ***************/

int main()
{
	string pathname = "11.jpg";
	ruler_indentify(pathname);

	/*Mat a = imread("a.jpg", 0);
	threshold(a, a, 0, 255, THRESH_OTSU);
	traverse_mat(a);
	imwrite("aa.png", a);*/

	system("pause");
	return 0;
}