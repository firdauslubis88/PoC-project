/*******************************************************************
 ImageUtility 클래스

 Training과 Matching에서 같이 사용하게 되는 전역 함수 묶음 클래스
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Image.h"
#include "Patch.h"

namespace BORA
{
	const unsigned int SAVETYPE_IMG		= 0;
	const unsigned int SAVETYPE_PATCH		= 1;

	class ImageUtility
	{
	public:
		/* 이미지와 좌표를 인자로 이미지의 해당 좌표에서 패치와 인덱스를 받아온다. */
		static void GetPatchAndIndex(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index);
		static void GetPatchAndIndexAndOrientation(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index, double &_orientation);
		static void GetQPatchAndIndex(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_qpatch, BORA::INDEX &_dst_index);

		/* 패치를 인자로 퀀타이제이션된 패치를 받아온다. */
		static void Quantization(const BORA::Patch &_src_patch, BORA::Patch &_dst_patch);
		static void Quantization_self(BORA::Patch &_patch);

		/* 이미지를 인자로 배경이 확장된 이미지를 받아온다. */
		static void ExpandImage(const BORA::Image &_src, BORA::Image &_dst, BORA::Position &_shift_pos);

		// Dec. 2011, Min-Hyuk Sung
		static void CropImage(BORA::Image &_src, BORA::Image &_dst, 
			const BORA::Position &_start_pos, const BORA::Position &_end_pos);

		/* 칼라 이미지를 인자로 흑백 배경의 이미지를 받아온다. */
		static void ColorImageToGray(const BORA::Image &_src, BORA::Image &_dst);

		/* 흑백 이미지를 인자로 흑백 배경의 이미지를 받아온다. */
		static void GrayImageToColor(const BORA::Image &_src, BORA::Image &_dst);

		/* 이미지와 위치들을 인자로 해당 위치들에 점이 찍혀 있는 이미지를 받아온다. */
		static void DrawPoints(const BORA::Image &_src, const BORA::POSITIONS &_positions, BORA::Image &_dst, const CvScalar _color = cvScalar(0.0, 255.0, 0.0));
		static void DrawPoints_self(const BORA::POSITIONS &_positions, BORA::Image &_image, const CvScalar _color = cvScalar(0.0, 255.0, 0.0));
		
		/* 이미지의 대각선 길이를 구한다. */
		static unsigned int DiagonalDistance(const BORA::Image &_src);

		/* Patch를 인자로 Image로 변환해서 돌려준다. */
		static void PatchToImage(const BORA::Patch &_patch, BORA::Image &_dst, const unsigned int _zoom = 1);

		/* Quantized Patch를 인자로 Image로 변환해서 돌려준다. */
		static void QPatchToImage(const BORA::Patch &_qPatch, BORA::Image &_dst, const unsigned int _zoom = 1);

		/* Image와 경로를 인자로 이미지 파일을 저장한다. */
		static bool SaveImage(const BORA::Image &_img, const std::string &_path, const unsigned int &_save_type = BORA::SAVETYPE_IMG);

	private:
		/* 패치 데이터와 가운데 픽셀을 이용해서 인덱스를 구한다. */
		static void CalcIndex(const BORA::PATCH &_patch, const unsigned int &_center_intensity, BORA::INDEX &_dst_index);
	};
}


