#include "lab5.h"
int godPleaseShowMeWhatSideCubeIs(vector<Point2f> imagePoints)
{
	int CubeRot = 42;
	if((imagePoints[0].x < imagePoints[1].x && imagePoints[0].y > imagePoints[1].y) || (imagePoints[0].x < imagePoints[1].x && imagePoints[0].y < imagePoints[1].y)) //—переди или сзади
	//Ёто сзади
	{
		if (imagePoints[6].y <= imagePoints[2].y)
			if (imagePoints[1].x >= imagePoints[2].x)
				CubeRot = 7; //ѕрава€ и верхн€€ грани на нас
			else
				CubeRot = 5; //Ћева€ и верхн€€
		else 
			if (imagePoints[1].x <= imagePoints[2].x)
				CubeRot = 6; //ѕрава€ и нижн€€ грани
			else
				CubeRot = 4; //Ћева€ и нижн€€
	}                                                                                                                             

	else
	//это спереди
	{
		if (imagePoints[4].y <= imagePoints[0].y)
			if (imagePoints[3].x >= imagePoints[0].x)
				CubeRot = 3; //ѕрава€ и верхн€€ грани на нас
			else
				CubeRot = 1; //Ћева€ и верхн€€
		else 
			if (imagePoints[3].x <= imagePoints[0].x)
				CubeRot = 0; //ѕрава€ и нижн€€ грани
			else
				CubeRot = 2; //Ћева€ и нижн€€
	}

	return CubeRot;
}

void drawWunderContours(Mat inputImage, vector<int> coordinates, vector<Point2f> imagePoints, vector<pair<Point, Point>> cntsLine)
{
	if (coordinates.size() != 6)
		return;
	/*vector<vector<Point>> cntsLine = {
				{imagePoints[coordinates[0]], imagePoints[coordinates[1]], imagePoints[coordinates[2]],imagePoints[coordinates[3]]},
				{imagePoints[coordinates[4]], imagePoints[coordinates[5]], imagePoints[coordinates[6]],imagePoints[coordinates[7]]},
				{imagePoints[coordinates[8]], imagePoints[coordinates[9]], imagePoints[coordinates[10]],imagePoints[coordinates[11]]}};*/
	vector<vector<Point>> cnts = { {
				imagePoints[coordinates[0]], imagePoints[coordinates[1]], imagePoints[coordinates[2]], imagePoints[coordinates[3]], imagePoints[coordinates[4]], imagePoints[coordinates[5]]} };

	for (int i = 0; i < 6; i++)
	{
		if (imagePoints[coordinates[i]].x > inputImage.cols || imagePoints[coordinates[i]].y > inputImage.rows || imagePoints[coordinates[i]].x < 0 || imagePoints[coordinates[i]].y < 0)
			return;
	}
	drawContours(inputImage, cnts, -1, Scalar(255, 255, 255), FILLED);

	for (int i = 0; i < cntsLine.size(); i++)
	{
		line(inputImage, cntsLine[i].first, cntsLine[i].second, Scalar(0, 255, 0), 1);
	}
}

void drawCube(Mat inputImage, Mat cameraMatrix, Mat distCoeffs, Vec3d rvecs, Vec3d tvecs, float len)
{
	vector<Point2f> imagePoints;
	vector<Point3f> pointWorld(8, Point3d(0, 0, 0));
	pointWorld[0] = Point3d(len / 2, len / 2, 0);
	pointWorld[1] = Point3d(-len / 2, len / 2, 0);
	pointWorld[2] = Point3d(-len / 2, -len / 2, 0);
	pointWorld[3] = Point3d(len / 2, -len / 2, 0);

	pointWorld[4] = Point3d(len / 2, len / 2, len);
	pointWorld[5] = Point3d(-len / 2, len / 2, len);
	pointWorld[6] = Point3d(-len / 2, -len / 2, len);
	pointWorld[7] = Point3d(len / 2, -len / 2, len);

	projectPoints(pointWorld, rvecs, tvecs, cameraMatrix, distCoeffs, imagePoints);

	

	//for(int i=0; i<8; i++)
	//	putText(inputImage, std::to_string(i), imagePoints[i], FONT_HERSHEY_SIMPLEX, 4, (255, 0, 0), 1, LINE_8);

	vector<pair<Point, Point>> cntsLine;
	switch (godPleaseShowMeWhatSideCubeIs(imagePoints))
	{
	case 0:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[2], imagePoints[1]},{imagePoints[1], imagePoints[0]},
			{imagePoints[2], imagePoints[6]},{imagePoints[1], imagePoints[5]},
			{imagePoints[0], imagePoints[4]},

			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]},
			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]} };

		drawWunderContours(inputImage, { 2, 1, 0, 4, 7, 6 }, imagePoints, cntsLine);
		break;
	case 1:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[2], imagePoints[3]},{imagePoints[3], imagePoints[0]},
			{imagePoints[2], imagePoints[6]},{imagePoints[3], imagePoints[7]},
			{imagePoints[0], imagePoints[4]},

			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]},
			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]} };
		drawWunderContours(inputImage, { 2, 3, 0, 4, 5, 6 }, imagePoints, cntsLine);
		break;
	case 2:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[0], imagePoints[1]},{imagePoints[3], imagePoints[0]},
			{imagePoints[1], imagePoints[5]},{imagePoints[0], imagePoints[4]},
			{imagePoints[3], imagePoints[7]},

			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]},
			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]} };

		drawWunderContours(inputImage, { 1, 0, 3, 7, 6, 5 }, imagePoints, cntsLine);
		break;
	case 3:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[2], imagePoints[1]},{imagePoints[2], imagePoints[3]},
			{imagePoints[2], imagePoints[6]},{imagePoints[1], imagePoints[5]},
			{imagePoints[3], imagePoints[7]},

			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]},
			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]} };
		drawWunderContours(inputImage, { 1, 2, 3, 7, 4, 5 }, imagePoints, cntsLine);
		break;

	case 4:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[0], imagePoints[3]},{imagePoints[3], imagePoints[2]},
			{imagePoints[0], imagePoints[4]},{imagePoints[3], imagePoints[7]},
			{imagePoints[2], imagePoints[6]},

			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]},
			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]} };

		drawWunderContours(inputImage, { 0, 3, 2, 6, 5, 4 }, imagePoints, cntsLine);
		break;
	case 5:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[0], imagePoints[1]},{imagePoints[1], imagePoints[2]},
			{imagePoints[0], imagePoints[4]},{imagePoints[1], imagePoints[5]},
			{imagePoints[2], imagePoints[6]},

			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]},
			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]} };
		drawWunderContours(inputImage, { 0, 1, 2, 6, 7, 4 }, imagePoints, cntsLine);
		break;
	case 6:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[2], imagePoints[3]},{imagePoints[1], imagePoints[2]},
			{imagePoints[3], imagePoints[7]},{imagePoints[2], imagePoints[6]},
			{imagePoints[1], imagePoints[5]},

			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]},
			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]} };

		drawWunderContours(inputImage, { 3, 2, 1, 5, 4, 7 }, imagePoints, cntsLine);
		break;
	case 7:
		//¬ундер-контура
		cntsLine = {
			{imagePoints[0], imagePoints[3]},{imagePoints[0], imagePoints[1]},
			{imagePoints[0], imagePoints[4]},{imagePoints[3], imagePoints[7]},
			{imagePoints[1], imagePoints[5]},

			{imagePoints[6], imagePoints[5]},{imagePoints[5], imagePoints[4]},
			{imagePoints[4], imagePoints[7]},{imagePoints[7], imagePoints[6]} };
		drawWunderContours(inputImage, { 3, 0, 1, 5, 6, 7 }, imagePoints, cntsLine);
		break;
	default:
		// красные линии
		for (int i = 0; i < 8; i += 2)
			line(inputImage, imagePoints[i], imagePoints[i + 1], Scalar(0, 0, 255), 1);

		// зеленые линии
		line(inputImage, imagePoints[1], imagePoints[2], Scalar(0, 255, 0), 1);
		line(inputImage, imagePoints[3], imagePoints[0], Scalar(0, 255, 0), 1);
		line(inputImage, imagePoints[5], imagePoints[6], Scalar(0, 255, 0), 1);
		line(inputImage, imagePoints[7], imagePoints[4], Scalar(0, 255, 0), 1);

		// синии линии
		for (int i = 0; i < 4; i++)
			line(inputImage, imagePoints[i], imagePoints[i + 4], Scalar(255, 0, 0), 1);
		break;
	}
}

void DrawCube(Ptr<aruco::Dictionary> dictionary, Mat cameraMatrix, Mat distCoeffs)
{
	// ќтрисовка куба на маркере
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
	cap.set(CAP_PROP_FOURCC, fourcc);

	Mat input_image;

	// переменна€ дл€ поворота кубов
	double s = 0.1;

	while (waitKey(1) != 27)
	{
		cap.read(input_image);
		if (!input_image.empty())
		{
			vector<vector<Point2f>> markerCorners, rejectedCandidates;
			Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
			vector<int> markerIds;
			detectMarkers(input_image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
			Mat outputImage = input_image.clone();
			// если обнаружен хот€ бы один маркер
			if (markerIds.size() > 0)
			{
				aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
				vector<Vec3d> rvecs1, tvecs1;
				aruco::estimatePoseSingleMarkers(markerCorners, 0.01, cameraMatrix, distCoeffs, rvecs1, tvecs1);
				s += 0.01;

				// рисуем кубы
				for (int i = 0; i < markerIds.size(); i++)
					drawCube(outputImage, cameraMatrix, distCoeffs, rvecs1[i], tvecs1[i], 0.01);
			}
			imshow("output", outputImage);
		}
	}
}