#define _USE_MATH_DEFINES

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

//部屋の広さ コアルーム 8.41 * 6.59
#define ROOM_H 6.59 //変更
#define ROOM_W 8.41 //
#define ROOM_CX 16.68
#define ROOM_CY 13.58

//WINDOW SIZE
#define WINDOW_H (ROOM_H * 100)
#define WINDOW_W (ROOM_W * 100)

//映像サイズ memo
//#define RICOH_W 3860
//#define RICOH_H 1830


#define LineMax 30

//カメラの設置の高さ(m) カメラ#1 左下0,0
#define c_z_1 2.5  //高さ
#define c_y_1 0.74  //縦
#define c_x_1 6.02 //横
#define c_r_1 270.0

//カメラの設置の高さ(m) カメラ#2 左下0,0
#define c_z_2 2.5  //高さ
#define c_y_2 5.57 //縦
//#define c_x_2 3.49 //横
#define c_x_2 4.49 //横
#define c_r_2 90.0


//#define deg_to_rad(deg) (((deg)/360)*2*M_PI)
#define deg_to_rad(deg) deg *M_PI / 180

#define p 14 //同時に解析する人数(仮)

double room(double width, double height1)
{
	double room = (ROOM_W / ROOM_H) * (WINDOW_H * 0.6);
	return room;
}

double deg_trans(double rad)
{
	if (rad < 0)rad = ((int)rad % 360) + 360.0;
	return rad;
}

double deg_adj_c1(double deg) {
	deg = c_r_1 - deg;
	return deg;
}

double deg_adj_c2(double deg) {
	//deg = deg + c_r_2;
	deg = c_r_2 - deg;
	if (deg < 0)deg = 360 + deg;
	return deg;
}

double camera1(double leftx_1[p], double rightx_1[p], double height1[p], double leftx_2[p], double rightx_2[p], double height[p], double room[14][18]) {

	double left_deg1 = 0.0, left_deg2 = 0.0;
	double right_deg1 = 0.0, right_deg2 = 0.0;
	double left_rad1 = 0.0, left_rad2 = 0.0;
	double right_rad1 = 0.0, right_rad2 = 0.0;
	double room_c1[14][18] = {}, room_c2[14][18] = {};
	int point_c1 = 0, point_c2 = 0;
	int point_c1r = 0, point_c2r = 0;

	// カメラの角度情報が入った配列を見ていく
	for (int n = 0; leftx_1[n] != NULL; n++) {

		/*camera 1*/
		left_deg1 = deg_adj_c1((leftx_1[n] / 3860.0 )* 360.0 );
		right_deg1 = deg_adj_c1((rightx_1[n] / 3860.0 )* 360.0 );

		left_rad1 = tan(deg_to_rad(left_deg1));
		right_rad1 = tan(deg_to_rad(right_deg1));

		point_c1 = 0.0;

		//printf("カメラ１確認用\n");
		//printf("leftx_1[n] = %lf\n", leftx_1[n]);
		//printf("rightx_1[n] = %lf\n", rightx_1[n]);

		//printf("left_deg1 = %lf\n", left_deg1);
		//printf("right_deg1 = %lf\n", right_deg1);

		//printf("left_rad1 = %lf\n", left_rad1);
		//printf("right_rad1 = %lf\n", right_rad1);


		// 確率密度関数に入れていく　17*13
		for (int i = 0; i < 17; i++) {
			//point_c1 = (int)((left_rad1 * ((double)i*0.5)) + (c_x_1 - left_rad1 * c_y_1))*2;
			point_c1 = (int)(left_rad1 * ((double)i * 0.5) + (c_y_1 - c_x_1 * left_rad1)) * 2;
			point_c1 = 13 - point_c1;

			point_c1r = (int)(right_rad1 * ((double)i * 0.5) + (c_y_1 - c_x_1 * right_rad1)) * 2;
			point_c1r = 13 - point_c1r;

			/*if (0 <= point_c1 && point_c1 <= 13) {
				printf("point c1 = %d\n", point_c1);
				printf("leftx_1 = %2.2f \n", leftx_1[n]);
			}*/
			if (0 <= point_c1 && point_c1 <= 13) {
				room_c1[point_c1][i] = (room_c1[point_c1][i] + 1.0) / 2.0;
			}
			else {
				for (int j = 0; j < 13; j++) {
					room_c1[j][i] = room_c1[j][i] / 2.0;
				}
			}

			if (0 <= point_c1r && point_c1r <= 13) {
				room_c1[point_c1r][i] = (room_c1[point_c1r][i] + 1.0) / 2.0;
			}
			else {
				/*for (int j = 0; j < 13; j++) {
					room_c1[j][i] = room_c1[j][i] / 2.0;
				}*/
			}

			//printf("test = %d\n",point_c1);
		}


		/*隣接するセルに値が入っていたら1/2の値を入れる*/
		for (int j = 0; j < 17;j++) {
			for (int k = 0; k < 13;k++) {
				if (room_c1[k][j] != 0.0 && k-1 > 0 && room_c1[k-1][j] == 0.0) {
					room_c1[k - 1][j] = ( room_c1[k][j] + room_c1[k-1][j]) / 2.0;
				}
				if (room_c1[k][j] != 0.0 && j - 1 > 0 && room_c1[k][j-1] == 0.0) {
					room_c1[k][j-1] = (room_c1[k][j] + room_c1[k][j-1]) / 2.0;
				}
				if (room_c1[k][j] != 0.0 && k + 1 < 13 && room_c1[k+1][j] == 0.0) {
					room_c1[k + 1][j] = (room_c1[k][j] + room_c1[k + 1][j]) / 2.0;
				}
				if (room_c1[k][j] != 0.0 && j + 1 < 17 && room_c1[k][j+1] == 0.0) {
					room_c1[k][j + 1] = (room_c1[k][j] + room_c1[k][j+1]) / 2.0;
				}
			}
		}

	}

	// カメラの角度情報が入った配列を見ていく
	for (int n = 0; leftx_2[n] != NULL; n++) {

		/*camera 2*/
		left_deg2 = deg_adj_c2(leftx_2[n] / 3860.0);
		right_deg2 = deg_adj_c2(rightx_2[n] / 3860.0);

		left_rad2 = deg_to_rad(left_deg2);
		right_rad2 = deg_to_rad(right_deg2);


		point_c2 = 0.0;

		/*printf("カメラ2確認用\n");
		printf("leftx_2[n] = %lf\n", leftx_2[n]);
		printf("rightx_2[n] = %lf\n", rightx_2[n]);

		printf("left_deg2 = %lf\n", left_deg2);
		printf("right_deg2 = %lf\n", right_deg2);

		printf("left_rad2 = %lf\n", left_rad2);
		printf("right_rad2 = %lf\n", right_rad2);*/


		// 確率密度関数に入れていく　17*13
		for (int i = 0; i < 18; i++) {
//		for (int i = 17; i > 0; i--) {
			printf("i=%d,point_c2=%d\n", i, point_c2);
			//y軸　反転させる
			point_c2 = (int)(left_rad2 * ((double)i * 0.5) + c_y_2 - c_x_2 * left_rad2) * 2;
			//point_c2 = (int)(left_rad2 * ((double)i * 0.5) + (c_y_2 - c_x_2 * left_rad2)) * 2;
			//point_c2 = point_c2;
			point_c2 = 13 - point_c2;

			point_c2r = (int)(right_rad2 * ((double)i * 0.5) + (c_y_2 - c_x_2 * right_rad2)) * 2;
			//point_c2r = (int)(right_rad2 * ((double)i * 0.5) + (c_y_2 - c_x_2 * right_rad2)) * 2;
			//point_c2r = point_c2r;
			point_c2r = 13 - point_c2r;

			/*if (0 <= point_c2 && point_c2 <= 13) {
				printf("point c2 = %d\n", point_c2);
				printf("leftx_2 = %2.2f \n", leftx_2[n]);
			}*/
			if (0 <= point_c2 && point_c2 <= 13) {
				room_c2[point_c2][i] = (room_c2[point_c2][i] + 1.0) / 2.0;
				//	room_c2[point_c2][17-i] = (room_c2[point_c2][17 - i] + 1.0) / 2.0;
			}
			else {
				for (int j = 0; j < 13; j++) {
					room_c2[j][i] = room_c2[j][i] / 2.0;
					//room_c2[j][17 - i] = room_c2[j][17 - i] / 2.0;
				}
			}
			if (0 <= point_c2r && point_c2r <= 13) {
				room_c2[point_c2r][i] = (room_c2[point_c2r][i] + 1.0) / 2.0;
				//room_c2[point_c2r][17 - i] = (room_c2[point_c2r][17 - i] + 1.0) / 2.0;
			}
			else {
				/*for (int j = 0; j < 13; j++) {
					room_c2[j][17 - i] = room_c2[j][17 - i] / 2.0;
				}*/
			}
		}

		/*隣接するセルに値が入っていたら1/2の値を入れる*/
		for (int j = 0; j < 17; j++) {
			for (int k = 0; k < 13; k++) {
				if (room_c2[k][j] != 0.0 && k - 1 > 0 && room_c2[k-1][j] == 0.0) {
					room_c2[k - 1][j] = (room_c2[k][j] + room_c2[k - 1][j]) / 2.0;
				}
				if (room_c2[k][j] != 0.0 && j - 1 > 0 && room_c2[k][j-1] == 0.0) {
					room_c2[k][j - 1] = (room_c2[k][j] + room_c2[k][j-1]) / 2.0;
				}
				if (room_c2[k][j] != 0.0 && k + 1 < 13 && room_c2[k + 1][j] == 0.0) {
					room_c2[k + 1][j] = (room_c2[k][j] + room_c2[k + 1][j]) / 2.0;
				}
				if (room_c2[k][j] != 0.0 && j + 1 < 17 && room_c2[k][j+1] == 0.0) {
					room_c2[k][j + 1] = (room_c2[k][j] + room_c2[k][j+1]) / 2.0;
				}
			}
		}

	}


	//printf("計算結果\n");
	for (int j = 0; j < 13; j++) {
		for (int k = 0; k < 17; k++) {
			room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2.0;
			//閾値を0.5に仮定
			if (room[j][k] < 0.4)room[j][k] = 0.0;
			room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2.0;
			printf("%2.2lf ", room_c2[j][k]);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}

int main()//int argc, const char* argv[]
{
	FILE* fp1 = NULL; // FILE型構造体 読み込む方 #1
	FILE* fp2 = NULL; // FILE型構造体 読み込む方 #2
	FILE* fp3 = NULL; // 出力用ファイル
	FILE* fp4 = NULL;
	/*ファイル名*/
	char fname1[256] = {}; // 読み込みファイル名 #1
	char fname2[256] = {}; // 読み込みファイル名 #2

	/*処理用*/
	double max_n = 0.0;
	int max_x = 0, max_y = 0;
	int person_num1[35] = {};
	int person_num2[35] = {};
	char readline1[LineMax] = {};
	char readline2[LineMax] = {};
	char delim[] = ", "; //区切り文字
	char* tp1, * tp2;
	int i, j;
	int output_counter = 0;
	char point_text[255] = {};

	double room[14][18] = { 0.0 };
	double rad_room[p] = {};

	double tmp_pointx[30][4] = {};
	double tmp_pointy[30][4] = {};
	cv::Point out_point[4];

	//ウィンドウサイズ
	double rec_x1 = 0.0 + 50.0, rec_y1 = 0.0 + 50.0; //部屋の描写
	double rec_x2 = ROOM_W * 50.0 + 50.0, rec_y2 = ROOM_H * 50.0 + 50.0;

	double flame_pointx[p * 2] = {};
	double flame_pointy[p * 2] = {};

	double d_top5[5] = {};
	int person = 4;


	errno_t error;
	errno_t error1;

	printf("テキストを2種類読み込みdataフォルダに連番画像を出力します。\n");

	/*ファイル読み込み　1回目*/
	printf("読み込むテキストファイル名を入力してください\n");
	gets_s(fname1, 256);
	fopen_s(&fp1, fname1, "r");

	if (fp1 == NULL)
	{
		printf("ファイルを開くことが出来ません。\n");
		return -1;
	}

	/*ファイル読み込み　2回目*/
	printf("読み込むテキストファイル名を入力してください\n");
	gets_s(fname2, 256);
	fopen_s(&fp2, fname2, "r");

	if (fp2 == NULL)
	{
		printf("ファイルを開くことが出来ません。\n");
		return -1;
	}

	char output[] = "output.txt"; //出力用　ファイル名
	char point[] = "point.txt"; //出力用　ファイル名

	error = fopen_s(&fp3, output, "a");
	error1 = fopen_s(&fp4, point, "a");

	int tmp1 = 0, tmp2 = 0;

	// 確率分布表は横長になるように
	if (ROOM_W < ROOM_H)
	{
		tmp1 = ((int)(ROOM_CY + 0.5)) - 1;
		tmp2 = ((int)(ROOM_CX + 0.5)) - 1;
	}
	else
	{
		tmp1 = ((int)(ROOM_CX + 0.5)) - 1;
		tmp2 = ((int)(ROOM_CY + 0.5)) - 1;
	}

	int counter = 0;
	/*--テキストファイル 読み込み-------------------------------------------------------------------------------------------------------- */
	while (fgets(readline1, sizeof(readline1), fp1) != NULL || fgets(readline2, sizeof(readline2), fp2) != NULL)
	{
		double left_x1[14] = {}, right_x1[14] = {}, height1[14] = {};
		double left_x2[14] = {}, right_x2[14] = {}, height2[14] = {};
		person_num1[30] = {};
		person_num2[30] = {};
		double output_px[255] = {}, output_py[255] = {};
		double past_px[255] = {}, past_py[255] = {}, past_ph[255] = {};
		char* ctx1 = NULL;
		char* ctx2 = NULL;
		i = 0;
		j = 0;
		while (1)
		{

			/*1回目：ナンバー #1 */
			tp1 = readline1;
			tp1 = strtok_s(readline1, delim, &ctx1);
			person_num1[i] = atoi(tp1);
			if (person_num1[i] == -1)
				break;

			/*2回目：x座標(左) #1 */
			tp1 = strtok_s(NULL, delim, &ctx1);
			left_x1[i] = atof(tp1);

			/*3回目：x座標(右) #1 */
			tp1 = strtok_s(NULL, delim, &ctx1);
			right_x1[i] = atof(tp1);

			/*4回目：高さ #1 */
			tp1 = strtok_s(NULL, delim, &ctx1);
			height1[i] = atof(tp1);

			i++;
			//printf("i = %d\n",i);
			fgets(readline1, sizeof(readline1), fp1);
		}

		//printf("テキスト切り替え\n");
		fgets(readline2, sizeof(readline2), fp2);
		while (1)
		{
			/*2回目：ナンバー #2 */
			tp2 = readline2;
			tp2 = strtok_s(readline2, delim, &ctx2);
			person_num2[j] = atoi(tp2);
			//printf("person_num2[%d] = %d\n",j,person_num2[j]);
			if (person_num2[j] == -1)
			{
				//printf("break2\n");
				break;
			}

			/*2回目：x座標(左) #2 */
			tp2 = strtok_s(NULL, delim, &ctx2);
			left_x2[j] = atof(tp2);
			//printf("left_x2[j] = %2.2f\n\n", left_x2[j]);

			/*3回目：x座標(右) #2 */
			tp2 = strtok_s(NULL, delim, &ctx2);
			right_x2[j] = atof(tp2);

			/*4回目：高さ #2 */
			tp2 = strtok_s(NULL, delim, &ctx2);
			height2[j] = atof(tp2);

			j++;
			//printf("j = %d\n", j);
			fgets(readline2, sizeof(readline2), fp2);
			counter++;
		}
		// フレーム　切り替え
		camera1(left_x1, right_x1, height1, left_x2, right_x2, height2, room);

		//printf("フレーム切り替え\n");

		cv::Mat img(cv::Size(rec_x2 + 450, rec_y2 + 50), CV_8UC3, cv::Scalar(255, 255, 255));
		cv::namedWindow("ROOM", cv::WINDOW_AUTOSIZE);
		cv::rectangle(img, cv::Point(rec_x1, rec_y1), cv::Point(rec_x2, rec_y2), cv::Scalar(0, 0, 0), 2, 4); //部屋の描写

		for (int j = 0; j < 13; j++) {
			for (int k = 0; k < 17; k++) {
				if (room[j][k] > 0.0)fprintf(fp4, "%d %d\n", j, k);
				//if (room[j][k] > d_top5[4])fprintf(fp4, "%d %d\n", j, k);
				fprintf(fp3, "%2.2lf ", room[j][k]);
			}
			fprintf(fp3, "\n");
		}
		fprintf(fp3, "\n");
		fprintf(fp4, "\n");

		for (int j = 1; j < 14; j++) {
			for (int k = 1; k < 17; k++) {
				double wid = 0.5 * k * 50.0 + rec_x1;
				double hei = 0.5 * j * 50.0 + rec_y1;
				cv::Point ws_line(rec_x1, hei);
				cv::Point wg_line(rec_x2, hei);
				cv::Point hs_line(wid, rec_y1);
				cv::Point hg_line(wid, rec_y2);
				cv::line(img, ws_line, wg_line, cv::Scalar(0, 0, 0), 2, 4);
				cv::line(img, hs_line, hg_line, cv::Scalar(0, 0, 0), 2, 4);
			}
		}

		for (int j = 0; j < 13; j++) {
			for (int k = 0; k < 17; k++) {
				if (room[j][k] > 0.4) {
					double wid = 0.5 * k * 50.0 + rec_x1;
					double hei = 0.5 * j * 50.0 + rec_y1;
					cv::Point pt[4];
					pt[0] = cv::Point(wid, hei);
					pt[1] = cv::Point(wid + 50.5, hei);
					pt[2] = cv::Point(wid + 25.5, hei + 25.5);
					pt[3] = cv::Point(wid, hei + 50.5);
					cv::rectangle(img, pt[0], pt[2], cv::Scalar(255, 255, 185), -1, CV_AA);
				}
			}
		}

		output_counter++;
		cv::imshow("ROOM", img);
		cv::waitKey(30);
		cv::imwrite("data/output_" + std::to_string(output_counter) + ".png", img);
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	cv::destroyAllWindows();

	return 0;
}