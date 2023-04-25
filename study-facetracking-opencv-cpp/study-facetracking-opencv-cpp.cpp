#include <iostream>
#include <string>
// container
#include <vector>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using std::cin;
using std::cout;
using std::endl;

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

    try
    {
        cascade.load("C:/pl-lib/Cpp/opencv/build/etc/haarcascades/haarcascade_frontalface_alt.xml");
    }
    catch (cv::Exception &e)
    {
        const char *err_msg = e.what();
        cout << "\n-----------\n"
             << err_msg << "\n-----------\n"
             << endl;
    }
    std::vector<cv::Rect> faces;
    try
    {
        cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));
    }
    catch (cv::Exception &e)
    {
        const char *err_msg = e.what();
        cout << "\n-----------\n"
             << err_msg << "\n-----------\n"
             << endl;
    }

    try
    {
        for (int i = 0; i < faces.size(); i++)
        {
            cv::rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0, 0, 255), 3);
        }
    }
    catch (cv::Exception &e)
    {
        const char *err_msg = e.what();
        cout << "\n-----------\n"
             << err_msg << "\n-----------\n"
             << endl;
    }

    cv::imshow("win", frame);

    return 0;
}