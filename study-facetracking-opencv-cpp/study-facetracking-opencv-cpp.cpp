#include <iostream>
#include <string>
// container
#include <vector>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using std::cin;
using std::cout;

#define ll long long

int main()
{
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        return -1;
    }

    cv::Mat frame;

    cap >> frame;

    cv::CascadeClassifier cascade;

    cascade.load("C:/pl-lib/C++/opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml");

    std::vector<cv::Rect> faces;

    cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));

    for (int i = 0; i < faces.size(); i++)
    {
        cv::rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0, 0, 255), 3);
    }

    cv::imshow("win", frame);

    return 0;
}