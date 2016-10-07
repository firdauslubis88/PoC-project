#include <stdio.h>
#include "Match.h"

#define DISSIMILARITY_SCORE 5

void show_help() 
{
	printf("Usage: \n");
	printf("Analysis <training_file_path> <output_file_path>\n");
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

	BORA::Match match;
	int width = 0, height = 0;
	match.loadData(argv[1], width, height);
	match.analysis(DISSIMILARITY_SCORE, argv[2]);


#ifdef WIN32
	system("pause");
#endif
	return 0;
}