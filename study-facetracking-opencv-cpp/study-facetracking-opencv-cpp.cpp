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

class face_placeSize
{
public:
    class centor
    {
        int x;
        int y;
    };
    class size
    {
        int width;
        int height;
    }
};

class camera_info
{
public:
    class camera
    {
        double width;
        double height;
    };

    // カメラの正面で、Ncm離したときの顔の大きさ
    double face_size;
};

std::vector<double>
poler_coordes(std::vector<int> centor, std::vector<int> size, camera_info data)
// 入力: 顔の中心の画像上の座標, サイズ, ,
// 出力: (正面を0とし、カメラから右側・上側を正として) 横の角度, 縦の角度, 距離
{
}

int main()
{
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        return -1;
    }

    cv::Mat frame;

    cv::CascadeClassifier cascade;
    cascade.load("../haarcascades/haarcascade_eye_tree_eyeglasses.xml");

    while (1)
    {
        // 映像をとってくる
        cap >> frame;
        // カスケード
        std::vector<cv::Rect> faces;
        cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));
        // カスケードから矩形描画
        for (int i = 0; i < faces.size(); i++)
        {
            cv::rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0, 0, 255), 3);
        }
        // faces[0]に対してカメラからの相対座標を計算する
        std::vector<double> coordes = poler_coordes({faces[0].x + faces[0].width / 2, faces[0].y + faces[0].height}, {faces[0].width, faces[0].height});
        // 描画
        cv::imshow("win", frame);
        // break
        const int key = cv::waitKey(1);
        if (key == 'q' /*113*/)
        {
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}