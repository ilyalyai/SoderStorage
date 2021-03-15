#include "lab5.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	// создание доски аруко
	Ptr<aruco::Dictionary> dictionary = getPredefinedDictionary(aruco::DICT_6X6_250);
	Ptr<aruco::GridBoard> board = aruco::GridBoard::create(5, 7, 0.028, 0.007, dictionary);
	Mat boardImage;
	board->draw(Size(500, 600), boardImage, 10, 1);
	//imwrite("boardImage.png", boardImage);

	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();

	// создание и заполнение вектора иображений
	vector<Mat> imgs;
	imgs = readImgs("C://Users//Ilya//source//repos//lab5_CV//Photo_Andre");

	// калибровка
	vector<vector<vector<Point2f>>> allCorners;
	vector<vector<int>>allIds;
	Size imgSize;

	//заполнение векторов allCorners и allIds
	for (int i = 0; i < imgs.size(); i++)
	{
		vector< int > ids;
		vector< vector< Point2f > > corners, rejected;
		// detect markers
		aruco::detectMarkers(imgs[i], dictionary, corners, ids, parameters, rejected);
		allCorners.push_back(corners);
		allIds.push_back(ids);
		imgSize = imgs[i].size();
	}

	vector<vector<Point2f>> allCornersConcatenated, rejectedCandidates, imgpts;
	vector<int> allIdsConcatenated;
	vector<int> markerCounterPerFrame;
	markerCounterPerFrame.reserve(allCorners.size());

	for (int i = 0; i < allCorners.size(); i++)
	{
		markerCounterPerFrame.push_back((int)allCorners[i].size());
		for (int j = 0; j < allCorners[i].size(); j++)
		{
			allCornersConcatenated.push_back(allCorners[i][j]);
			allIdsConcatenated.push_back(allIds[i][j]);
		}
	}

	Mat cameraMatrix, distCoeffs;
	vector<Mat> rvecs, tvecs;
	double arucoRepErr;
	arucoRepErr = calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
		markerCounterPerFrame, board, imgSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, 0);

	ofstream resultAruco;
	resultAruco.open("C://Users//Ilya//source//repos//lab5_CV////lab5_CV//cameraMatrixAruco.txt", ios::out);
	resultAruco << cameraMatrix;
	resultAruco.close();
	
	vector<Mat> resultCalib = CameraCalib();
	
	system("cls");
	while(1)
	{
		cout << "Выберите задание (от 0 до 3, 4- выход):" << endl;
		char b = _getch();
		switch (b)
		{
			case '0':
				imshow("boardImage", boardImage);
				while (waitKey(1) != 27){}
				destroyAllWindows();
				break;
			case '1':
				cout << "cameraMatrix : " << cameraMatrix << endl;
				cout << arucoRepErr << endl;
				break;
			case '2':
				cout << "cameraMatrix : " << resultCalib[0] << endl;
				cout << "distCoeffs : " << resultCalib[1] << endl;
				cout << "Rotation vector : " << resultCalib[2] << endl;
				cout << "Translation vector : " << resultCalib[3] << endl;
				break;
			case '3':
				DrawCube(dictionary, cameraMatrix, distCoeffs);
				break;
			case '4':
				return 0;
			default: 
				break;
		}
	}
}