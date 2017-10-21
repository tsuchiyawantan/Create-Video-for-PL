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
	cv::Mat temp_image;
	bool used = false;

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
	}
	~People(){}

	int getPicsLength(){
		return pics.size();
	}

	int getPics(cv::Mat &image, int i){
		if (getPicsLength() <= i) return -1;
		//image = pics.at(i);

		return 1;
	}

	int getPics(cv::Mat &image){
		if (pics.empty()) return -1;
		image = *pics.begin();
		pics.pop_front();
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
};