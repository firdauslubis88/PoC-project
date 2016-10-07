#pragma once


#include "global.h"
#include "model.h"
#include "tick.h"

class Cal3d{
protected:
	Model *model;
	std::string datapath;
	unsigned int lasttick;
	// constructors/destructor
public:
	Cal3d(std::string _datapath);
	Cal3d(std::string _datapath, std::string cfgfile);
	~Cal3d();

	// member functions
	bool Load(std::string _cfgfile);
	void Render();
	void Update();
	void SetAnimation(int _stateNo, float _delay);
};