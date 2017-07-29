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

class Mysvm : public CvSVM
{
public:
	int get_alpha_count(){
		return this->sv_total;
	}

	int get_sv_dim(){
		return this->var_all;
	}

	int get_sv_count(){
		return this->decision_func->sv_count;
	}

	double* get_alpha(){
		return this->decision_func->alpha;
	}

	float** get_sv(){
		return this->sv;
	}

	float get_rho(){
		return this->decision_func->rho;
	}
};

using namespace std;
using namespace cv;

/* ѵ�����ݵ�����Ŀ */
const int training_amounts = 5415;
const int correctE_amounts = 1824;
const int incorrectE_amount = 3591;
const string classifierSavePath = "dector.txt";
const int feature_size = 3780;

int main()
{
	/* initialize mat */
	CvMat *sampleLabelMat = cvCreateMat(training_amounts, 1, CV_32FC1);//������ʶ 
	CvMat *sampleFeaturesMat = cvCreateMat(training_amounts, feature_size, CV_32FC1);
	cvSetZero(sampleFeaturesMat);
	cvSetZero(sampleLabelMat);

	/* read true training pictures */
	std::cout << "start reading correct pictures. " << endl;
	for (int i = 0; i < correctE_amounts; ++i) {
		/* get training picture name */
		string picture_name;
		stringstream stream;
		stream << i;
		stream >> picture_name;
		picture_name = "training_true/" + picture_name + ".png";
		//picture_name = "measure_imgs/" + picture_name + ".jpg";

		/* read in training picture and do resize */
		Mat img = imread(picture_name);
		resize(img, img, cv::Size(64, 128));

		/* compute HOG features */
		HOGDescriptor hog(cv::Size(64, 128), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
		//HOGDescriptor hog(cvSize(3, 3), cvSize(3, 3), cvSize(5, 10), cvSize(3, 3), 9);
		vector<float> feature_vec;
		hog.compute(img, feature_vec, cv::Size(8, 8));

		for (int j = 0; j < feature_vec.size(); j++){
			CV_MAT_ELEM(*sampleFeaturesMat, float, i, j) = feature_vec[j];
		}

		/* update labels */
		sampleLabelMat->data.fl[i] = 1.0;
	}
	std::cout << "finish reading correct pictures. " << endl;

	/* read false training pictures */
	std::cout << "start reading incorrect pictures. " << endl;
	for (int i = 1824; i < incorrectE_amount + 1824; ++i) {
		/* get training picture name */
		int x = i - 1823;

		string picture_name;
		stringstream stream;
		stream << x;
		stream >> picture_name;
		picture_name = "training_false/wrong ("  + picture_name + ").jpg";

		/* read in training picture and resize */
		Mat img = imread(picture_name);
		resize(img, img, cv::Size(64, 128));
		
		/* compute HOG features */
		HOGDescriptor hog(cv::Size(64, 128), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
		//HOGDescriptor hog(cvSize(3, 3), cvSize(3, 3), cvSize(5, 10), cvSize(3, 3), 9);
		vector<float> feature_vec;
		hog.compute(img, feature_vec, cv::Size(8, 8));

		for (int j = 0; j < feature_vec.size(); j++){
			CV_MAT_ELEM(*sampleFeaturesMat, float, i, j) = feature_vec[j];
		}

		/* update labels */
		sampleLabelMat->data.fl[i] = -1.0;
	}
	std::cout << "finish reading incorrect pictures. " << endl;

	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON);
	params.C = 0.01;

	Mysvm svm;
	svm.train(sampleFeaturesMat, sampleLabelMat, Mat(), Mat(), params); //��SVM���Է�����ѵ��  
	svm.save(classifierSavePath.c_str());

	cvReleaseMat(&sampleFeaturesMat);
	cvReleaseMat(&sampleLabelMat);

	/* ��ѵ���õ�svmת��Ϊ������� */
	int DescriptorDim = svm.get_var_count();//����������ά������HOG�����ӵ�ά��  
	int supportVectorNum = svm.get_support_vector_count();//֧�������ĸ���  
	std::cout << "֧������������" << supportVectorNum << endl;

	Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);//alpha���������ȵ���֧����������  
	Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);//֧����������  
	Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);//alpha��������֧����������Ľ��  

	//��֧�����������ݸ��Ƶ�supportVectorMat������  
	for (int i = 0; i<supportVectorNum; i++){
		const float * pSVData = svm.get_support_vector(i);//���ص�i��֧������������ָ��  
		for (int j = 0; j<DescriptorDim; j++){
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}

	//��alpha���������ݸ��Ƶ�alphaMat��  
	double * pAlphaData = svm.get_alpha();//����SVM�ľ��ߺ����е�alpha����  
	for (int i = 0; i<supportVectorNum; i++){
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}

	//����-(alphaMat * supportVectorMat),����ŵ�resultMat��  
	//gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//��֪��Ϊʲô�Ӹ��ţ�  
	resultMat = -1 * alphaMat * supportVectorMat;

	//�õ����յ�setSVMDetector(const vector<float>& detector)�����п��õļ����  
	vector<float> myDetector;
	//��resultMat�е����ݸ��Ƶ�����myDetector��  
	for (int i = 0; i<DescriptorDim; i++)
	{
		myDetector.push_back(resultMat.at<float>(0, i));
	}
	//������ƫ����rho���õ������  
	myDetector.push_back(svm.get_rho());
	std::cout << "�����ά����" << myDetector.size() << endl;

	

	/* ����ʶ�� */
	Mat img = imread("test.png");
	vector<cv::Rect>  found;
	//HOGDescriptor hog(cvSize(3, 3), cvSize(3, 3), cvSize(5, 10), cvSize(3, 3), 9);
	HOGDescriptor hog;
	
	hog.setSVMDetector(myDetector);
	hog.detectMultiScale(img, found, 0, cv::Size(16, 16), cv::Size(16, 16), 1.05, 2);
	for (int i = 0; i<found.size(); i++){
		Rect r = found[i];

		r.x += cvRound(r.width);
		r.width = cvRound(r.width);
		r.y += cvRound(r.height);
		r.height = cvRound(r.height);
		rectangle(img, r.tl(), r.br(), Scalar(0, 255, 0), 3);
	}

	cv::imshow("src", img);
	cv::waitKey(0);

	//int total = 0;
	//for (int p = 235; p < 235+incorrectE_amount; ++p){
	//	string picture_name;
	//	stringstream stream;
	//	stream << p;
	//	stream >> picture_name;
	//	picture_name = "training_false/" + picture_name + ".png";
	//	Mat img = imread(picture_name);
	//	resize(img, img, cv::Size(64, 128));

	//	/* compute HOG features */
	//	HOGDescriptor hog(cv::Size(64, 128), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	//	//HOGDescriptor hog(cvSize(3, 3), cvSize(3, 3), cvSize(5, 10), cvSize(3, 3), 9);
	//	vector<float> feature_vec;
	//	hog.compute(img, feature_vec, cv::Size(8, 8));

	//	int* arr = new int[feature_vec.size()];
	//	for (int x = 0; x < feature_vec.size(); ++x)
	//		arr[x] = feature_vec[x];

	//	/* �����������Mat */
	//	CvMat aim_mat;
	//	cvInitMatHeader(&aim_mat, 1, feature_vec.size(), CV_32FC1, arr);
	//	for (int n = 0; n < feature_vec.size(); ++n){
	//		cvmSet(&aim_mat, 0, n, feature_vec[n]);
	//	}

	//	/* ʹ��ģ�ͽ��з��� */
	//	float response = svm.predict(&aim_mat);
	//	if (response == -1.0)
	//		total++;

	//}
	//cout << total << endl;
	//system("pause");

}