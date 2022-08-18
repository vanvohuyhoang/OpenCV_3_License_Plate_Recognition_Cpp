#ifndef Recognise_H
#define Recognise_H
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/ml.hpp>

#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::ml;


class LPR_Recognise{
    
    public:
    LPR_Recognise();
    std::string CharacterDetection(Mat plate);
    char character_recognition(Mat img_character);

};
#endif
