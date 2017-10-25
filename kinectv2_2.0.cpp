#include <iostream>
#include <sstream>
#include "stdafx.h"
#include "Depth.h"
#include "Dot.h"
#include "NeonDesign.h"
#include "Log.h"
#include "CatmullSpline.h"
#include "NtKinect.h"
#include "Effect.h"
#include "Node.h"
#include "Graph.h"
#include "People.h"
#include <time.h>

#define HUE 80
#define SPACESIZE 10
#define EFFECT_FLAG 1
#define BOX_WIDTH 20
#define BOX_HEIGHT 20
Dot dot;
CatmullSpline catmull;
Graph graph;
Effect effect;
vector<vector<Node *>> former_node_array;
vector<vector<vector<Node *>>> box_node;

int test_count = 0;

void doIm(cv::Mat &result_img, vector<vector<Node *>> node_array, int rows, int cols){

	cv::Mat image = result_img.clone();
	for (int i = 0; i < node_array.size(); i++){
		for (int j = 0; j < node_array[i].size(); j++){
			Node *node = node_array[i].at(j);
			int y = (*node).getNodeY();
			int x = (*node).getNodeX();
			if (node->isAngularNode())
				circle(image, cv::Point(x, y), 3, cv::Scalar(0, 255, 0), -1, 8);
		}
	}
	//cv::imwrite("cornerimage/image" + to_string(test_count++) + ".png", image);
	cv::imshow("corner", image);
}

void doCatmull(cv::Mat &result_img, vector<vector<Node *>> node_array){
	catmull.init();
	catmull.drawLine(result_img, node_array, HUE);
	catmull.drawInlineHanddraw(result_img, node_array, HUE);
	/* ��`��������Ȃ����̕\�� */
	//catmull.drawInline(result_img, node_array, HUE);
	//doIm(result_img, node_array, result_img.rows, result_img.cols);
}


void doGraph(vector<vector<Node *>> &node_array){
	graph.toGraph(dot.divide_contours, node_array);
	//�n�_�ւ̃G�b�W���Ԗڂւ̃G�b�W�ɕύX����֐��͂����ɓ����
	graph.setEdgeToOtherNode(node_array);
	graph.setCorner(node_array);
	graph.setEdge(node_array);
	graph.deformeNode(node_array, ::box_node, BOX_WIDTH, BOX_HEIGHT);
}

void removeNodes(vector<vector<Node *>> &node_array){
	for (int i = 0; i < node_array.size(); i++){
		int end = node_array[i].size();
		if (node_array[i].size() > 2){
			int second_x = node_array[i].at(1)->getNodeX();
			int second_y = node_array[i].at(1)->getNodeY();
			int end_x = node_array[i].at(node_array[i].size() - 1)->getNodeX();
			int end_y = node_array[i].at(node_array[i].size() - 1)->getNodeY();
			if (second_x == end_x && second_y == end_y) {
				end = node_array[i].size() - 1;
			}
		}
		for (int j = 0; j < end; j++){
			Node *node = node_array[i].at(j);
			delete (node);
		}
	}
}

void removeFormerNodes(){
	for (int i = 0; i < ::former_node_array.size(); i++){
		int end = ::former_node_array[i].size();
		if (::former_node_array[i].size() > 2){
			int second_x = ::former_node_array[i].at(1)->getNodeX();
			int second_y = ::former_node_array[i].at(1)->getNodeY();
			int end_x = ::former_node_array[i].at(::former_node_array[i].size() - 1)->getNodeX();
			int end_y = ::former_node_array[i].at(::former_node_array[i].size() - 1)->getNodeY();
			if (second_x == end_x && second_y == end_y) {
				end = ::former_node_array[i].size() - 1;
			}
		}
		for (int j = 0; j < end; j++){
			Node *node = ::former_node_array[i].at(j);
			delete (node);
		}
	}
	::former_node_array.clear();
	::former_node_array.shrink_to_fit();
}

void copyNodes(vector<vector<Node *>> node_array, vector<vector<Node *>> &former_node_array){
	for (int i = 0; i < node_array.size(); i++){
		vector<Node *> node_array_child;
		for (int j = 0; j < node_array[i].size(); j++){
			Node node = (*node_array[i].at(j));
			if (j == node_array[i].size() - 1 && node_array[i].size() > 2){
				int second_x = node_array[i].at(1)->getNodeX();
				int second_y = node_array[i].at(1)->getNodeY();
				int end_x = node_array[i].at(node_array[i].size() - 1)->getNodeX();
				int end_y = node_array[i].at(node_array[i].size() - 1)->getNodeY();
				if (second_x == end_x && second_y == end_y) {
					Node *node_b = node_array_child.at(1); //���������m�[�h
					node_array_child.push_back(node_b);
					break;
				}
			}
			node_array_child.push_back(new Node(node));
		}

		//�m�[�h�̘A������
		Node *this_node;
		Node *prev_node;
		Node *next_node;

		for (int l = 0; l < node_array_child.size(); l++){
			if (l == 0){ //�n�_
				this_node = node_array_child.at(l);
				next_node = node_array_child.at(l + 1);
				(*this_node).addEdgeNode2(next_node, 0);
			}
			else if (l == node_array_child.size() - 1){ //�I�_
				this_node = node_array_child.at(l);
				prev_node = node_array_child.at(l - 1);
				int edgearray_num = (*prev_node).hasEdge(this_node);
				if (edgearray_num >= 0){
					Edge *edge = (*prev_node).getEdge(edgearray_num);
					(*this_node).addEdge(edge);
				}
			}
			else {
				this_node = node_array_child.at(l);
				prev_node = node_array_child.at(l - 1);
				next_node = node_array_child.at(l + 1);
				(*this_node).addEdgeNode2(next_node, 0);
				int edgearray_no = (*prev_node).hasEdge(this_node);
				if (edgearray_no >= 0){
					Edge *edge = (*prev_node).getEdge(edgearray_no);
					(*this_node).addEdge(edge);
				}
			}
		}
		former_node_array.push_back(node_array_child);
	}
}

//for finding near points
//�ߎ��_��T�����߂�3����vector�𐶐�
void mkBoxNode(cv::Mat src_img, vector<vector<vector<Node *>>> &box_node){
	int wimg = 0;
	int himg = 0;

	box_node.clear();
	box_node.shrink_to_fit();
	for (int i = 0; i < src_img.cols; i += BOX_WIDTH){
		wimg++;
	}
	for (int i = 0; i < src_img.rows; i += BOX_HEIGHT){
		himg++;
	}
	box_node.resize(himg + 5);
	for (int i = 0; i < himg + 1; i++){
		box_node[i].resize(wimg + 5);
	}
}

void copyNodesInfo(cv::Mat &src_img, vector<vector<vector<Node *>>> &box_node, vector<vector<Node *>> former_node_array){
	//former_node_array�̓_�����Ă���
	for (int i = 0; i < former_node_array.size(); i++){
		for (int j = 0; j < former_node_array[i].size(); j++){
			Node *node = former_node_array[i].at(j);
			int x = node->getNodeX() / BOX_WIDTH;
			int y = node->getNodeY() / BOX_HEIGHT;
			box_node[y].at(x).push_back(node);
		}
	}
}

void doDot(cv::Mat &src_img, cv::Mat &result_img){
	vector<vector<Node *>> node_array;

	dot.init();
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	dot.divideCon(SPACESIZE);
	doGraph(node_array);
	doCatmull(result_img, node_array);

	if (former_node_array.size()) {
		removeFormerNodes();
	}

	//���������
	if (node_array.size() > 0) {
		mkBoxNode(src_img, ::box_node);
		copyNodes(node_array, former_node_array);
		copyNodesInfo(src_img, ::box_node, former_node_array);
		removeNodes(node_array);
		node_array.clear();
		node_array.shrink_to_fit();
	}
}

//string ty = type2str(result_img.type());
//printf("Matrix: %s %dx%d \n", ty.c_str(), result_img.cols, result_img.rows);
string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

void doAfterImg(cv::Mat &result_img, cv::Mat depthcontour_img, vector<cv::Mat> &afterimg_array){
	//1��ڂ�dot����n�߂āA2��ڈȍ~��effect������result���ق����̂ŁAeffect����n�߂�
	if (afterimg_array.size() == 0){
		doDot(depthcontour_img, result_img);
		//1���ڂ𖾂邳������array�ɕۑ�
		effect.addAfterImg(result_img, afterimg_array);
	}
	else {
		//string ty = type2str(result_img.type());
		//printf("Matrix: %s %dx%d \n", ty.c_str(), result_img.cols, result_img.rows);
		//array�ɓ����Ă���摜��or���Z�q�łȂ��Ĕw�i�ɂ���
		for (int i = 0; i < afterimg_array.size(); i++){
			bitwise_or(result_img, afterimg_array.at(i), result_img);
		}

		//��œ���ꂽresult_img��w�i�ɂ��Đ����㏑������
		doDot(depthcontour_img, result_img);
		//���̎��̐���array�ɒǉ�����
		effect.addAfterImg(result_img, afterimg_array);
	}
}

int getMaxPeopleLength(vector<People> videos){
	int max = -1;
	for (auto itr = videos.begin(); itr != videos.end(); ++itr){
		if (max < (*itr).getPicsLength()) max = (*itr).getPicsLength();
	}
	return max;
}

void createVideo(vector<cv::Mat> &frames, int fps){
	int width = (*frames.begin()).cols;
	int height = (*frames.begin()).rows;
	cv::VideoWriter writer("ppls/ppl_result.avi", cv::VideoWriter::fourcc('I', '4', '2', '0'), fps, cv::Size(width, height), true);
	if (!writer.isOpened()){
		cout << "Error!! Unable to open video file for output." << endl;
		exit(-1);
	}
	for (auto itr = frames.begin(); itr != frames.end(); ++itr){
		writer << *itr;
	}

	cv::Mat black = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
	writer << black;
	writer << black;
}

void doLightArtLike(vector<cv::Mat> frames, vector<cv::Mat> &result_frames){
	vector<cv::Mat> afterimg_array;
	cv::Mat result_frame, matty;
	int count = 0;
	for (auto itr = frames.begin(); itr != frames.end(); ++itr){
		result_frame = cv::Mat((*frames.begin()).rows, (*frames.begin()).cols, CV_8UC3, cv::Scalar(0, 0, 0));
		if (EFFECT_FLAG){			/* EFFECT_FLAG=1�Ȃ�΁A�c������version */
			cv::threshold(*itr, matty, 1, 255, cv::THRESH_BINARY);
			doAfterImg(result_frame, matty, afterimg_array);
		}
		else {
			/* �c���Ȃ�version */
			doDot(*itr, result_frame);
		}
		cv::GaussianBlur(result_frame, result_frame, cv::Size(21, 3), 20, 3);
		result_frames.push_back(result_frame);
	}
}

int getRandomNumfromVids(vector<People> &videos){
	int random_num = rand() % videos.size();
	if ((videos.at(random_num)).getUsed())  return getRandomNumfromVids(videos);
	(videos.at(random_num)).setUsed();
	return random_num;
}

int getProcessingVids(vector<int> dummy){
	for (auto itr = dummy.begin(); itr != dummy.end(); ++itr){
		if (*itr >= 0) return 1;
	}
	return 0;
}

int allVideosUsed(vector<People> videos){
	for (auto itr = videos.begin(); itr != videos.end(); ++itr){
		if ((*itr).getPicsLength() != 0) return false;
	}
	return true;
}

int allVideosChecked(vector<People> videos){
	for (auto itr = videos.begin(); itr != videos.end(); ++itr){
		if (!(*itr).getUsed()) return false;
	}
	return true;
}

void choosePics(cv::Mat &image, People video, int fps, int video_flag){

}

void doJob(){
	vector<People> videos;
	vector<cv::Mat> frames, result_frames;
	int count = 1;
	srand(time(NULL));

	//Capture recorded vids
	while (true){
		cv::VideoCapture cap("ppls/ppl_" + to_string(count) + ".avi");
		if (!cap.isOpened()) {
			cout << "Unable to open the camera\n";
			break;
		}
		People people(cap);
		videos.push_back(people);
		count++;
	}

	int video_flag = 0;
	vector<int> dummy;
	vector<int> video_dummy;
	int fps = 30;
	int height = (*videos.begin()).getHeight();
	int width = (*videos.begin()).getWidth();
	while (video_flag >= 0){
		cv::Mat result_image = cv::Mat(height, width, CV_8UC1, cv::Scalar(0, 0, 0));
		if ((video_flag == 0 || video_flag % 25 == 0) && !allVideosChecked(videos)){
			int i = getRandomNumfromVids(videos);
			dummy.push_back(i);
		}
		if (getProcessingVids(dummy)){
			for (auto itr = dummy.begin(); itr != dummy.end(); ++itr){
				if (*itr < 0) continue;
				cv::Mat image;
				videos.at(*itr).getPics(image, video_flag, fps);
				cv::bitwise_or(image, result_image, result_image);
				if ((videos.at(*itr)).getPicsLength() == 0) {
					*itr = -1;
				}
			}
		}
		if (allVideosUsed(videos)){
			break;
		}
		frames.push_back(result_image);
		video_flag++;
	}

	//add afterimage and make it light-art-like
	doLightArtLike(frames, result_frames);

	createVideo(result_frames, fps);
}

void main() {
	try {
		doJob();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}
}