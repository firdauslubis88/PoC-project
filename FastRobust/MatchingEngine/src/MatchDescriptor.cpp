#include "stdafx.h"
#include "MatchDescriptor.h"

namespace BORA{
	MatchDescriptor::MatchDescriptor(){
		patchInit();
	}
	MatchDescriptor::MatchDescriptor(int &_index, BYTE *_patch, Position &_point)
		:index(_index),point(_point)
	{
		patchInit();
		setPatch(_patch);
	}

	MatchDescriptor::MatchDescriptor(int &_index, PATCH &_patch, Position &_point)
		:index(_index),point(_point)
	{
		patchInit();
		setPatch(_patch);
	}

	MatchDescriptor::MatchDescriptor( const MatchDescriptor &other )
		:index(other.index),point(other.point)
	{
		patchInit();
		setPatch(other.data);
	}

	MatchDescriptor::MatchDescriptor( const Descriptor &other )
		:index(0)				// cannot converted
		,point(Position(0,0))	// cannot converted
	{
		patchInit();
		setPatch(other.getDesc());

		for(unsigned int i = 0; i < 5; i++)
			data[i] = data[i] ^ 0xffffffffffffffffU;
	}

	MatchDescriptor::~MatchDescriptor(){
		//delete [] desc;
	}

	void MatchDescriptor::patchInit(){
		//desc = new unsigned __int64[5];
		//memset(desc, 0, 5*sizeof(*desc));
		for(int i=0; i<5; i++){
			data[i] = 0;
		}
	}

	void MatchDescriptor::setPatch(const BYTE *_quantized_patch){
		patchInit();
		for(int i=0; i<64; i++){
			//		for(int x=0; x<5; x++)
			//			printf("%016llx\n", desc[x]);
			//		printf("qpatch[i]=%d, %016llx | %016llx = %016llx\n",
			//			quantized_patch[i],
			//			desc[quantized_patch[i]],
			//			shift64[63-i],
			//			desc[quantized_patch[i]] | shift64[63-i]);
			data[_quantized_patch[i]] |= shift64[63-i];
		}
	}

	void MatchDescriptor::setPatch(const PATCH &_quantized_patch){
		patchInit();
		for(int i=0; i<64; i++){
			data[_quantized_patch[i]] |= shift64[63-i];
		}
	}

	void MatchDescriptor::setPatch(const unsigned __int64 *_desc){
		patchInit();
		for(int i=0; i<5; i++){
			data[i] = _desc[i];
		}
	}

	void MatchDescriptor::setPatch(const DESC &_data){
		patchInit();
		for(int i=0; i<5; i++){
			data[i] = _data[i];
		}
	}

	void MatchDescriptor::getDescStr(char *_dest, bool _insert_space){
		int ii=0;
		//	for(int x=0; x<5; x++)
		//		printf("%llx", desc[x]);
		for(int i=0; i<64; i++){
			for(int k=0; k<5; k++){
				//			printf("%d", ((desc[k]>>(63-i))&1));
				_dest[ii] = '0'+ ((data[k]>>(63-i))&1);
				ii++;
			}
			if(_insert_space){
				//			printf(" ");
				_dest[ii] = ' ';
				ii++;
			}
		}
		_dest[ii]='\0';
	}

	unsigned __int64 MatchDescriptor::compare(const Descriptor &_other){
		MatchDescriptor *other = (MatchDescriptor *)(&_other);
		return  (data[0] & other->data_[0]) |
				(data[1] & other->data_[1]) |
				(data[2] & other->data_[2]) |
				(data[3] & other->data_[3]) |
				(data[4] & other->data_[4]);
	}
}
