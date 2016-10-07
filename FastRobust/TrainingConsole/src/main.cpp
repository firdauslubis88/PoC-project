#include <stdio.h>
#include <stdlib.h>
#include <FastRobustTraining.h>

void show_help() 
{
	printf("Usage: \n");
	printf("Training <image_file_path> <option_file_path>\n");
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		show_help();
#ifdef WIN32
		system("pause");
#endif
		return false;
	}

	char output_file[512];
	bool ret = FastRobustTraining::training_start(argv[1], argv[2], output_file);

#ifdef WIN32
	system("pause");
#endif
	return ret;
}