#include "Recognise.h"
#include <vector>
#include <numeric>
#include <iostream>
#include "feature.h"
/* Custom Color */
const Scalar BLUE  = Scalar(255, 0, 0);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar RED   = Scalar(0, 0, 255);
const Scalar WHITE = Scalar(255, 255, 255);

#define Black_Plate 1
#define White_Plate 2
#define Yellow_Plate 3

LPR_Recognise::LPR_Recognise(){}

//Function for average
void find_Average( vector<Rect> v,double *avg_H, double *avg_W, double *avg_X){
	vector<int> v_H,v_W,v_Y;
	for(int i =0;i< v.size();i++)
	v_H.push_back(v.at(i).height);
	sort(v_H.begin(), v_H.end());

	for(int i =0;i< v.size();i++)
	v_W.push_back(v.at(i).width);
	sort(v_W.begin(), v_W.end());

	for(int i =0;i< v.size();i++)
	v_Y.push_back(v.at(i).y);
	sort(v_Y.begin(), v_Y.end());

	*avg_H = accumulate( v_H.begin()+2, v_H.end()-2, 0.0)/(v_H.size()-4);  
	*avg_W = accumulate( v_W.begin()+2, v_W.end()-2, 0.0)/(v_W.size()-4);    
	*avg_X = accumulate( v_Y.begin()+2, v_Y.end()-2, 0.0)/(v_Y.size()-4);             
}

 int idx;
int is_character(Mat binary_image,vector<Rect> &r_characters){
	// Find the contours
	vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binary_image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	if( contours.size()==0)
		return 1;
	for (int i = 0; i < contours.size(); i++) 
	{
		
		Rect r = boundingRect(contours[i]);
		double ratio;
		 ratio = (double) r.height/r.width;
		int size = r.width * r.height;
		if((r.width<5) || (r.height<10)) continue;
		if((ratio>0.8) && (ratio < 5.7)){
			if(ratio>3.7){if(r.width>r.height/3) continue;} // check number 1 
		 if( r.height>(binary_image.size().height/ 4)) {
			if( size > ((binary_image.size().height*binary_image.size().width)/80)
				&& size < ((binary_image.size().height*binary_image.size().width)/5))
				{ 
				r_characters.push_back(r);
				}
			}
		}
	}
	return 0;
}
int number_character(Mat image)
{
	 // Find the contours
	Mat image_clone = image.clone();
	vector<vector<Mat>> characters;
	vector<Rect> r_characters;
	vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
	vector<Mat> char_image;
	is_character(image,r_characters);
	if(r_characters.size()<4)
		return -1;

	for(int i = 0; i < r_characters.size() - 1; i++)
	{	
       
		for(int j = i + 1; j < r_characters.size(); j++)
			if(r_characters[i].x > r_characters[j].x)
			{
				Rect tg;
				tg = r_characters[i];
				r_characters[i] = r_characters[j];
				r_characters[j] = tg;
			}
	}
	for(int i = 0; i < r_characters.size(); i++)
	{
        Mat image_cnt = image(r_characters.at(i));
		// remove noise pixcel
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		Mat mask = Mat(image_cnt.rows, image_cnt.cols, CV_8UC1, cv::Scalar(255,255,255));
		Mat mask2 = Mat(image_cnt.rows, image_cnt.cols, CV_8UC1, cv::Scalar(255,255,255));
		findContours(image_cnt, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
		{
			drawContours(mask, contours, i, Scalar(0,0,0), -1);
		}
		Mat clean(image_cnt.size(), image_cnt.type());
		bitwise_or(image_cnt,mask,clean);     
		int ratio_white = countNonZero(clean); 
		if(ratio_white < clean.cols*clean.rows*0.85 )
        char_image.push_back(clean);
	}
	characters.push_back(char_image);
	if(char_image.size()<4)
		return -1;
	return char_image.size();
}
 Scalar fixHSVRange(int h, int s, int v){
    return (180 * h / 360, 255 * s / 100, 255 * v / 100);
 }
int Find_Plate_Color(Mat plate, Mat& output)
{
Mat img_hsv = plate.clone();
 cvtColor(img_hsv, img_hsv, COLOR_BGRA2BGR);

Mat img_white,img_black,img_red,img_yellow;

inRange(img_hsv, Scalar(117,124,119), Scalar(255,255,255), img_white); // white

inRange(img_hsv, Scalar(0,0,0), Scalar(102,89,76), img_black); // black

Mat img_white_not;
bitwise_not(img_white,img_white_not);
Mat img_black_not;
bitwise_not(img_black,img_black_not);

vector<int> number;
vector<Mat> img_num;
number.push_back(number_character(img_white));
img_num.push_back(img_white);
number.push_back(number_character(img_black));
img_num.push_back(img_black);
number.push_back(number_character(img_black_not));
img_num.push_back(img_black_not);
number.push_back(number_character(img_white_not));
img_num.push_back(img_white_not);


int maxElementIndex = std::max_element(number.begin(),number.end()) - number.begin();
int maxElement = *std::max_element(number.begin(), number.end());
if(maxElement <4) return -1;
output = img_num.at(maxElementIndex).clone();
return maxElement;

}
std::string LPR_Recognise::CharacterDetection(Mat plate){

	vector<vector<Mat>> characters;
	vector<Rect> r_characters;
	Mat plate_resize, rs_gray, rs_binary, character, rs_clone;
	RotatedRect minRect;
	string text_recognition;
	bool is_one_row = false;
	 rs_clone = plate.clone();
	if(Find_Plate_Color(rs_clone,rs_binary)<0) return "-1";
	adaptiveThreshold(rs_binary, rs_binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);
    vector<Mat> char_image;
	
	is_character(rs_binary,r_characters);
	
	double avg_H,avg_W,avg_Y;
	find_Average(r_characters,&avg_H,&avg_W,&avg_Y);
	if(avg_H>(double)(rs_clone.size().height/2-5)){
	 is_one_row = true;
	}
	else 
	{
		is_one_row = false;
	}

	if(r_characters.size()<4)
	return "-1";
	// sort x
	for(int i = 0; i < r_characters.size() - 1; i++)
	{	
       
		for(int j = i + 1; j < r_characters.size(); j++)
			if(r_characters[i].x > r_characters[j].x)
			{
				Rect tg;
				tg = r_characters[i];
				r_characters[i] = r_characters[j];
				r_characters[j] = tg;
			}
	}
	vector<Rect> character_temp;
	if(is_one_row!=true){
	vector<Rect> Fist_line,Second_line;
	for(int i =0;i< r_characters.size();i++){
		if(r_characters[i].y > rs_clone.size().height/3)
		{
			Second_line.push_back(r_characters[i]);
		}
		else Fist_line.push_back(r_characters[i]);
	}
	Mat img_rotate;
	
	if(Fist_line.size()>0){
		for(int i =0;i<Fist_line.size();i++)
		character_temp.push_back(Fist_line[i]);
	}
	if(Second_line.size()>0){
		for(int i =0;i<Second_line.size();i++)
		character_temp.push_back(Second_line[i]);
	}
	}
	else  character_temp = r_characters;
	int largest_area=0;
	for(int i = 0; i < character_temp.size(); i++)
	{
        Mat image = rs_binary(character_temp.at(i));
		// remove these noise pixcel
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		Mat mask = Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(255,255,255));
		Mat mask2 = Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(255,255,255));
		findContours(image, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
		{
		drawContours(mask, contours, i, Scalar(0,0,0), -1);
   	 	double a=contourArea( contours[i],false);  //  Find the area of contour
   		 if(a>largest_area){
        	largest_area=a;
			} 
		}
		Mat clean(image.size(), image.type());
		bitwise_or(image,mask,clean);     
		int ratio_white = countNonZero(clean); 
		if(ratio_white < clean.cols*clean.rows*0.85 )
        char_image.push_back(clean);

	}
	characters.push_back(char_image);
	if(r_characters.size()<4)
	return "-1";
	for (size_t i = 0; i < characters.size(); i++)
	{
		string result;
		for (int j = 0; j < characters.at(i).size(); ++j)
		{
			char cs = character_recognition(characters.at(i).at(j));
			result.push_back(cs);
		}
		text_recognition += result.c_str();
	}

	idx++;
	if(idx>50) idx=0;
	return text_recognition;
}
char LPR_Recognise::character_recognition(Mat img_character)
	{
		//Load SVM training file OpenCV 3.1
		Ptr<SVM> svmNew = SVM::create();
		svmNew = SVM::load("./svm.xml");
		char c = '*';
		vector<float> feature = calculate_feature(img_character);
		// Open CV3.1
		Mat m = Mat(1, number_of_feature, CV_32FC1);  
		for (size_t i = 0; i < feature.size(); ++i)
		{
			float temp = feature[i];
			m.at<float>(0, i) = temp;
		}
		int ri = int(svmNew->predict(m)); 
		if (ri >= 0 && ri <= 9)
			c = (char)(ri + 48); // ascii 0 = 48
		if (ri >= 10 && ri < 18)
			c = (char)(ri + 55); // accii A = 5, --> tu A-H
		if (ri >= 18 && ri < 22)
			c = (char)(ri + 55 + 2); //K-N, bo I,J
		if (ri == 22) c = 'P';
		if (ri == 23) c = 'S';
		if (ri >= 24 && ri < 27)
			c = (char)(ri + 60); //T-V,  
		if (ri >= 27 && ri < 30)
			c = (char)(ri + 61); //X-Z
		if (ri == 30) c = 'W';
		if (ri == 31) c = 'O';
		if (ri == 32) c = 'I';
		if (ri == 33) c = 'Q';
		if (ri == 34) c = 'R';
		if (ri == 35) c = 'J';

		return c;

	}
