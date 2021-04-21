#include "lab7_lib.h"

bool hasNeighbourBlack1(Mat image, int row, int coll)
{
	bool Step3 = !image.at<bool>(row, coll-1) || !image.at<bool>(row+1, coll) || !image.at<bool>(row, coll+1);
	bool Step4 = !image.at<bool>(row+1, coll) || !image.at<bool>(row, coll+1) || !image.at<bool>(row-1, coll);
	return Step3 && Step4;
}

bool hasNeighbourBlack2(Mat image, int row, int coll)
{
	bool Step3 = !image.at<bool>(row, coll-1) || !image.at<bool>(row+1, coll) || !image.at<bool>(row-1, coll);
	bool Step4 = !image.at<bool>(row, coll-1) || !image.at<bool>(row, coll+1) || !image.at<bool>(row-1, coll);
	return Step3 && Step4;
}

bool hasOneWhiteBlack(Mat image, int row, int coll)
{
	int Trans = 0;
	vector<bool> Matrix{
		image.at<bool>(row -1, coll),
		image.at<bool>(row -1, coll+1),
		image.at<bool>(row, coll +1),
		image.at<bool>(row+1, coll +1),
		image.at<bool>(row +1, coll),
		image.at<bool>(row +1, coll -1),
		image.at<bool>(row, coll -1),
		image.at<bool>(row -1, coll -1),
		image.at<bool>(row -1, coll)
	};

	for(int i=0; i<Matrix.size()-1; i++)
	{
		if(!Matrix[i] && Matrix[i+1])
			Trans++;
	}

	return (Trans == 1);
}

bool hasWhiteNeighbours(Mat image, int row, int coll)
{
	int NoNiggaCounter = 0;
	for(int h = -1; h <= 1; h++)
	{
		for(int l = -1; l<= 1; l++)
		{
			if(h==0 && l == 0)
				continue;
			if(image.at<bool>(row + h, coll + l))
				NoNiggaCounter++;
		}
	}
	return (NoNiggaCounter >= 2 && NoNiggaCounter <=6);
}

Mat MakeSkeletImage(Mat inputImage, bool isONE)
{
	Mat BoolImage;
	threshold(inputImage, BoolImage, 200, 255, THRESH_BINARY);
	erode(BoolImage, BoolImage, getStructuringElement(MORPH_RECT, Size(3, 3)));
	dilate(BoolImage, BoolImage, getStructuringElement(MORPH_RECT, Size(3, 3)));
	Mat ChangedImage = BoolImage.clone();
	if(isONE)
	{
		imshow("Input", BoolImage);
		while (waitKey(1) != 27)
		{
		}
	}
	int Counter = -1;
	while(Counter != 0)
	{
		Counter = 0;
		for(int h = 1; h<= BoolImage.rows-1; h++)
		{
			for(int l = 1; l<= BoolImage.cols-1; l++)
			{
				if(BoolImage.at<bool>(h, l)) //0
				{
					if(hasNeighbourBlack1(BoolImage, h, l) && hasWhiteNeighbours(BoolImage, h, l) && hasOneWhiteBlack(BoolImage, h, l))
					{
						ChangedImage.at<bool>(h, l) = false;
						Counter ++;
					}
				}
			}
		}

		for(int h = 1; h<= BoolImage.rows-1; h++)
		{
			for(int l = 1; l<= BoolImage.cols-1; l++)
			{
				if(BoolImage.at<bool>(h, l)) //0
				{
					if(hasNeighbourBlack2(BoolImage, h, l) && hasWhiteNeighbours(BoolImage, h, l) && hasOneWhiteBlack(BoolImage, h, l))
					{
						ChangedImage.at<bool>(h, l) = false;
						Counter ++;
					}
				}
			}
		}
		BoolImage = ChangedImage;
	}
	return ChangedImage;
}

void Skeletonization()
{
	const Mat inputImage = imread("C://Users//Ilya//source//repos//lab7_CV//lab7_CV//img.jpg", IMREAD_GRAYSCALE);
	if(inputImage.empty())
	{
		cout << endl << endl << endl << "ÈÇÎÁÐÀÆÅÍÈÅ ÍÅ ÍÀÉÄÅÍÎ" << endl << endl << endl;
		return;
	}
	Mat ChangedImage = MakeSkeletImage(inputImage, true);
	imshow("Result", ChangedImage);
	while (waitKey(1) != 27)
	{
	}
	destroyAllWindows();
}