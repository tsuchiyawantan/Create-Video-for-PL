#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <time.h>
#include <list>

using namespace std;

class People{
private:
	list<cv::Mat> pics;
	cv::Mat temp_image, interpolate_image;
	bool used = false;
	int fps = -1;
	int frame_count = 0;

public:
	void changeDesign(cv::Mat &src_image, int i){
		/*
		case 0 : Normal
		case 1 : Opposite
		*/
		switch (i){
		case 0 : 
			break;
		case 1 : cv::flip(src_image, src_image, 1);
			break;
		}
	}

	People(cv::VideoCapture &cap){
		int i = rand() % 2;
		while (true) {
			cap >> temp_image;
			if (temp_image.empty()) {
				std::cout << "Can't read frames from your camera\n";
				break;
			}
			cv::cvtColor(temp_image, temp_image, cv::COLOR_BGR2GRAY);
			changeDesign(temp_image, i);
			pics.push_back(temp_image);
		}
		interpolate_image = cv::Mat(cap.get(CV_CAP_PROP_FRAME_HEIGHT), cap.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC1, cv::Scalar(0, 0, 0));
		setFps(cap.get(CV_CAP_PROP_FPS));
	}
	~People(){}

	int getPicsLength(){
		if (pics.empty()) return 0;
		return pics.size();
	}

	int getPics(cv::Mat &image, int video_frame, int fps){
		if (pics.empty()) return -1;
		if (getFps() < fps){
			if(video_frame % getFps() == 0){
				image = *pics.begin();
				interpolate_image = image;
				pics.pop_front();
			}
			else {
				image = interpolate_image;
			}
		}
		else{
			image = *pics.begin();
			pics.pop_front();
		}
		return 1;
	}

	void setUsed(){
		used = true;
	}

	bool getUsed(){
		return used;
	}

	int getHeight(){
		return (*pics.begin()).rows;
	}	
	
	int getWidth(){
		return (*pics.begin()).cols;
	}

	void setFps(int i){
		fps = i;
	}

	int getFps(){
		return fps;
	}
};
