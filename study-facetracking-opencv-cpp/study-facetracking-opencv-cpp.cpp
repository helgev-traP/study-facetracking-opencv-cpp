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

    while (1)
    {
        cap >> frame;
        cv::imshow("cap", frame);

        const int key = cv::waitKey(1);
        if (key == 'q')
        {
            break;
        }
    }
    cv::destroyAllWindows();
    return 0;
}