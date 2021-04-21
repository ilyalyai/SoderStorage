#include "lab7_lib.h"

void TossACoin()
{
	Mat image = imread("monetki_0.jpg");
	imshow("IN", image);
	Mat gray, afterErodeAndDilate;

	cvtColor(image, gray, COLOR_BGR2GRAY);
	threshold(gray, gray, 170, 255, THRESH_BINARY);
	erode(gray, afterErodeAndDilate, getStructuringElement(MORPH_ELLIPSE, Size(1,1)));
	dilate(afterErodeAndDilate, afterErodeAndDilate, getStructuringElement(MORPH_ELLIPSE, Size(2,2)));

	vector<Vec3f> circles;
	HoughCircles(afterErodeAndDilate, circles, HOUGH_GRADIENT, 1, afterErodeAndDilate.cols / 5.5, 200, 20, 5, 100);

	cout << circles.size();

	// Вектор картинок с найденными монетками
	vector<Mat> coins;
	for (int i = 0; i < circles.size(); i++)
	{
		//Берем найденный круг и определяем его параметры
		Vec3f c = circles[i];
		Point center = Point(c[0], c[1]);
		int radius = c[2];

		Size size(radius * 2 + 20, radius * 2 + 20);
		Scalar black(0, 0, 0);

		Rect rectangle(center.x - radius, center.y - radius, radius * 2, radius * 2);
		Mat outputImageWithCoin(image, rectangle);

		coins.push_back(outputImageWithCoin);
	}

	// Шаблоны
	Mat copper = imread("copper.jpg");
	imshow("copper", copper);
	Mat nikel = imread("nikel.jpg");
	imshow("nikel", nikel);

	Mat hsv_copper, hsv_nikel;
	cvtColor(copper, hsv_copper, COLOR_BGR2HSV);
	cvtColor(nikel, hsv_nikel, COLOR_BGR2HSV);
	
	// Вектор картинок с монетками в пространстве HSV
	vector<Mat> coins_HSV;
	for (int i = 0; i < coins.size(); i++)
	{
		Mat temp_HSV;
		cvtColor(coins[i], temp_HSV, COLOR_BGR2HSV);
		coins_HSV.push_back(temp_HSV);
	}

	// Параметры для поиска гистограмм
	int h_bins = 180, s_bins = 256, v_bins = 256;
	int histSize[] = { h_bins, s_bins, v_bins };

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	float v_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges, v_ranges };

	int channels[] = { 0, 1, 2 };

	// Вычисление гистограмм монет
	vector<Mat> coins_hist;
	for (size_t i = 0; i < coins_HSV.size(); i++)
	{
		Mat temp_hist;
		calcHist(&coins_HSV[i], 1, channels, Mat(), temp_hist, 2, histSize, ranges, true, false);
		normalize(temp_hist, temp_hist, 0, 1, NORM_MINMAX, -1, Mat());
		coins_hist.push_back(temp_hist);
	}

	Mat hist_copper, hist_nikel;
	calcHist(&hsv_copper, 1, channels, Mat(), hist_copper, 2, histSize, ranges, true, false);
	normalize(hist_copper, hist_copper, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_nikel, 1, channels, Mat(), hist_nikel, 2, histSize, ranges, true, false);
	normalize(hist_nikel, hist_nikel, 0, 1, NORM_MINMAX, -1, Mat());


	for (size_t i = 0; i < coins_hist.size(); i++)
	{
		double coin_copper = compareHist(coins_hist[i], hist_copper, HISTCMP_INTERSECT);
		double coin_nikel = compareHist(coins_hist[i], hist_nikel, HISTCMP_INTERSECT);
		if (coin_copper > coin_nikel)
		{
			Vec3f c = circles[i];
			Point center = Point(c[0], c[1]);
			int radius = c[2];
			circle(image, center, radius, Scalar(0, 0, 120), 2, 16);
		}
		else
		{
			Vec3f c = circles[i];
			Point center = Point(c[0], c[1]);
			int radius = c[2];
			circle(image, center, radius, Scalar(120, 0, 0), 2, 16);
		}
	}

	imshow("OUT", image);
	while (waitKey(1) != 27){}
}