#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
// container
#include <vector>
// math
#include <cmath>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using std::cin;
using std::cout;
using std::endl;

#define ll long long

// --- struct

struct FaceCoordinates
{
    // struct
    struct CenterOfFace
    {
        int x;
        int y;
    };
    struct SizeOfFace
    {
        int width;
        int height;
    };
    // val
    CenterOfFace center;
    SizeOfFace size;
    double face_size()
    {
        return size.width + size.height;
    }
    // setter
    void set_coordes(int x, int y, int width, int height)
    {
        center.x = x + width / 2;
        center.y = y + height / 2;
        size.width = width;
        size.height = height;
    }
};

class CameraInfo
{
public:
    struct Camera
    {
        int width;
        int height;
        double angle_of_view;
    };
    Camera camera;
    // カメラの正面で、Ncm離したときの顔の大きさ(縦横の平均)
    double std_distance;
    double face_size;
    double dis_x_tan;

private:
    bool camera_info_setted = false;
    bool par_setted = false;

public:
    // setter
    void setCameraInfo(int width, int height, double angle_of_view)
    {
        camera.width = width;
        camera.height = height;
        camera.angle_of_view = angle_of_view;
        if (par_setted == true)
        {
            dis_x_tan = std_distance * std::tan(camera.angle_of_view);
        }
        camera_info_setted = true;
    }
    void setPar(double distance, double size)
    {
        std_distance = distance;
        face_size = size;
        if (camera_info_setted == true)
        {
            dis_x_tan = std_distance * std::tan(camera.angle_of_view);
        }
        par_setted = true;
    }
};

struct HeadPosition
{
    double x; // 水平 右側が正
    double y; // 鉛直 上が正
    double z; // 奥行
    // double distance; // 距離
};

// --- function

HeadPosition PolerCoordes(FaceCoordinates face, CameraInfo info)
// func 頭の位置検出
// 入力: FaceCoordinates, CameraInfo
// 出力: HeadPosition
{
    HeadPosition head_pos;
    head_pos.x = info.std_distance * info.dis_x_tan *
                 (face.center.x / info.camera.width) *
                 (info.face_size / face.face_size());
    head_pos.y = info.std_distance * info.dis_x_tan *
                 (face.center.y / info.camera.width) *
                 (info.face_size / face.face_size());
    head_pos.z = info.std_distance * (info.face_size / face.face_size());
    return head_pos;
}

int main()
{
    // *  almost constant
    int width = 1920;
    int height = 1080;
    double distance = 0.5;
    // *
    cv::VideoCapture cap(0);

    // カメラの異常について
    if (!cap.isOpened())
    {
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    cv::Mat frame;

    cv::CascadeClassifier cascade;
    cascade.load("../haarcascades/haarcascade_frontalface_alt2.xml");

    // カメラの情報と基本の位置での顔の大きさを記録しておく
    CameraInfo cam_info;
    cam_info.setCameraInfo(1920, 1080, M_PI / 2.0);

    while (1)
    {
        cap >> frame;
        // カスケード
        std::vector<cv::Rect> faces;
        cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));
        // 最大の検出をマークする
        int isLargest = -1;
        double max_size = 0;
        for (int i = 0; i < faces.size(); i++)
        {
            if (faces[i].width + faces[i].height > max_size)
            {
                max_size = faces[i].width + faces[i].height;
                isLargest = i;
            }
        }

        // カスケードから矩形描画
        if (isLargest != -1)
        {
            // ! バグテスト 後で消す
            if (isLargest >= faces.size() || isLargest < 0)
            {
                cout << "something go wrong at \"isLargest\"" << endl;
            }

            cv::rectangle(frame, cv::Point(faces[isLargest].x, faces[isLargest].y),
                          cv::Point(faces[isLargest].x + faces[isLargest].width,
                                    faces[isLargest].y + faces[isLargest].height),
                          cv::Scalar(0, 0, 255), 3);
        }

        // 描画
        cv::imshow("win", frame);
        // break
        const int key = cv::waitKey(33);
        if (key != -1)
        {
            if (isLargest != -1)
            {
                cam_info.setPar(distance, faces[isLargest].width + faces[isLargest].height);
                break;
            }
            else
            {
                cout << "try again" << endl;
            }
        }
    }

    // テスト
    while (1)
    {
        // 映像をとってくる
        cap >> frame;

        // カスケード
        std::vector<cv::Rect> faces;
        cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));

        // 最大の検出をマークする
        int isLargest = 0;
        double max_size = 0;
        for (int i = 0; i < faces.size(); i++)
        {
            if (faces[i].width + faces[i].height > max_size)
            {
                max_size = faces[i].width + faces[i].height;
                isLargest = i;
            }
        }

        // カスケードから矩形描画
        cv::rectangle(frame, cv::Point(faces[isLargest].x, faces[isLargest].y),
                      cv::Point(faces[isLargest].x + faces[isLargest].width,
                                faces[isLargest].y + faces[isLargest].height),
                      cv::Scalar(0, 0, 255), 3);

        // faces[isLargest]に対してカメラからの相対座標を計算する
        FaceCoordinates face_coords;
        face_coords.set_coordes(faces[isLargest].x, faces[isLargest].y,
                                faces[isLargest].width, faces[isLargest].height);
        HeadPosition head_pos = PolerCoordes(face_coords, cam_info);

        // 描画
        cv::imshow("win", frame);

        // break
        const int key = cv::waitKey(33);
        if (key == 'q' /*113*/)
        {
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}