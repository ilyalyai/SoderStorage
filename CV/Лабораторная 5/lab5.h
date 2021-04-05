#pragma once
#include <iostream>
#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"
#include <conio.h>

using namespace cv;
using namespace std;

vector<Mat> CameraCalib();
void DrawCube(Ptr<aruco::Dictionary> dictionary, Mat cameraMatrix, Mat distCoeffs);
vector<Mat> readImgs(string filePath);
