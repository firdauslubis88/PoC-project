#pragma once
#include "FAST9.h"
#include "Descriptor.h"
#include "Patch.h"

namespace BORA{
	class MatchDescriptor : public Descriptor{
	public:
		INDEX index;
		//unsigned __int64 desc[5];
		DESC data;
		Position point;

		MatchDescriptor();
		MatchDescriptor(int &_index, BYTE *_patch, Position &_point);
		MatchDescriptor(int &_index, PATCH &_patch, Position &_point);
		MatchDescriptor(const MatchDescriptor &other);
		MatchDescriptor(const Descriptor &other);
		~MatchDescriptor();
		void inline patchInit();
		void inline setPatch(const BYTE *_quantized_patch);
		void inline setPatch(const PATCH &_quantized_patch);
		void inline setPatch(const unsigned __int64 *_desc);
		void inline setPatch(const DESC &_data);
		void getDescStr(char *_dst, bool _insert_space=false);

		unsigned __int64 compare(const Descriptor &_other);
	};

	struct MatchPair{
		int iData;
		int iQuery;
		int score;
		Position pData;
		Position pQuery;
		int inlier;
	};

}

