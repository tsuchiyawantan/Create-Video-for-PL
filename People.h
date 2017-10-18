#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <time.h>

using namespace std;

class People{
private:
	vector<cv::Mat> pics;
	cv::Mat temp_image;

public:

	People(cv::VideoCapture &cap){
		while (true) {
			cap >> temp_image;
			if (temp_image.empty()) {
				std::cout << "Can't read frames from your camera\n";
				break;
			}
			cv::cvtColor(temp_image, temp_image, cv::COLOR_BGR2GRAY);
			pics.push_back(temp_image);
		}
		int i = 0;
	}
	~People(){}

	int getPicsLength(){
		return pics.size();
	}

	int getPics(cv::Mat &image, int i){
		if (getPicsLength() <= i) return -1;
		image = pics.at(i);

		return 1;
	}
};