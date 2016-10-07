#include "StdAfx.h"
#include "Image.h"

BORA::Image::Image(void)
	: image_(NULL)
{
}

BORA::Image::Image( const Image &_other )
	: image_(NULL)
{
	if(isLoaded())
		ClearIplImage();

	image_ = cvCloneImage(_other.image_);	
}

BORA::Image::Image( const IplImage *_other )
	: image_(NULL)
{
	if(isLoaded())
		ClearIplImage();

	image_ = cvCloneImage(_other);
}

BORA::Image::~Image(void)
{
	ClearIplImage();
}

const IplImage * BORA::Image::getIplImage() const
{
	return image_;
}

IplImage * BORA::Image::getIplImage()
{
	return image_;
}

const unsigned int BORA::Image::getWidth() const
{
	return static_cast<unsigned int>(image_->width);
}

unsigned int BORA::Image::getWidth()
{
	return static_cast<unsigned int>(image_->width);
}

const unsigned int BORA::Image::getHeight() const
{
	return static_cast<unsigned int>(image_->height);
}

unsigned int BORA::Image::getHeight()
{
	return static_cast<unsigned int>(image_->height);
}

const unsigned int BORA::Image::getWidthStep() const
{
	return static_cast<unsigned int>(image_->widthStep);
}

unsigned int BORA::Image::getWidthStep()
{
	return static_cast<unsigned int>(image_->widthStep);
}

void BORA::Image::ClearIplImage()
{
	if(image_ != NULL)
	{
		cvReleaseImage(&image_);
		image_ = NULL;
	}
}

bool BORA::Image::isGrayImage()
{
	return image_->nChannels == 1;
}

const bool BORA::Image::isGrayImage() const
{
	return image_->nChannels == 1;
}

bool BORA::Image::isLoaded()
{
	return image_ != NULL;
}

const bool BORA::Image::isLoaded() const
{
	return image_ != NULL;
}

bool BORA::Image::LoadFromFile( const std::string &_path )
{
	IplImage *loadImage = cvLoadImage(_path.c_str());
	if(loadImage == NULL)
	{
		return false;
	}
	else
	{
		// Remove previous image
		if(isLoaded())
			ClearIplImage();

		image_ = loadImage;
		return true;
	}
}


void BORA::Image::operator=( const BORA::Image &_other )
{
	if(isLoaded())
		ClearIplImage();

	image_ = cvCloneImage(_other.getIplImage());
}

void BORA::Image::operator=( const IplImage *_other )
{
	if(isLoaded())
		ClearIplImage();

	image_ = cvCloneImage(_other);
}

const CvPoint2D32f BORA::Image::getCenterPosition() const
{
	return cvPoint2D32f(static_cast<double>(image_->width)/2.0, static_cast<double>(image_->height)/2.0);
}
