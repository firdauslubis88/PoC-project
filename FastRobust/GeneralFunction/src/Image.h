/*******************************************************************
 MImage Class

 OpenCV�� IplImage�� �����ϴ� Ŭ����.
 Training�� Matching���� �������� ��� �Ѵ�.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"

namespace BORA
{
	class Image
	{
	private:
		IplImage *image_;		//***** OpenCV�� �̹��� �ڷ���

	public:
		Image(void);
		Image(const IplImage *_other);
		Image(const Image &_other);
		~Image(void);

		bool LoadFromFile( const std::string &_path );
		
		void operator=(const BORA::Image &_other);
		void operator=(const IplImage *_other);
		bool isLoaded();		//***** �̹����� ������ �����ϸ� true
		const bool isLoaded() const;

		bool isGrayImage();		//***** ����̹��� �̸� true
		const bool isGrayImage() const;

		const CvPoint2D32f getCenterPosition() const;

		/* �⺻���� set&get �Լ� */
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
