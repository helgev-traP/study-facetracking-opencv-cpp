#include <iostream>
#include <string>
// container
#include <vector>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#define ll long long

int main()
{

    // デバイスのオープン
    cv::VideoCapture cap(0);

    // カメラデバイスが正常にオープンしたか確認．
    if (!cap.isOpened())
    {
        // 読み込みに失敗したときの処理
        return -1;
    }

    // 取得したフレーム
    cv::Mat frame;

    // カスケード分類器格納場所
    cv::CascadeClassifier cascade;

    // 正面顔情報が入っているカスケード
    cascade.load("C:/pl-lib/c++/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");

    // 輪郭情報を格納場所
    std::vector<cv::Rect> faces;
    std::cout<<"reached\n";

    // 無限ループ
    while (1)
    {
        // USBカメラが得た動画の１フレームを格納
        cap >> frame;

        // 格納されたフレームに対してカスケードファイルに基づいて顔を検知
        cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(20, 20));

        // 検出した顔の個数"faces.size()"分ループを行う
        for (int i = 0; i < faces.size(); i++)
        {
            // 検出した顔を赤色矩形で囲む
            rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0, 0, 255), 3);
        }

        // 画像を表示．
        imshow("win", frame);

        // キーボード入力を受け付ける
        const int key = cv::waitKey(1);

        // qボタンが押されたとき
        if (key == 'q' /*113*/)
        {
            break; // whileループから抜ける．
        }
    }
    // すべてのウィンドウを閉じる
    cv::destroyAllWindows();

    return 0;
}