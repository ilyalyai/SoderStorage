#include "lab5.h"

// считывание изображений
vector<Mat> readImgs(string filePath)
{
	vector<cv::String> fn;
	glob(filePath, fn, true);
	vector<Mat> imgs(fn.size());
	for (uint i = 0; i < fn.size(); i++)
	{
		imgs[i] = imread(fn[i]);
	}
	return imgs;
}

vector<Mat> CameraCalib()
{
	// создание и заполнение вектора иображений
	vector<Mat> imgsChess;
	imgsChess = readImgs("C://Users//Ilya//source//repos//lab5_CV//Photo_matlab");
	if (imgsChess.empty())
	{
		cout << "НЕ НАЙДЕНЫ ИЗОБРАЖЕНИЯ!";
		return {};
	}

	// размеры шахматной доски
	int CHECKERBOARD[2]{ 6, 9 };

	// создание вектора из векторов 3D точек для каждого изображения
	vector<vector<Point3f> > objpoints;

	// создание вектора из векторов 2D точек для каждого изображения
	vector<vector<Point2f> > imgpoints;

	// определение мировых координат 3D точек
	vector<Point3f> objp;
	for (int i = 0; i < CHECKERBOARD[1]; i++)
	{
		for (int j = 0; j < CHECKERBOARD[0]; j++)
			objp.emplace_back(j, i, 0);
	}

	Mat gray;
	// вектор для хранения пиксельных координат обнаруженных углов шахматной доски
	vector<Point2f> corner_pts;
	bool success;

	for (int i = 0; i < imgsChess.size(); i++)
	{
		cvtColor(imgsChess[i], gray, COLOR_BGR2GRAY);

		// нахождение углов на шахматной доске
		success = findChessboardCorners(gray, Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

		if (success)
		{
			TermCriteria criteria(TermCriteria::Type::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);

			// уточнение координат пикселей для заданных 2d-точек
			cornerSubPix(gray, corner_pts, Size(11, 11), cv::Size(-1, -1), criteria);

			objpoints.push_back(objp);
			imgpoints.push_back(corner_pts);
		}
	}

	Mat cameraMatrixChess, distCoeffsChess, R, T;

	calibrateCamera(objpoints, imgpoints, Size(gray.rows, gray.cols), cameraMatrixChess, distCoeffsChess, R, T);

	ofstream resultChessBoard;
	resultChessBoard.open("C://Users//Ilya//source//repos//lab5_CV//lab5_CV//cameraMatrixChessBoard.txt", ios::out);
	resultChessBoard << cameraMatrixChess;
	resultChessBoard.close();

	return { cameraMatrixChess, distCoeffsChess, R, T };
}