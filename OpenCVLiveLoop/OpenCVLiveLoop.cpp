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
const int MY_WAIT_IN_MS   = 5;
const int MY_FPS		  = 60;


VideoCapture initCam(const int index) {
	VideoCapture cap(index);
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam [0]" << endl;
		throw 0;
	}
	cap.set(CV_CAP_PROP_FPS, MY_FPS);
	double dWidth1 = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight1 = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);
	cout << "cam["<< index << "] Frame size: " << dWidth1 << " x " << dHeight1 << endl;
	return cap;
}

Mat genKernel(int size) {
	return getStructuringElement(MORPH_ELLIPSE, Size(size, size));
}


int StereoLoop()
{
	Mat inputFrame1;
	Mat inputFrame2;

	Mat resultFrame;
	Mat backgroundFrame;
	Mat mask;
	Mat diffImage;
	Mat blur;
	bool capturingBackground = true;
	double dist = 0.0;
	int distThreshhold = 900;
	int kernelSize = 9;
  
	bool exiting = false;
	//Mat erodeDiluteKernel = Mat::ones(9, 9, CV_8UC1);
	Mat erodeDiluteKernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	vector<Mat> channels;
	Mat diffSum;
	VideoCapture cap1;
	VideoCapture cap2;
	try {
		cap1 = initCam(0);
		cap2 = initCam(1);
	}
	catch (int e) {
		system("PAUSE");
		return 1;
	}

  cout << "Leertaste druecken um Hintergrund aufzunehmen." << endl;
  while(!exiting)
  {
	// Bilder von beiden Kameras holen
    bool bSuccess1 = cap1.read(inputFrame1);
    bool bSuccess2 = cap2.read(inputFrame2);
    if (!bSuccess1 || !bSuccess2)
    {
      cout << "Cannot read a frame from video stream ["<< (!bSuccess1 ? "0" : "1") << "]" << endl;
	  system("PAUSE");
      break;
    }
    
	if (capturingBackground) {
		// Hintergrundbild vor dem speichern glätten
		GaussianBlur(inputFrame1.clone(), backgroundFrame, Size(3,3), 0.0);
		imshow("Vorschau", inputFrame1);
	} else {		
		// Maske vorbereiten und geglättetes Bild berechnen (muss gleiches Verfahren wie gespeicherter Hintergrund sein)  
		mask = Mat::zeros(inputFrame1.rows, inputFrame1.cols, CV_8UC1);
		GaussianBlur(inputFrame1, blur, Size(3,3), 0.0);
		
		// Differenz Bild bauen und dann nach distThreshhold in Binärbild umwandeln
		absdiff(blur, backgroundFrame, diffImage);	
		for(int j = 0; j < diffImage.rows; ++j) {
			for(int i = 0 ; i < diffImage.cols; ++i) {
				Vec3b pix = diffImage.at<Vec3b>(j,i);

				dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);

				if(dist > distThreshhold) {
					mask.at<unsigned char>(j,i) = 255;
				}
			}
		}

		// Binärbild putzen
		Mat kernel = genKernel(kernelSize + 1);
		dilate(mask, diffImage, kernel);
		erode(diffImage, mask, kernel);

		// inputFrame2 und inputFrame1 mit diffImage als Maske zusammenbauen
		inputFrame2.copyTo(resultFrame);
		inputFrame1.copyTo(resultFrame,  mask);

		// Bilder anzeigen
		imshow("Live-Vordergrund", inputFrame1);
		imshow("Differenz Bild", mask);
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
			if (capturingBackground) {
				destroyWindow("Vorschau");
				namedWindow("Differenz Bild", CV_WINDOW_AUTOSIZE);
				createTrackbar("diffThreshold", "Differenz Bild", &distThreshhold, 18000);
				createTrackbar("diffKernel", "Differenz Bild", &kernelSize, 19);
			}
			capturingBackground = !capturingBackground;
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

