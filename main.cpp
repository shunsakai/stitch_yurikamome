#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

#include <stdio.h>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

#ifdef _DEBUG
#pragma comment(lib,"opencv_world300d.lib")
#pragma comment(lib,"opencv_ts300d.lib")
#endif

#ifdef _WINDOWS
  #include <windows.h>
#else
  #include <unistd.h>
  #define Sleep(x) usleep((x)*1000)
#endif

using namespace cv;
using namespace std;

Mat white = imread("massiro.jpg");
Mat mask_front = imread("frontmask_0706.jpg", 0); // マスク画像読み込み 引数0はグレースケールで読み込み
Mat mask_back = imread("backmask_0706.jpg", 0); // マスク画像読み込み 引数0はグレースケールで読み込み
Mat mask_green = imread("mask_green.jpg", 0);
Mat mask_final = imread("mask_final.jpg", 0);

string INPUT_PATH_FRONT = "front_raw/";
string INPUT_PATH_BACK = "back_raw/";
string OUTPUT_PATH = "result/";
// frames count
int end_img_num = 6200;
 
void mask_stitch(int frame_no){
  //int型をstring型に変換
  int backno = frame_no + 23;
  int frontno = frame_no + 36;
  stringstream number;
  number << std::setw(5) << std::setfill('0') << frame_no;
  stringstream back_number;
  back_number << std::setw(4) << std::setfill('0') << backno;
  stringstream front_number;
  front_number << std::setw(9) << std::setfill('0') << frontno;

  // 切り抜きたい新規画像
  string FRONT_IMG_PATH =INPUT_PATH_FRONT + "front_raw" + front_number.str() + ".jpg";
  string BACK_IMG_PATH = INPUT_PATH_BACK + "back_raw" + back_number.str() + ".jpg";


  // 1,新規画像読み込み
  Mat image_front = imread(FRONT_IMG_PATH);
  Mat image_back = imread(BACK_IMG_PATH);
  Mat result;

  //マスクをかけつつfrontとbackの画像を合成
  image_front.copyTo(white, mask_front);
  image_back.copyTo(white, mask_back);

  //緑除去用のマスクをかける
  white.copyTo(white, mask_green);

  //inpaintと補間
  inpaint(white, mask_final, result, 3, INPAINT_TELEA);

  //保存
  imwrite(OUTPUT_PATH + "result" + number.str() + ".jpg", result);
}

int main(){
  //時間表示
  std::vector<int> v;
  auto start = std::chrono::system_clock::now();

  for(int i=1; i<end_img_num; i++){
    mask_stitch(i);
    printf("%05d\n", i);

    if((i % 50) == 0){
      Sleep(1000);
    }
  }

  auto end = std::chrono::system_clock::now(); // 計測終了時刻を保存
  auto dur = end - start; // 要した時間を計算
  auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  auto sec = msec/1000.0;
  printf("%f\n", sec);

  FILE *file;
  file = fopen("count.txt","w");
  fprintf(file, "%lld seconds!", sec);
  fclose(file);

  return 0;
}
