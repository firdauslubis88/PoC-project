/*******************************************************************
 MImage Class

 OpenCV의 IplImage를 래핑하는 클래스.
 Training과 Matching에서 공용으로 사용 한다.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"

namespace BORA
{
	class Image
	{
	private:
		IplImage *image_;		//***** OpenCV의 이미지 자료형

	public:
		Image(void);
		Image(const IplImage *_other);
		Image(const Image &_other);
		~Image(void);

		bool LoadFromFile( const std::string &_path );
		
		void operator=(const BORA::Image &_other);
		void operator=(const IplImage *_other);
		bool isLoaded();		//***** 이미지가 실제로 존재하면 true
		const bool isLoaded() const;

		bool isGrayImage();		//***** 흑백이미지 이면 true
		const bool isGrayImage() const;

		const CvPoint2D32f getCenterPosition() const;

		/* 기본적인 set&get 함수 */
		const IplImage	*getIplImage() const;
		IplImage		*getIplImage();

		const unsigned int	getWidth() const;
		unsigned int		getWidth();
		const unsigned int	getHeight() const;
		unsigned int		getHeight();
		const unsigned int	getWidthStep() const;
		unsigned int		getWidthStep();

	public:
		void ClearIplImage();
	};
}
