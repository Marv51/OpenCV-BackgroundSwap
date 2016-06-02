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


VideoCapture initCam(const int index) {
	VideoCapture cap(index);
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam [0]" << endl;
		throw 0;
	}
	cap.set(CV_CAP_PROP_FPS, 15);
	double dWidth1 = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight1 = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);
	cout << "cam["<< index << "] Frame size: " << dWidth1 << " x " << dHeight1 << endl;
	return cap;
}


int StereoLoop()
{
	Mat inputFrame1;
	Mat inputFrame2;
	Mat outputFrame1;
	Mat resultFrame;
	Mat backgroundFrame;
	Mat outputFrame3, outTest;
	bool firstFrame = true;
	double dist = 0.0;
	int distThreshhold = 900;
  
	bool exiting = false;
	Mat erodeDiluteKernel = Mat::ones(5, 5, CV_8UC1);

	vector<Mat> channels;
	Mat diffSum;
	VideoCapture cap1;
	VideoCapture cap2;
	try {
		cap1 = initCam(1);
		cap2 = initCam(0);
	}
	catch (int e) {
		return 1;
	}

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
    
	if (firstFrame) {
		GaussianBlur(inputFrame1.clone(), backgroundFrame, Size(3,3), 0.0);
		outputFrame1 = inputFrame1.clone();
		outputFrame3 = inputFrame1.clone();
		resultFrame = inputFrame1.clone();
		imshow("Vorschau", inputFrame1);
	} else {		
		Mat blur;
		outputFrame3 = Mat::zeros(inputFrame1.rows, inputFrame1.cols, CV_8UC1);
		GaussianBlur(inputFrame1, blur, Size(3,3), 0.0);
		absdiff(blur, backgroundFrame, outTest);

		for(int j = 0; j < outTest.rows; ++j) {
			for(int i = 0 ; i < outTest.cols; ++i) {
				Vec3b pix = outTest.at<Vec3b>(j,i);

				dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);

				if(dist > distThreshhold) {
					outputFrame3.at<unsigned char>(j,i) = 255;
				}
			}
		}
		
		dilate(outputFrame3, outTest, erodeDiluteKernel);
		erode(outTest, outputFrame3, erodeDiluteKernel);


		inputFrame2.copyTo(resultFrame);
		inputFrame1.copyTo(resultFrame,  outputFrame3);

		outputFrame1 = inputFrame1;

		imshow("Live-Vordergrund", outputFrame1);
		imshow("Differenz Bild", outputFrame3);
		imshow("Live-Ergebnis", resultFrame);
	}
	
	
    auto key = waitKey(MY_WAIT_IN_MS);
    switch (key)
    {
		case 27: {
			cout << "ESC key was pressed by the user" << endl;
			exiting = true;
			break;
		}
		case 32: {
			destroyWindow("Vorschau");
			namedWindow("Differenz Bild", CV_WINDOW_AUTOSIZE);
			createTrackbar("diffThreshold", "Differenz Bild", &distThreshhold, 18000);
			firstFrame = false;
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

