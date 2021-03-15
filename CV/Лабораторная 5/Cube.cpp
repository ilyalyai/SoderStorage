#include "lab5.h"


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

	// красные линии 
	for (int i = 0; i < 8; i += 2)
	{
		line(inputImage, imagePoints[i], imagePoints[i + 1], Scalar(0, 0, 255), 1);
	}

	// зеленые линии 
	line(inputImage, imagePoints[1], imagePoints[2], Scalar(0, 255, 0), 1);
	line(inputImage, imagePoints[3], imagePoints[0], Scalar(0, 255, 0), 1);
	line(inputImage, imagePoints[5], imagePoints[6], Scalar(0, 255, 0), 1);
	line(inputImage, imagePoints[7], imagePoints[4], Scalar(0, 255, 0), 1);

	// синии линии
	for (int i = 0; i < 4; i++)
	{
		line(inputImage, imagePoints[i], imagePoints[i + 4], Scalar(255, 0, 0), 1);
	}
}

void DrawCube(Ptr<aruco::Dictionary> dictionary, Mat cameraMatrix, Mat distCoeffs)
{
	// Отрисовка куба на маркере
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
	cap.set(CAP_PROP_FOURCC, fourcc);

	Mat input_image;

	// переменная для поворота кубов
	double s = 0.1;

	while (waitKey(1) != 27)
	{
		cap.read(input_image);
		if (!input_image.empty())
		{
			vector<vector<Point2f>> markerCorners, rejectedCandidates;
			Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
			vector<int> markerIds;
			aruco::detectMarkers(input_image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
			Mat outputImage = input_image.clone();
			// если обнаружен хотя бы один маркер
			if (markerIds.size() > 0) 
			{
				aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
				vector<Vec3d> rvecs1, tvecs1;
				aruco::estimatePoseSingleMarkers(markerCorners, 0.01, cameraMatrix, distCoeffs, rvecs1, tvecs1);
				s += 0.01;
				// рисуем кубы
				for (int i = 0; i < markerIds.size(); i++)
				{
					Vec3d RX(0, 0, s);
					Vec3d TX(0, 0, 0);
					//aruco::drawAxis(outputImage, cameraMatrix, distCoeffs, rvecs1[i], tvecs1[i], 0.01);
					//composeRT(RX, TX, rvecs1[i], tvecs1[i], rvecs1[i], tvecs1[i]);
					drawCube(outputImage, cameraMatrix, distCoeffs, rvecs1[i], tvecs1[i], 0.01);
				}
			}
			imshow("output", outputImage);
		}
	}
}