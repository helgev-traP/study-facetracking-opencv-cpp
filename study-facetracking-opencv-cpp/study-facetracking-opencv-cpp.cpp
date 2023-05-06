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
    };

    // 本体
    class Data
    {
    private:
        cv::CascadeClassifier cascade;

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
            Setted ret;
            // カスケード
            std::vector<cv::Rect> faces;
            cascade.detectMultiScale(img, faces, 1.1, 3, 0, cv::Size(20, 20));
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

                cv::rectangle(img, cv::Point(faces[isLargest].x, faces[isLargest].y),
                              cv::Point(faces[isLargest].x + faces[isLargest].width,
                                        faces[isLargest].y + faces[isLargest].height),
                              cv::Scalar(0, 0, 255), 3);
                // 返り値 errorは放置
                ret.isSetted = 0;
                ret.image = img;
                ret.position.x = (faces[isLargest].x + faces[isLargest].width / 2.0) - cam.width / 2.0;
                ret.position.y = (faces[isLargest].y + faces[isLargest].height / 2.0) - cam.height / 2.0;
                ret.position.width = faces[isLargest].width;
                ret.position.height = faces[isLargest].height;
            }
            else
            {
                // 何もなかったとき
                // 返り値 positionは放置
                ret.isSetted = -1;
                if (isLargest == -1)
                {
                    ret.error = "nothing detected";
                }
                else
                {
                    ret.error = "unknown error";
                }
                ret.image = img;
            }
            // camera_infoに書き込む
            camera_info.setCameraInfo(cam.width, cam.height, cam.view_angle);
            camera_info.setPar(std_distance, faces[isLargest].width + faces[isLargest].height);
            return ret;
        }

        Position getPosition() {}
    };

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

}

namespace dp = DetectHeadPosition;

int main()
{

    cv::destroyAllWindows();

    return 0;
}