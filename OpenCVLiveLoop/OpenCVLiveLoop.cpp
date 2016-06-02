// OpenCVLiveLoop.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iostream>
using namespace std;
using namespace cv;

const int MY_IMAGE_WIDTH  = 640;
const int MY_IMAGE_HEIGHT = 480;
const int MY_WAIT_IN_MS   = 20;

int StereoLoop()
{
  cv::VideoCapture cap1(1);
  cv::VideoCapture cap2(0);

  if(!cap1.isOpened())
  {
    cout << "Cannot open the video cam [0]" << endl;
    return -1;
  }

  if(!cap2.isOpened())
  {
    cout << "Cannot open the video cam [1]" << endl;
    return -1;
  }
  
  // Set cameras to 15fps (if wanted!!!)
  cap1.set(CV_CAP_PROP_FPS, 15);
  cap2.set(CV_CAP_PROP_FPS, 15);

  double dWidth1 = cap1.get(CV_CAP_PROP_FRAME_WIDTH);
  double dHeight1 = cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
  double dWidth2 = cap2.get(CV_CAP_PROP_FRAME_WIDTH);
  double dHeight2 = cap2.get(CV_CAP_PROP_FRAME_HEIGHT);

  // Set image size
  cap1.set(CV_CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
  cap1.set(CV_CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);
  cap2.set(CV_CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
  cap2.set(CV_CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);

  // display the frame size that OpenCV has picked in order to check 
  cout << "cam[0] Frame size: " << dWidth1 << " x " << dHeight1 << endl;
  cout << "cam[1] Frame size: " << dWidth2 << " x " << dHeight2 << endl;
  //cv::namedWindow("cam[1]",CV_WINDOW_AUTOSIZE);
  /*cv::namedWindow("diff",CV_WINDOW_AUTOSIZE);
  cv::namedWindow("result",CV_WINDOW_AUTOSIZE);
  */
  cv::Mat inputFrame1, inputFrame2;
  cv::Mat outputFrame1, outputFrame2;
  cv::Mat resultFrame;
  cv::Mat backgroundFrame;
  cv::Mat outputFrame3, outTest;
  int firstFrame = 0;
  double dist = 0.0;
  int distThreshhold = 900;
  bool exiting = false;



  vector<Mat> channels;
  Mat diffSum;

  cout << "Leertaste druecken um Hintergrund aufzunehmen." << endl;
  while(!exiting)
  {
   
    bool bSuccess1 = cap1.read(inputFrame1);
    bool bSuccess2 = cap2.read(inputFrame2);

    if (!bSuccess1)
    {
      cout << "Cannot read a frame from video stream [0]" << endl;
      break;
    }

    if (!bSuccess2)
    {
      cout << "Cannot read a frame from video stream [1]" << endl;
      break;
    }
    

    /*******************************todo*****************************/
	if (firstFrame <= 25) {
		GaussianBlur(inputFrame1.clone(), backgroundFrame, Size(3,3), 0.0);
		outputFrame1 = inputFrame1.clone();
		outputFrame2 = inputFrame2.clone();
		outputFrame3 = inputFrame1.clone();
		resultFrame = inputFrame1.clone();
		imshow("Vorschau", inputFrame1);
	} else {		
		cv::Mat blur;
		outputFrame3 = cv::Mat::zeros(inputFrame1.rows, inputFrame1.cols, CV_8UC1);
		GaussianBlur(inputFrame1, blur, Size(3,3), 0.0);
		absdiff(blur, backgroundFrame, outTest);

		for(int j = 0; j < outTest.rows; ++j) {
			for(int i = 0 ; i < outTest.cols; ++i) {
				cv::Vec3b pix = outTest.at<cv::Vec3b>(j,i);

				dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);

				if(dist > distThreshhold) {
					outputFrame3.at<unsigned char>(j,i) = 255;
				}
			}
		}
		
		dilate(outputFrame3, outTest, Mat::ones(5, 5, CV_8UC1));
		erode(outTest, outputFrame3, Mat::ones(5, 5, CV_8UC1));


		inputFrame2.copyTo(resultFrame);
		inputFrame1.copyTo(resultFrame,  outputFrame3);

		outputFrame1 = inputFrame1;
		outputFrame2 = inputFrame2;

		imshow("Live-Vordergrund", outputFrame1);
		imshow("Differenz Bild", outputFrame3);
		imshow("Live-Ergebnis", resultFrame);
	}
	
	
    /***************************end todo*****************************/
	
    //imshow("Hintergrund", backgroundFrame);

	auto key = cv::waitKey(MY_WAIT_IN_MS);
    switch (key)
    {
		case 27: {
			cout << "ESC key was pressed by the user" << endl;
			exiting = true;
			break;
		}
		case 32: {
			destroyWindow("Vorschau");
			cv::namedWindow("Differenz Bild", CV_WINDOW_AUTOSIZE);
			createTrackbar("diffThreshold", "Differenz Bild", &distThreshhold, 18000);
			firstFrame = 26;
			break;
		}
	
	} 
  }
  return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
  return StereoLoop();
}

