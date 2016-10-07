/*******************************************************************
 ImageUtility Ŭ����

 Training�� Matching���� ���� ����ϰ� �Ǵ� ���� �Լ� ���� Ŭ����
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
		/* �̹����� ��ǥ�� ���ڷ� �̹����� �ش� ��ǥ���� ��ġ�� �ε����� �޾ƿ´�. */
		static void GetPatchAndIndex(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index);
		static void GetPatchAndIndexAndOrientation(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index, double &_orientation);
		static void GetQPatchAndIndex(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_qpatch, BORA::INDEX &_dst_index);

		/* ��ġ�� ���ڷ� ��Ÿ�����̼ǵ� ��ġ�� �޾ƿ´�. */
		static void Quantization(const BORA::Patch &_src_patch, BORA::Patch &_dst_patch);
		static void Quantization_self(BORA::Patch &_patch);

		/* �̹����� ���ڷ� ����� Ȯ��� �̹����� �޾ƿ´�. */
		static void ExpandImage(const BORA::Image &_src, BORA::Image &_dst, BORA::Position &_shift_pos);

		// Dec. 2011, Min-Hyuk Sung
		static void CropImage(BORA::Image &_src, BORA::Image &_dst, 
			const BORA::Position &_start_pos, const BORA::Position &_end_pos);

		/* Į�� �̹����� ���ڷ� ��� ����� �̹����� �޾ƿ´�. */
		static void ColorImageToGray(const BORA::Image &_src, BORA::Image &_dst);

		/* ��� �̹����� ���ڷ� ��� ����� �̹����� �޾ƿ´�. */
		static void GrayImageToColor(const BORA::Image &_src, BORA::Image &_dst);

		/* �̹����� ��ġ���� ���ڷ� �ش� ��ġ�鿡 ���� ���� �ִ� �̹����� �޾ƿ´�. */
		static void DrawPoints(const BORA::Image &_src, const BORA::POSITIONS &_positions, BORA::Image &_dst, const CvScalar _color = cvScalar(0.0, 255.0, 0.0));
		static void DrawPoints_self(const BORA::POSITIONS &_positions, BORA::Image &_image, const CvScalar _color = cvScalar(0.0, 255.0, 0.0));
		
		/* �̹����� �밢�� ���̸� ���Ѵ�. */
		static unsigned int DiagonalDistance(const BORA::Image &_src);

		/* Patch�� ���ڷ� Image�� ��ȯ�ؼ� �����ش�. */
		static void PatchToImage(const BORA::Patch &_patch, BORA::Image &_dst, const unsigned int _zoom = 1);

		/* Quantized Patch�� ���ڷ� Image�� ��ȯ�ؼ� �����ش�. */
		static void QPatchToImage(const BORA::Patch &_qPatch, BORA::Image &_dst, const unsigned int _zoom = 1);

		/* Image�� ��θ� ���ڷ� �̹��� ������ �����Ѵ�. */
		static bool SaveImage(const BORA::Image &_img, const std::string &_path, const unsigned int &_save_type = BORA::SAVETYPE_IMG);

	private:
		/* ��ġ �����Ϳ� ��� �ȼ��� �̿��ؼ� �ε����� ���Ѵ�. */
		static void CalcIndex(const BORA::PATCH &_patch, const unsigned int &_center_intensity, BORA::INDEX &_dst_index);
	};
}


