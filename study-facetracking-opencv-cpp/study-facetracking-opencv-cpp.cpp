// func 関数の機能の説明
// ! 最終的に直されるべき箇所・注意
// * ほぼ定数(解像度に関する値・画角など)

#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <iomanip>
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

namespace DetectHeadPosition
{
    // 補助
    struct cam_status
    {
        int width;
        int height;
        double view_angle;
    };
    cam_status camera(int width, int height, double view_angle)
    {
        cam_status cam;
        cam.width = width;
        cam.height = height;
        cam.view_angle = view_angle;
        return cam;
    }
    // 関数の返り値として使う構造体
    struct Setted
    {
        int isSetted;
        std::string error = "";
        struct pos
        {
            int x;
            int y;
            int width;
            int height;
        };
        pos position;
        cv::Mat image;
    };
    struct Position
    {
        int isDetected;
        std::string error = "";
        struct pos
        {
            double x;
            double y;
            double z;
            double distance;
        };
        pos position;
        cv::Mat image;
    };

    // 本体
    class Data
    {
    private:
        // class and struct
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
            double x;        // 水平 右側が正
            double y;        // 鉛直 上が正
            double z;        // 奥行
            double distance; // 距離
        };

        // function
        HeadPosition DescartesCoordes(FaceCoordinates face, CameraInfo info)
        // func 頭の位置検出
        // 入力: FaceCoordinates, CameraInfo
        // 出力: HeadPosition
        {
            HeadPosition head_pos;
            head_pos.x = info.dis_x_tan *
                         (double(face.center.x) / double(info.camera.width)) *
                         (info.face_size / face.face_size());
            head_pos.y = info.dis_x_tan *
                         (double(face.center.y) / double(info.camera.width)) *
                         (info.face_size / face.face_size());
            head_pos.z = info.std_distance *
                         (info.face_size / face.face_size());
            head_pos.distance = std::sqrt(std::pow(head_pos.x, 2) +
                                          std::pow(head_pos.y, 2) +
                                          std::pow(head_pos.z, 2));
            return head_pos;
        }

        std::vector<cv::Rect> FastCascade(cv::Mat img, double magnification)
        {
            // reshape
            cv::Mat small_img;
            cv::resize(img, small_img, cv::Size(), magnification, magnification);
            // cascade
            std::vector<cv::Rect> faces;
            cascade.detectMultiScale(small_img, faces, 1.1, 3, 0, cv::Size(20, 20));
            // fix Rect magnification
            for (int i = 0; i < faces.size(); i++)
            {
                faces[i].x /= magnification;
                faces[i].y /= magnification;
                faces[i].width /= magnification;
                faces[i].height /= magnification;
            }

            return faces;
        }

        // vallue
        cv::CascadeClassifier cascade;
        FaceCoordinates face_coordinates;
        CameraInfo camera_info;
        HeadPosition head_position;

    public:
        void setCascade(std::string path)
        {
            cascade.load(path);
        }

        Setted setStdPosition(cv::Mat img, double std_distance, cam_status cam)
        {
            // 返り値
            Setted return_setted;
            // カスケード
            std::vector<cv::Rect> faces = FastCascade(img, 0.5);
            // 最大の検出をマーク
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
            // 分岐

            if (isLargest != -1)
            {
                // 検出があるとき
                // ! バグテスト 後で消す
                if (isLargest >= faces.size() || isLargest < 0)
                {
                    cout << "something go wrong at \"isLargest\"" << endl;
                }
                // !
                cv::rectangle(img, cv::Point(faces[isLargest].x, faces[isLargest].y),
                              cv::Point(faces[isLargest].x + faces[isLargest].width,
                                        faces[isLargest].y + faces[isLargest].height),
                              cv::Scalar(0, 0, 255), 3);
                // camera_infoに書き込む
                camera_info.setCameraInfo(cam.width, cam.height, cam.view_angle);
                camera_info.setPar(std_distance, faces[isLargest].width + faces[isLargest].height);
                // 返り値 errorは放置
                return_setted.isSetted = 0;
                return_setted.image = img;
                return_setted.position.x = (faces[isLargest].x + faces[isLargest].width / 2.0) - cam.width / 2.0;
                return_setted.position.y = (faces[isLargest].y + faces[isLargest].height / 2.0) - cam.height / 2.0;
                return_setted.position.width = faces[isLargest].width;
                return_setted.position.height = faces[isLargest].height;
            }
            else
            {
                // 何もなかったとき
                // 返り値
                return_setted.isSetted = -1;
                if (isLargest == -1)
                {
                    return_setted.error = "nothing detected";
                }
                else
                {
                    return_setted.error = "unknown error";
                }
                return_setted.image = img;
            }
            return return_setted;
        }

        Position getPosition(cv::Mat img)
        {
            // 返り値
            Position return_position;
            // カスケード
            std::vector<cv::Rect> faces = FastCascade(img, 0.5);

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

            if (isLargest != -1)
            {
                // カスケードから矩形描画
                cv::rectangle(img, cv::Point(faces[isLargest].x, faces[isLargest].y),
                              cv::Point(faces[isLargest].x + faces[isLargest].width,
                                        faces[isLargest].y + faces[isLargest].height),
                              cv::Scalar(0, 0, 255), 3);

                // faces[isLargest]に対してカメラからの相対座標を計算する
                FaceCoordinates face_coords;
                face_coords.set_coordes(faces[isLargest].x - camera_info.camera.width / 2.0,
                                        faces[isLargest].y - camera_info.camera.height / 2.0,
                                        faces[isLargest].width, faces[isLargest].height);
                HeadPosition head_pos = DescartesCoordes(face_coords, camera_info);

                return_position.isDetected = 0;
                return_position.image = img;
                return_position.position.x = head_pos.x;
                return_position.position.y = head_pos.y;
                return_position.position.z = head_pos.z;
                return_position.position.distance = head_pos.distance;
            }
            else
            {
                return_position.isDetected = -1;
                if (isLargest == -1)
                {
                    return_position.error = "nothing detected";
                }
                else
                {
                    return_position.error = "unknown error";
                }
                return_position.image = img;
            }
            return return_position;
        }
    };
}

namespace dp = DetectHeadPosition;

int main()
{
    // * almost const
    int width = 1920;
    int height = 1080;
    double distance = 0.5;
    double view_angle = M_PI / 4.0;
    // *
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    dp::Data testData;

    testData.setCascade("../haarcascades/haarcascade_frontalface_alt2.xml");

    cv::Mat frame;
    while (1)
    {
        cap >> frame;
        dp::Setted catch_result = testData.setStdPosition(frame, distance, dp::camera(width, height, view_angle));

        cv::imshow("win", catch_result.image);

        const int key = cv::waitKey(1);
        if (key != -1)
        {
            if (catch_result.isSetted == 0)
                break;
            else
                cout << "try again" << endl;
        }
    }

    cv::destroyAllWindows();

    return 0;
}