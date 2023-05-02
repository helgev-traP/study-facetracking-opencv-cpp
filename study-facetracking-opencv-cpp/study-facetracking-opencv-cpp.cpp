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

// --- struct

struct face_coordinates
{
    // struct
    struct center_of_face
    {
        int x;
        int y;
    };
    struct size_of_face
    {
        int width;
        int height;
    };
    // val
    center_of_face center;
    size_of_face size;
    double face_size()
    {
        return (size.width + size.height) / 2.0;
    }
};

struct camera_info
{
    struct camera
    {
        double width;
        double height;
    };

    // カメラの正面で、Ncm離したときの顔の大きさ
    double face_size;
};

struct head_position
{
    double horizontal_angle;
    double vertical_angle;
    double distance;
};

// --- function

head_position poler_coordes(face_coordinates face, camera_info info)
// 入力: face_coordinates, camera_info
// 出力: (正面を0とし、カメラから右側・上側を正として) 横の角度, 縦の角度, 距離
{
    head_position pass;
    return pass;
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
    cascade.load("../haarcascades/haarcascade_frontalface_alt2.xml");

    // カメラの情報と基本の位置での顔の大きさを記録しておく
    camera_info cam_info;

    // 無限ループ
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
        face_coordinates face_coords;
        face_coords.center.x = faces[0].x + faces[0].width / 2;
        face_coords.center.y = faces[0].y + faces[0].height / 2;
        face_coords.size.width = faces[0].width;
        face_coords.size.height = faces[0].height;
        head_position head_pos = poler_coordes(face_coords, cam_info);

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