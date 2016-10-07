#include "cal3d2.h"


Cal3d::Cal3d(std::string _datapath)
	:model(NULL), datapath(_datapath)
{
	lasttick = Tick::getTick();
}

Cal3d::Cal3d(std::string _datapath, std::string _cfgfile)
	:model(NULL), datapath(_datapath)
{
	lasttick = Tick::getTick();
	Load(_cfgfile);
}

Cal3d::~Cal3d()
{
	if(model){
		model->onShutdown();
		delete model;
	}
}

bool Cal3d::Load( std::string _cfgfile )
{
	if(model){
		model->onShutdown();
		delete model;
	}
	model = new Model();
	//pModel->setPath(datapath+"/"+dataDir+"/");
	//if(!model->onInit(datapath+"/"+_cfgfile)){
	if(!model->onInit(_cfgfile)){
		delete model;
		model = NULL;
		std::cerr << "Model initialization failed! (" << _cfgfile << ")" << std::endl;
		return false;
	}
	std::cout << std::endl;

}

void Cal3d::Render()
{
	glPushMatrix();
	double scale_ = 0.005 * (1.0 / model->getRenderScale());
	glScaled(scale_, scale_, scale_);
	model->onRender();
	glPopMatrix();
}

void Cal3d::Update()
{
	// get the current tick value
	unsigned int tick;
	tick = Tick::getTick();

	// calculate the amount of elapsed seconds
	float elapsedSeconds;
	elapsedSeconds = (float)(tick - lasttick) / 1000.0f;

	model->onUpdate(elapsedSeconds);

	lasttick = tick;
}

void Cal3d::SetAnimation( int stateNo, float delay )
{
	model->setAnimation(stateNo, delay);

}
