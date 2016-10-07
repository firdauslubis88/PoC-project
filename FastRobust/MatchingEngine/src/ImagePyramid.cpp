#include "stdafx.h"
#include "ImagePyramid.h"

namespace BORA{
	ImagePyramid::ImagePyramid(){
		for(int i=0; i<IMAGE_PYRAMID_DEPTH; i++)
			data[i] = NULL;
	}

	ImagePyramid::ImagePyramid(int _width, int _height){
		init(_width, _height);
	}

	ImagePyramid::~ImagePyramid(){
		clear();
	}

	void ImagePyramid::init(int _width, int _height){
		data[0] = cvCreateImage(cvSize(_width, _height), IPL_DEPTH_8U, 1);
		for(int i=1; i<IMAGE_PYRAMID_DEPTH; i++){
			data[i] = cvCreateImage(cvSize(_width>>i, _height>>i), IPL_DEPTH_8U, 1);
		}
	}

	void ImagePyramid::clear(){
		for(int i=0; i<IMAGE_PYRAMID_DEPTH; i++){
			if(data[i]){
				cvReleaseImage(&data[i]);
				data[i] = NULL;
			}
		}
	}

	void ImagePyramid::setImage(IplImage *_data){
		if(data[0] == NULL || data[0]->width != _data->width || data[0]->height != _data->height){
			clear();
			init(_data->width, _data->height);
		}
		cvCopy(_data, data[0]);
		for(int i=1; i<IMAGE_PYRAMID_DEPTH; i++){
			cvPyrDown(data[i-1], data[i]);
		}
	}

	void ImagePyramid::setImage(Image &_data){
		if(data[0]->width == _data.getWidth() && data[0]->height == _data.getHeight()){
			cvCopy(_data.getIplImage(), data[0]);
			for(int i=1; i<IMAGE_PYRAMID_DEPTH; i++){
				cvPyrDown(data[i-1], data[i]);
			}
		}
	}

	unsigned int ImagePyramid::getDescriptor(vector<MatchDescriptor> &dst, MatchOption &opt){
		unsigned int numOfCorners = 0;
		for(int i=0; i<IMAGE_PYRAMID_DEPTH; i++){
			POSITIONS corners;
			if(opt.one_point_mode){
				corners.push_back(Position(opt.point.x>>i, opt.point.y>>i));
			}else{
				FAST9::FindCornersIpl(data[i], opt.threshold, corners);
			}

			//calc num of corners
			numOfCorners += corners.size();

			//Position f;
			for(POSITIONS::iterator fIter = corners.begin(); fIter != corners.end(); ++fIter){
				//f=*fIter;
				if(fIter->x >= 7 && fIter->x < data[i]->width-7 && fIter->y >= 7 && fIter->y < data[i]->height-7){
					MatchDescriptor point;
					//switch (db.getIndexTable().m_index_type){
					//case BORA::Options::Indexing::INDEX_5:
					//	point.index = makeIndex5(gray, f);
					//	break;
					//case BORA::Options::Indexing::INDEX_13:
					//	point.index = makeIndex13(gray, f);
					//	break;
					//}
					Patch temp_p, temp_qp;
					ImageUtility::GetPatchAndIndex(data[i], *fIter, temp_p, point.index);
					point.point(fIter->x<<i, fIter->y<<i);
					
					ImageUtility::Quantization(temp_p, temp_qp);
					point.setPatch(temp_qp.getPATCH_ref());
					dst.push_back(point);
				}
			}

//			//debug
//			IplImage *temp = cvCreateImage(cvSize(data[i]->width, data[i]->height), IPL_DEPTH_8U, 3);
//			cvCvtColor(data[i], temp, CV_GRAY2RGB);
//			for(POSITIONS::iterator fIter = corners.begin(); fIter != corners.end(); ++fIter){
//				cvCircle(temp, cvPoint(fIter->x, fIter->y), 2, cvScalar(0,255,0),2);
//			}
//			char fff[100];
//			sprintf(fff, "p%d.jpg", i);
//			cvSaveImage(fff, temp);
//			cvReleaseImage(&temp);
		}

		return numOfCorners;
	}

}