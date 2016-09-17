// hw_code.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <utility>
#include <climits>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
//#include <Windows.h>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

#define SHOWIMG
typedef vector<Point> PenStroke;
struct SPenStroke
{
	PenStroke spenstroke;
	//vector <Point> spenstroke;
	ushort flag;
};
typedef  float DirectHist[9];
struct  OnlineCharacter
{
	vector<PenStroke>penStroke_vec_;
	vector<Mat>featMap_vec_;
	ushort label_;
};

void print_vec(vector<int> vec)
{
	for (int i = 0; i < vec.size() ; i++)
	{
		cout << vec[i] << endl;
	}
}

void print_vec(vector<string> vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << endl;
	}
}

int max_(int a, int b)
{
	int c = (a>b || a == b) ? a : b;
	return c;
}

int min_(int a, int b)
{
	int c = (a<b || a == b) ? a : b;
	return c;
}

void Normalize(vector<OnlineCharacter> &character)
{
	float x_min = character[0].penStroke_vec_[0][0].x;
	float x_max = character[0].penStroke_vec_[0][0].x;
	float y_min = character[0].penStroke_vec_[0][0].y;
	float y_max = character[0].penStroke_vec_[0][0].y;
	for (int n = 0; n<character.size(); ++n)
	{
		for (int i = 0; i<character[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<character[n].penStroke_vec_[i].size(); ++j)
			{
				x_min = min_(character[n].penStroke_vec_[i][j].x, x_min);
				x_max = max_(character[n].penStroke_vec_[i][j].x, x_max);
				y_min = min_(character[n].penStroke_vec_[i][j].y, y_min);
				y_max = max_(character[n].penStroke_vec_[i][j].y, y_max);
			}
		}
	}

	float scale = 1.0f * 60 / max(max(y_max - y_min, x_max - x_min), 0.0001f); ///for CASIA_OLHWDB1.x
	//float scale =1.0f*80/max(y_max-y_min,0.0001f); /// for CASIA_OLHWDB2.x
	for (int n = 0; n<character.size(); ++n)
	{
		for (int i = 0; i<character[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<character[n].penStroke_vec_[i].size(); ++j)
			{
				character[n].penStroke_vec_[i][j].x = (character[n].penStroke_vec_[i][j].x)*scale;
				character[n].penStroke_vec_[i][j].y = (character[n].penStroke_vec_[i][j].y)*scale;
			}
		}
	}

}
void Normalize_by_avg_height(vector<OnlineCharacter> &character)
{
	int x_min = 102400;
	int x_max = 0;
	int y_min = 102400;
	int y_max = 0;
	float  height = 0;
	int  xx = 0;
	int tmp = 0;
	float scale = 0.0;
	for (int n = 0; n<character.size(); ++n)
	{
		for (int i = 0; i<character[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<character[n].penStroke_vec_[i].size(); ++j)
			{
				x_min = min_(character[n].penStroke_vec_[i][j].x, x_min);
				x_max = max_(character[n].penStroke_vec_[i][j].x, x_max);
				y_min = min_(character[n].penStroke_vec_[i][j].y, y_min);
				y_max = max_(character[n].penStroke_vec_[i][j].y, y_max);
			}
		}
		tmp = y_max - y_min;
		if (tmp>20)
		{
			height += tmp;
			xx++;
		}

		y_min = 102400;
		y_max = 0;
		x_min = 102400;
		x_max = 0;
	}



	if (height>0 && xx>0)
	{
		height = height / xx;
		scale = 1.0f * 60 / max_(height, 0.0001f); //针对大多数文本行
	}
	else if (height == 0 && xx == 0)
	{
		scale = 1.0;                                   //针对一行文本行，每个字的行高都很小
	}

	for (int n = 0; n<character.size(); ++n)
	{
		for (int i = 0; i<character[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<character[n].penStroke_vec_[i].size(); ++j)
			{
				character[n].penStroke_vec_[i][j].x = (character[n].penStroke_vec_[i][j].x)*scale;
				character[n].penStroke_vec_[i][j].y = (character[n].penStroke_vec_[i][j].y)*scale;
			}
		}
	}

}
//读入uni函数
int readuni(char *infile, vector<OnlineCharacter> &characterVec, int &width, int &height)
{

	FILE *fp_uni;
	//FILE *fp_label;
	if (NULL == (fp_uni = fopen(infile, "r")))
	{
		fprintf(stdout, "Can not open uni file %s\n", infile);
		return 1;
	}

	vector<Point>penstr;
	//PenStroke p;
	OnlineCharacter character;
	//vector<PenStroke>character;
	Point  pPts;
	int    nPts = 0;
	char line[2048];
	char tmp_x[256];
	char tmp_y[256];
	while (fgets(line, 2048, fp_uni))
	{
		//fprintf(stdout,"%s\n",line);
		if (NULL != strstr(line, "SEGMENT LINE"))
		{
			//fprintf(fp_label,"%s",line);
			//fclose(fp_label);
			continue;
		}
		else if (NULL != strstr(line, "PEN_UP"))
		{
			character.penStroke_vec_.push_back(penstr);
			penstr.clear();
			nPts++;
			continue;
		}
		else if (NULL != strstr(line, "PEN_DOWN") || NULL != strstr(line, "SEGMENT WORD") || NULL != strstr(line, "END_SEG LINE"))
		{
			continue;
		}
		else if (NULL != strstr(line, "END_SEG WORD"))
		{
			characterVec.push_back(character);
			character.penStroke_vec_.clear();
			//character.featMap_vec_.clear();
			continue;
		}
		else
		{
			sscanf(line, "%s %s", tmp_x, tmp_y);/////读取坐标点
			pPts.x = atoi(tmp_x);
			pPts.y = atoi(tmp_y);
			pPts.y = pPts.y*(-1);
			//p.penstroke.push_back(pPts);
			penstr.push_back(pPts);
			//Point_chara.push_back(pPts);
			nPts++;
		}
	}
	fclose(fp_uni);

	//Normalize(characterVec);
	if (characterVec.size()>1)
	{
		Normalize_by_avg_height(characterVec);
	}
	else
	{
		Normalize(characterVec);
	}


	float x_min = characterVec[0].penStroke_vec_[0][0].x;
	float x_max = characterVec[0].penStroke_vec_[0][0].x;
	float y_min = characterVec[0].penStroke_vec_[0][0].y;
	float y_max = characterVec[0].penStroke_vec_[0][0].y;
	for (int n = 0; n<characterVec.size(); ++n)
	{
		for (int i = 0; i<characterVec[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<characterVec[n].penStroke_vec_[i].size(); ++j)
			{
				x_min = min_(characterVec[n].penStroke_vec_[i][j].x, x_min);
				x_max = max_(characterVec[n].penStroke_vec_[i][j].x, x_max);
				y_min = min_(characterVec[n].penStroke_vec_[i][j].y, y_min);
				y_max = max_(characterVec[n].penStroke_vec_[i][j].y, y_max);
			}
		}
	}


	for (int n = 0; n<characterVec.size(); ++n)
	{
		for (int i = 0; i<characterVec[n].penStroke_vec_.size(); ++i)
		{
			for (int j = 0; j<characterVec[n].penStroke_vec_[i].size(); ++j)
			{
				characterVec[n].penStroke_vec_[i][j].x = characterVec[n].penStroke_vec_[i][j].x - x_min + 40;
				characterVec[n].penStroke_vec_[i][j].y = characterVec[n].penStroke_vec_[i][j].y - y_min + 40;
			}
		}
	}

	width = x_max - x_min + 180;
	height = y_max - y_min + 80;

	return 0;
	//end
}
//得到原始图像
Mat Writ2raw_Bmp(vector<OnlineCharacter> characterVec, int width, int height, string out)
{
	Mat OfflineImg(height, width, CV_8UC1, Scalar(255));
	Point point_begin, point_end;
	//int segment_start = 
	for (int n = 0; n<characterVec.size(); n++)
	{
		int word_start = 100000;
		int word_end = 0;
		for (int i = 0; i<characterVec[n].penStroke_vec_.size(); i++)
		{
			for (int j = 0; j<characterVec[n].penStroke_vec_[i].size() - 1; j++)
			{
				if (characterVec[n].penStroke_vec_[i][j].x < word_start)
				{
					word_start = characterVec[n].penStroke_vec_[i][j].x;
				}
				if (characterVec[n].penStroke_vec_[i][j].x > word_end)
				{
					word_end = characterVec[n].penStroke_vec_[i][j].x;
				}

				point_begin = characterVec[n].penStroke_vec_[i][j];
				point_end = characterVec[n].penStroke_vec_[i][j + 1];
				cv::line(OfflineImg, point_begin, point_end, Scalar(0), 1, CV_AA);
				//imshow("img",OfflineImg);
				//waitKey();
			}
		}
		cout << "number of this word: " << n<< " start: " << word_start << " end: " << word_end<<" word_length: "<<word_end-word_start<<endl;
	}

	Mat small_OfflineImg;
	float rate = 0.8;
	resize(OfflineImg, small_OfflineImg, Size(width*rate, height *rate), 0, 0, CV_INTER_LINEAR);

	
	#ifdef SHOWIMG
		string name;
		name = out + "img.bmp";
		//namedWindow("uni");
		imshow("uni", small_OfflineImg);
		//cvShowImage("uni", OfflineImg);
		waitKey();
		//imwrite(name, OfflineImg);
	#endif
	
	return OfflineImg;
}
//得到分割后的图像（目前不用这个）
Mat Writ2seg_Bmp(vector<OnlineCharacter> characterVec, int width, int height, string out,vector<int> seg_position)
{
	Mat OfflineImg(height, width, CV_8UC3, Scalar(255,255,255));
	Point point_begin, point_end;
	for (int n = 0; n<characterVec.size(); n++)
	{
		int word_start = 100000;
		int word_end = 0;
		for (int i = 0; i<characterVec[n].penStroke_vec_.size(); i++)
		{
			for (int j = 0; j<characterVec[n].penStroke_vec_[i].size() - 1; j++)
			{
				if (characterVec[n].penStroke_vec_[i][j].x < word_start)
				{
					word_start = characterVec[n].penStroke_vec_[i][j].x;
				}
				if (characterVec[n].penStroke_vec_[i][j].x > word_end)
				{
					word_end = characterVec[n].penStroke_vec_[i][j].x;
				}

				point_begin = characterVec[n].penStroke_vec_[i][j];
				point_end = characterVec[n].penStroke_vec_[i][j + 1];
				cv::line(OfflineImg, point_begin, point_end, Scalar(0,0,0), 1, CV_AA);
			
			}
		}
		//cout << "number of this word: " << n << " start: " << word_start << " end: " << word_end << " word_length: " << word_end - word_start << endl;
	}

	//偏移seg_position
	int off_size = -20;
	for (int i = 0; i < seg_position.size(); i++)
	{
		seg_position[i] += off_size;
	}

	//draw the segment in OfflineImg
	for (int i = 0; i < seg_position.size(); i++)
	{
		point_begin.x = seg_position[i];
		point_begin.y = 0;
		point_end.x = seg_position[i];
		point_end.y = width;
		cv::line(OfflineImg, point_begin, point_end, Scalar(0,0,0), 1, CV_AA);
	}

	Mat small_OfflineImg;
	float rate = 0.8;
	resize(OfflineImg, small_OfflineImg, Size(width*rate, height *rate), 0, 0, CV_INTER_LINEAR);


#ifdef SHOWIMG
	string name;
	name = out + "img.bmp";
	//namedWindow("uni");
	cv::imshow("uni", small_OfflineImg);
	waitKey();
#endif

	return OfflineImg;
}
//计算一句话中的分割错误
int count_and_list_segment_error(vector<OnlineCharacter> characterVec,vector<int> seg_position, vector<int> error_word_position)
{
	Point point_begin, point_end;
	//偏移seg_position
	int off_size = -20;
	for (int i = 0; i < seg_position.size(); i++)
	{
		seg_position[i] += off_size;
	}	
	int count = 0;
	//有error word才进行下面的统计，否则直接输出0
	if (error_word_position.size() != 0)
	{	//draw the error_word's segment
		vector<int> error_seg;
		vector<int> error_seg_start;
		vector<int> error_seg_end;
		vector<int> error_word_start;
		vector<int> error_word_end;
		
		//判断分割线是否越界，如果多识别字，则会出错
		while (error_word_position.size()!=0 )
		{
			if (error_word_position.back() * 2 > seg_position.size())
			{
					error_word_position.pop_back();
					count++;
			}
			else break;			
		}
		if (error_word_position.size() != 0)
		{
			for (int n = 0; n < characterVec.size(); n++)
			{
				//判断这个字在不在error word的集合里面
				for (int k = 0; k < error_word_position.size(); k++)
				{
					if (n + 1 == error_word_position[k])//是错字
					{
						//计算错字的开始和结束点
						int word_start = 100000;
						int word_end = 0;
						for (int i = 0; i < characterVec[n].penStroke_vec_.size(); i++)
						{
							for (int j = 0; j < characterVec[n].penStroke_vec_[i].size() - 1; j++)
							{
								if (characterVec[n].penStroke_vec_[i][j].x < word_start)
								{
									word_start = characterVec[n].penStroke_vec_[i][j].x;
								}
								if (characterVec[n].penStroke_vec_[i][j].x > word_end)
								{
									word_end = characterVec[n].penStroke_vec_[i][j].x;
								}
							}
						}
						error_word_start.push_back(word_start);
						error_word_end.push_back(word_end);
					}
				}
			}
			for (int i = 0; i < error_word_position.size(); i++)
			{
				error_seg.push_back(2 * error_word_position[i] - 2);
				error_seg.push_back(2 * error_word_position[i] - 1);
			}
			//get the start and the end of the segment for the wrong word
			for (int i = 0; i < error_seg.size(); i++)
			{


				if (i % 2 == 0)
					error_seg_start.push_back(seg_position[error_seg[i]]);
				else
					error_seg_end.push_back(seg_position[error_seg[i]]);

			}
			//对error_seg_start/end进行扩宽，扩宽允许外延的长度
			int extension = 9;
			for (int i = 0; i < error_seg_start.size(); i++)
			{
				error_seg_start[i] -= extension;
			}
			for (int i = 0; i < error_seg_end.size(); i++)
			{
				error_seg_end[i] += extension;
			}

			//进行比较，得到错误结果
			int error_word_num = error_word_position.size();
			for (int i = 0; i < error_word_num; i++)
			{
				if (error_word_start[i] <= error_seg_start[i] || error_word_end[i] >= error_seg_end[i])
				{
					count++;
					//cout << "the " << i + 1<<"th word segment error" << endl;
				}
			}
			//debug
			/*
			cout << "error_word_start:" << endl;
			print_vec(error_word_start);
			cout << "error_word_end:" << endl;
			print_vec(error_word_end);
			cout << "error_seg_start:" << endl;
			print_vec(error_seg_start);
			cout << "error_seg_end:" << endl;
			print_vec(error_seg_end);
			*/
		}
	}
	
	return count;	
}
//Writ2shwo_error_word_Bmp的子函数
Mat show_error_word_Bmp(Mat OfflineImg, int width, int height, string out, vector<int> seg_position, vector<int> error_word_position)
{
	Point point_begin, point_end;
	//偏移seg_position
	int off_size = -20;
	for (int i = 0; i < seg_position.size(); i++)
	{
		seg_position[i] += off_size;
	}

	//draw the segment in OfflineImg
	for (int i = 0; i < seg_position.size(); i++)
	{
		point_begin.x = seg_position[i];
		point_begin.y = 0;
		point_end.x = seg_position[i];
		point_end.y = width;		
		cv::line(OfflineImg, point_begin, point_end, Scalar(0, 0, 0), 1, CV_AA);
	
	}
	//seg the wrong word
	while (error_word_position.size() != 0)
	{
		if (error_word_position.back() * 2 > seg_position.size())
		{
			error_word_position.pop_back();		
		}
		else break;
	}
	if (error_word_position.size() != 0)
	{
		//判断是否有多识别的字，是否越界
		while (error_word_position.back() * 2 > seg_position.size())
		{
			error_word_position.pop_back();
		}
		if (error_word_position.size() != 0){
			//draw the error_word's segment
			vector<int> error_seg;
			int error_size = error_word_position.size();
			for (int i = 0; i < error_size; i++)
			{
				error_seg.push_back(2 * error_word_position[i] - 2);
				error_seg.push_back(2 * error_word_position[i] - 1);
			}
			for (int i = 0; i < error_seg.size(); i++)
			{
				point_begin.x = seg_position[error_seg[i]];
				point_begin.y = 0;
				point_end.x = seg_position[error_seg[i]];
				point_end.y = width;
				cv::line(OfflineImg, point_begin, point_end, Scalar(0, 0, 255), 2, CV_AA);

			}
		}
		
	}
	//缩放图片
	Mat small_OfflineImg;
	float rate = 0.6;
	resize(OfflineImg, small_OfflineImg, Size(width*rate, height *rate), 0, 0, CV_INTER_LINEAR);

	string name;
	name = out + "img.bmp";
	//cv:namedWindow("uni", 0);
	cv::imshow("uni", small_OfflineImg);
	//cv::imwrite("C:\\Users\\summer\\Desktop\\handwriting_research\\uni.bmp", small_OfflineImg);
	waitKey();
	return OfflineImg;
}
//显示错字函数
Mat Writ2show_error_word_Bmp(vector<OnlineCharacter> characterVec, int width, int height, string out, vector<int> seg_position,vector<int> error_word_position)
{
	Mat OfflineImg(height, width, CV_8UC3, Scalar(255,255,255));
	Point point_begin, point_end;
	for (int n = 0; n<characterVec.size(); n++)
	{
		int word_start = 100000;
		int word_end = 0;
		for (int i = 0; i<characterVec[n].penStroke_vec_.size(); i++)
		{
			for (int j = 0; j<characterVec[n].penStroke_vec_[i].size() - 1; j++)
			{				
				point_begin = characterVec[n].penStroke_vec_[i][j];
				point_end = characterVec[n].penStroke_vec_[i][j + 1];
				cv::line(OfflineImg, point_begin, point_end, Scalar(0,0,0), 1, CV_AA);

			}
		}
	}
	show_error_word_Bmp(OfflineImg, width, height, out, seg_position, error_word_position);
	return OfflineImg;
}
//判断str是否以strStart开头的函数
bool startWith(const string& str, const string& strStart)
{
	if (str.empty() || strStart.empty())
	{
		return false;
	}

	return str.compare(0, strStart.size(), strStart) == 0 ? true : false;
}
//得到一个uniName的分割位置，返回一个vector
vector<int> getSegment(string uniName, string position_path)
{
	vector<int> word_position;
	ifstream postion_file(position_path);
	if (!postion_file)
	{		
		cerr << "The Name of the uni is wrong.Please try again." ;
		return word_position;
	}
	string tmp;
	int flag = 0;
	while (!postion_file.eof())
	{
		postion_file >> tmp;
		if (startWith(tmp, uniName))
		{
			while (true){
				postion_file >> tmp;
				if (startWith(tmp, "C"))
				{
					flag = 1;
					break;
				}
				int segment = atoi(tmp.c_str());
				word_position.push_back(segment);
				//cout << segment << " ";
				
			}
		}
		if (flag == 1)
			break;
	};
	postion_file.close();
	return word_position;
}
//得到一个uniName的错误字，返回一个vector
vector<int> getErrorWord(string uniName, string error_word_path)
{
	vector<int> error_word_position;
	ifstream error_postion_file(error_word_path);
	if (!error_postion_file)
	{
		cerr << "Cannot find error_path.";
		return error_word_position;
	}
	string tmp;
	int flag = 0;
	while (!error_postion_file.eof())
	{
		error_postion_file >> tmp;
		if (startWith(tmp, uniName))
		{
			while (true){
				error_postion_file >> tmp;
				if (startWith(tmp, "C"))
				{
					flag = 1;
					break;
				}
				int segment = atoi(tmp.c_str());
				error_word_position.push_back(segment);
				//cout << segment << " ";
				
			}
		}
		if (flag == 1)
			break;
	};
	error_postion_file.close();
	return error_word_position;
}
//得到所有uni的uniName，返回一个vector
vector<string> get_uni_name(string uniFolder)
{
	vector<string> uni_name;
	ifstream uni_file(uniFolder);
	if (!uni_file)
	{
		cerr << "The Name of the uni is wrong.Please try again.";
		return uni_name;
	}
	string tmp;
	while (!uni_file.eof())
	{
		uni_file >> tmp;
		if (startWith(tmp, "C"))
		{
			uni_name.push_back(tmp);
			//cout << tmp << " ";
		}		
	};
	uni_file.close();
	return uni_name;
}
//字符串数组连接函数，将str2连接到str1之后，在原始字符串上修改
char* connect(char str1[], char str2[])
{
	int i, j;
	i = strlen(str1);
	for (j = 0; str2[j] != '\0'; j++)
		str1[i + j] = str2[j];
	str1[i + j] = '\0';
	return str1;
}
//查看任意一句话分割错误和分割后图像的函数，需要输入uniName
void show_seg_img(char position_path[], char uniFolder_souce[], char error_word_path[])
{
	char uniFolder[200] = {};
	connect(uniFolder, uniFolder_souce);
	char uni_num[15];
	cout << "输入uni名: ";
	cin >> uni_num ;
	char uniName[] = "";
	connect(uniName, uni_num);
	cout << uniName << endl;
	
	vector<int> word_position;
	vector<int> error_word_position;
	int width;
	int height;
	//get word_position
	word_position = getSegment(uniName, position_path);
	//print_vec(word_position);
	if (word_position.size() == 0)
		cout << "wrong uni number or position_path " << endl;
	//get error_word_position
	error_word_position = getErrorWord(uniName, error_word_path);
	//print_vec(error_word_position);
	if (error_word_position.size() == 0)
		cout << "no error word in this sentense" << endl;
	
	//uniName必须在word_position和error_word_position产生之后连接
	connect(uniName, ".uni");
	connect(uniFolder, uniName);
	char *infile1 = uniFolder;

	vector<OnlineCharacter> characterVec;
	string out = "uni";
	int success = readuni(infile1, characterVec, width, height);
	if (success==1)
		return;
	//Writ2seg_Bmp(characterVec, width, height, out, word_position); system("pause");
	int count = count_and_list_segment_error(characterVec, word_position, error_word_position);
	cout << "the error segment in this sentense is: " << count << endl;
	Writ2show_error_word_Bmp(characterVec, width, height, out, word_position,error_word_position);// system("pause");
}
//计算一句话的分割错误的函数
int count_sentense_seg_error(char position_path[], char uniFolder_souce[], char error_word_path[], char uniName[])
{
	char uniFolder[200] = {};
	connect(uniFolder, uniFolder_souce);
	int width, height;
	vector<int> word_position;
	vector<int> error_word_position;
	vector<OnlineCharacter> characterVec;

	word_position = getSegment(uniName, position_path);	//get word_position
	//if (word_position.size() == 0) cout << "wrong uni number or position_path " << endl;

	error_word_position = getErrorWord(uniName, error_word_path);//get error_word_position
	//if (error_word_position.size() == 0) cout << "no error word in this sentense" << endl;		

	connect(uniName, ".uni");//uniName必须在word_position和error_word_position产生之后连接
	//cout << uniName;
	connect(uniFolder, uniName);
	char *infile1 = uniFolder;	
	int success = readuni(infile1, characterVec, width, height);
	if (success == 1)
	{
		cout << "fail to read uni"<<endl;
		return 0;
	}
	int count = count_and_list_segment_error(characterVec, word_position, error_word_position);
	//cout << "the error segment in this sentense is: " << count << endl;
	return count;
}
//计算所有分割所悟的函数
int count_total_seg_error(char position_path[], char uniFolder_souce[], char error_word_path[])
{
	vector<string>uni_name;
	uni_name = get_uni_name(position_path);
	//print_vec(uni_name);
	int total_count = 0;

	for (int i = 0; i <uni_name.size()-1; i++)
	{
		if (i % 100 == 0)
			cout<<i<<"'s " << "total error count is: " << total_count << endl;
		char uni_file[20];
		strcpy(uni_file, "");
		strcpy(uni_file, uni_name[i].c_str());
		//cout<<uni_file << " :"<<endl;
		total_count = total_count + count_sentense_seg_error(position_path, uniFolder_souce, error_word_path, uni_file);
	}
	cout<< "total error segment count is: " << total_count<<endl;

	return total_count;
}
//计算所有错误的函数（现在已不用）
int count_total_error_word(char position_path[],char error_word_path[])
{	
	vector<string>uni_name;
	uni_name = get_uni_name(position_path);
	//print_vec(uni_name);
	int total_count = 0;
	for (int i = 0; i <uni_name.size()-1; i++)
	{
		if (i % 100 == 0)
			cout << "total count is: " << total_count << endl;
		char uni_file[20];
		strcpy(uni_file, "");
		strcpy(uni_file, uni_name[i].c_str());				
		total_count += getErrorWord(uni_name[i], error_word_path).size();
	}
	cout << "total error word is: " << total_count << endl;
	return total_count;
}
//主函数
int main()
{
	char position_path[] = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_count2\\pdf_position.txt";
	char uni_path[] = "C:\\Users\\summer\\Desktop\\handwriting_research\\Competition_text_uni\\";
	char error_word_path[] = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_count2\\substitute_error_result.txt";
	//count_total_seg_error(position_path, uni_path, error_word_path);
	//count_total_error_word(position_path, error_word_path);
	
	while (true){
		show_seg_img(position_path, uni_path, error_word_path);
	}	
	system("pause");
	return 0;
}




