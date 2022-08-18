// Main.cpp

#include "Main.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
  LPR_Recognise RC;
  String Plate_result;
  cv::Mat imgOriginalScene;           // input image

  imgOriginalScene = cv::imread("image1.png");         // open image
  int down_width = 720;
  int down_height = 480;
  cv::Mat resized_down;
  //resize down
  cv::resize(imgOriginalScene, imgOriginalScene, cv::Size(down_width, down_height), cv::INTER_LINEAR);


    if (imgOriginalScene.empty()) {                             // if unable to open image
        std::cout << "error: image not read from file\n\n";     // show error message on command line
	    getchar();
        return(0);                                              // and exit program
    }

    std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(imgOriginalScene);          // detect plates

    vectorOfPossiblePlates = detectCharsInPlates(vectorOfPossiblePlates);                               // detect chars in plates

    cv::imshow("imgOriginalScene", imgOriginalScene);           // show scene image

    if (vectorOfPossiblePlates.empty()) {                                               // if no plates were found
        std::cout << std::endl << "no license plates were detected" << std::endl;       // inform user no plates were found
    }
    else {                                                                            // else
                                                                                      // if we get in here vector of possible plates has at leat one plate

                                                                                      // sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
        std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);

        // suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate
        PossiblePlate licPlate = vectorOfPossiblePlates.front();

        cv::imshow("imgPlate", licPlate.imgPlate);            // show crop of plate and threshold of plate
        cv::imshow("imgThresh", licPlate.imgThresh);

        Plate_result = RC.CharacterDetection(licPlate.imgPlate);
        std::cout<<Plate_result<<std::endl;
    }

    cv::waitKey(0);                 // hold windows open until user presses a key

    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawRedRectangleAroundPlate(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point2f p2fRectPoints[4];

    licPlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect

    for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
        cv::line(imgOriginalScene, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void writeLicensePlateCharsOnImage(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point ptCenterOfTextArea;                   // this will be the center of the area the text will be written to
    cv::Point ptLowerLeftTextOrigin;                // this will be the bottom left of the area that the text will be written to

    int intFontFace = cv::FONT_HERSHEY_SIMPLEX;                              // choose a plain jane font
    double dblFontScale = (double)licPlate.imgPlate.rows / 30.0;            // base font scale on height of plate area
    int intFontThickness = (int)std::round(dblFontScale * 1.5);             // base font thickness on font scale
    int intBaseline = 0;

    cv::Size textSize = cv::getTextSize(licPlate.strChars, intFontFace, dblFontScale, intFontThickness, &intBaseline);      // call getTextSize

    ptCenterOfTextArea.x = (int)licPlate.rrLocationOfPlateInScene.center.x;         // the horizontal location of the text area is the same as the plate

    if (licPlate.rrLocationOfPlateInScene.center.y < (imgOriginalScene.rows * 0.75)) {      // if the license plate is in the upper 3/4 of the image
                                                                                            // write the chars in below the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) + (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    }
    else {                                                                                // else if the license plate is in the lower 1/4 of the image
                                                                                          // write the chars in above the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) - (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    }

    ptLowerLeftTextOrigin.x = (int)(ptCenterOfTextArea.x - (textSize.width / 2));           // calculate the lower left origin of the text area
    ptLowerLeftTextOrigin.y = (int)(ptCenterOfTextArea.y + (textSize.height / 2));          // based on the text area center, width, and height

                                                                                            // write the text on the image
    cv::putText(imgOriginalScene, licPlate.strChars, ptLowerLeftTextOrigin, intFontFace, dblFontScale, SCALAR_YELLOW, intFontThickness);
}


