#include "lab7_lib.h"

void VideoMedian()
{
	Mat frame;
	VideoCapture cap;
	cap.open("0.avi");
	while(waitKey(1) != 27)
	{
		clock_t start = clock();
		cap.read(frame);
		if (frame.empty())
		{
			imshow("Video", imread("exit_image.png"));
			while (waitKey(1) != 27){}
			destroyAllWindows();
			return;
		}
		
		Mat frameOUT = frame.clone();
		cvtColor(frame, frame, COLOR_BGR2GRAY);

		threshold(frame, frame, 70, 255, THRESH_BINARY);
		erode(frame, frame, getStructuringElement(MORPH_RECT, Size(3, 3)));
		dilate(frame, frame, getStructuringElement(MORPH_RECT, Size(3, 3)));

		frame = MakeSkeletImage(frame, false);
		normalize(frame, frame, 0, 255, NORM_MINMAX);
		frame.convertTo(frame, CV_8UC1);
		vector<Vec4i> lines;
		HoughLinesP(frame, lines, 1, CV_PI / 180, 50, 30, 50);

		vector<Point> allLine;
		for(size_t i = 0; i< lines.size(); i++)
		{
			Vec4i currPoint = lines[i];
			Point lineP1 = Point(currPoint[0], currPoint[1]);
			Point lineP2 = Point(currPoint[2], currPoint[3]);
			allLine.push_back(lineP1);
			allLine.push_back(lineP2);
			line(frameOUT, lineP1, lineP2, Scalar(0, 0, 255), 3, LINE_AA);
		}
		if(clock() - start < 28)
		{
			Sleep(28 - clock() + start); //28- +- пара миллисекунд на другие вычисления, так получается чисто 33 миллисекунды на кадр
		}
		imshow("Video", frameOUT);
	}
}