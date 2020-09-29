﻿#define _USE_MATH_DEFINES

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

//部屋の広さ コアルーム 8.34*6.79
#define ROOM_H 6.79
#define ROOM_W 8.34
#define ROOM_CX 16.68
#define ROOM_CY 13.58

//WINDOW SIZE
#define WINDOW_H (ROOM_H*100)
#define WINDOW_W (ROOM_W*100)

//映像サイズ
#define RICOH_W 3860
#define RICOH_H 1830

#define LineMax 30

//カメラの設置の高さ(m) カメラ#1 左下0,0
#define c_z_1 2.5 //高さ
#define c_y_1 0.0 //縦 6.29
#define c_x_1 5.84 //横
#define c_r_1 270

//カメラの設置の高さ(m) カメラ#2 左下0,0
#define c_z_2 2.5 //高さ
#define c_y_2 6.29 //縦
#define c_x_2 4.84 //横
#define c_r_2 90

//#define deg_to_rad(deg) (((deg)/360)*2*M_PI)
#define deg_to_rad(deg) deg * M_PI/180

#define p 14 //同時に解析する人数(仮)

double room(double width, double height1) {
	double room = (ROOM_W / ROOM_H) * (WINDOW_H * 0.6);
	return room;
}

double deg_trans(double rad) {
	if (-180 <= rad && rad < 0) rad = 360 + rad;
	if (-360 <= rad && rad < -180) rad = - rad;
	return rad;
}

double d_camera(double leftx_1[p], double leftx_2[p], double rightx_1[p], double rightx_2[p],double output_px[p],double output_py[p]) {
	double point_x1 = 0.0, point_y1 = 0.0;
	double point_x2 = 0.0, point_y2 = 0.0;
	double point_x3 = 0.0, point_y3 = 0.0;
	double point_x4 = 0.0, point_y4 = 0.0;
	double men = 0.0;
	int k = 0;
	// 部屋の左下を0,0とする 向かい合うカメラは　2　と仮定する
	// 2直線の交点 ((d-b)/(a-c),(ad-bc)/(a-c))
	// y = ax+b, y = cx+d のとき
	//d = c_x_2 - c_y_2		b = c_x_1 - c_y_2
	//test
	double a1, a2, a3, a4, c1, c2,c3, c4, b,d;


	for (int i = 0; (leftx_1[i] != NULL); i++) {
		
		for (int j = 0; (leftx_2[j] != NULL); j++) {
			
			/*点A	leftx_1,leftx_2	の交点	*/
			a1 = tan(deg_to_rad(deg_trans(c_r_1-((leftx_1[i] * 360) / 3860)))); //直線の傾き #1
			c1 = tan(deg_to_rad(deg_trans(c_r_2-((leftx_2[j] * 360) / 3860)))); //直線の傾き #2

			b = c_y_1 - a1 * c_x_1;
			d = c_y_2 - c1 * c_x_2;

			point_x1 = (d - b) / (a1 - c1);
			point_y1 = (a1 * d - b * c1) / (a1 - c1);

			/*対象外条件*/
			if (0 > point_x1 || point_x1 > ROOM_W)break;
			if (0 > point_y1 || point_y1 > ROOM_H)break;

			/*点2	leftx_1, rightx_2 の交点*/
			a2 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_1[i] * 360) / 3860)))); //直線の傾き #1
			c2 = tan(deg_to_rad(deg_trans(c_r_2 - ((rightx_2[j] * 360) / 3860)))); //直線の傾き #2

			b = c_y_1 - a2 * c_x_1;
			d = c_y_2 - c2 * c_x_2;

			point_x2 = (d - b) / (a2 - c2);
			point_y2 = (a2 * d - b * c2) / (a2 - c2);

			/*対象外条件*/
			if (0 > point_x2 || point_x2 > ROOM_W)break;
			if (0 > point_y2 || point_y2 > ROOM_H)break;
			
			/*点3	rightx_1, rightx_2 の交点*/
			a3 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #1
			c3 = tan(deg_to_rad(deg_trans(c_r_2 - ((rightx_2[j] * 360) / 3860)))); //直線の傾き #2

			b = c_y_1 - a3 * c_x_1;
			d = c_y_2 - c3 * c_x_2;

			point_x3 = (d - b) / (a3 - c3);
			point_y3 = (a3 * d - b * c3) / (a3 - c3);

			/*対象外条件*/
			if (0 > point_x3 || point_x3 > ROOM_W)break;
			if (0 > point_y3 || point_y3 > ROOM_H)break;

			/*点4	leftx_2, rightx_1 の交点*/
			a4 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #1
			c4 = tan(deg_to_rad(deg_trans(c_r_2 - ((leftx_2[j] * 360) / 3860)))); //直線の傾き #2

			b = c_y_1 - a4 * c_x_1;
			d = c_y_2 - c4 * c_x_2;

			point_x4 = (d - b) / (a4 - c4);
			point_y4 = (a4 * d - b * c4) / (a4 - c4);

			/*対象外条件*/
			if (0 > point_x4 || point_x4 > ROOM_W)break;
			if (0 > point_y4 || point_y4 > ROOM_H)break;

			men = (point_x4 - point_x2 ) * ( point_y4 - point_y3) - 1/2 * (( point_x1 - point_x2 ) * ( point_y4 - point_y2 ) + 
				(point_x3 - point_x1) * (point_y2 - point_y3) + (point_x4 - point_x2) * (point_y4 - point_y1));

			if (men >0.25 )break;

			output_px[k] = (point_x1 + point_x2 + point_x3 + point_x4) / 4;
			output_py[k] = (point_y1 + point_y2 + point_y3 + point_y4) / 4;

			//printf("k = %d　%2.2f,%2.2f\n",k,output_px[k],output_py[k]);

			k++;
			//printf("point_x1 %0.2f\n", point_x1);
			//printf("point_y1 %0.2f\n\n", point_y1);

			//printf("point_x2 %0.2f\n", point_x2);
			//printf("point_y2 %0.2f\n\n", point_y2);

			//printf("point_x3 %0.2f\n", point_x3);
			//printf("point_y3 %0.2f\n\n", point_y3);

			//printf("point_x4 %0.2f\n", point_x4);
			//printf("point_y4 %0.2f\n", point_y4);
			//printf("\n\n");
		}
	}

	//output_px[k] = -1;
	//output_py[k] = -1;
	//test

	return 0;
}

int main()
{
	FILE* fp1 = NULL; // FILE型構造体 読み込む方 #1
	FILE* fp2 = NULL; // FILE型構造体 読み込む方 #2
	FILE* fp3 = NULL; //出力用ファイル
	FILE* fp4 = NULL; // 最大値出力ファイル読み込み

	/*ファイル名*/
	char max[] = "max.txt";
	char fname1[256]; // 読み込みファイル名 #1
	char fname2[256]; // 読み込みファイル名 #2


	/*処理用*/
	double max_n = 0.0;
	int max_x = 0, max_y = 0;
	int person_num1[35] = {};
	int person_num2[35] = {};
	char readline1[LineMax] = {};
	char readline2[LineMax] = {};
	char delim[] = ", "; //区切り文字
	char* tp1, * tp2;
	int counter_m = 0;
	int i, j;

	//double ave[((int)(ROOM_CY + 0.5))][((int)(ROOM_CX + 0.5)) - 1] = {};
	//double ave_total[((int)(ROOM_CY + 0.5))][((int)(ROOM_CX + 0.5)) - 1] = { 1.0 };

	errno_t error;

	fopen_s(&fp4, max, "a");


	printf("読み込むテキストファイル名を入力してください\n");
	gets_s(fname1, 256);
	fopen_s(&fp1, fname1, "r");

	if (fp1 == NULL) {
		printf("ファイルを開くことが出来ません。\n");
		return -1;
	}

	printf("読み込むテキストファイル名を入力してください\n");
	gets_s(fname2, 256);
	fopen_s(&fp2, fname2, "r");

	if (fp2 == NULL) {
		printf("ファイルを開くことが出来ません。\n");
		return -1;
	}

	char output[] = "output.txt";

	error = fopen_s(&fp3, output, "a");

	int tmp1 = 0, tmp2 = 0;

	if (ROOM_W < ROOM_H) {
		tmp1 = ((int)(ROOM_CY + 0.5)) - 1;
		tmp2 = ((int)(ROOM_CX + 0.5)) - 1;
	}
	else {
		tmp1 = ((int)(ROOM_CX + 0.5)) - 1;
		tmp2 = ((int)(ROOM_CY + 0.5)) - 1;
	}

	int counter = 0;
	/*--テキストファイル #1	-------------------------------------------------------------------------------------------------------- */
	while (fgets(readline1, sizeof(readline1), fp1) != NULL || fgets(readline2, sizeof(readline2), fp2) != NULL) {
		double left_x1[14] = {}, right_x1[14] = {}, height1[14] = {};
		person_num1[30] = {};
		double left_x2[14] = {}, right_x2[14] = {}, height2[14] = {};
		person_num2[30] = {};
		double output_px[255] = {};
		double output_py[255] = {};
		char* ctx1 = NULL;
		char* ctx2 = NULL;
		i = 0;
		j = 0;
		while (1) {
			
		/*1回目：ナンバー #1 */
		tp1 = readline1;
		tp1 = strtok_s(readline1, delim, &ctx1);
		person_num1[i] = atoi(tp1);
		//printf("person_num[%d] = %d\n",i,person_num1[i]);
		if (person_num1[i] == -1) {
			//printf("break1\n");
			break;
		}

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
			while (1) {
				//puts(readline2);
				/*1回目：ナンバー #2 */
				tp2 = readline2;
				tp2 = strtok_s(readline2, delim, &ctx2);
				person_num2[j] = atoi(tp2);
				//printf("person_num2[%d] = %d\n",j,person_num2[j]);
				if (person_num2[j] == -1) {
					//printf("break2\n");
					break;
				}

				/*2回目：x座標(左) #2 */
				tp2 = strtok_s(NULL, delim, &ctx2);
				left_x2[j] = atof(tp2);

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
			//fprintf(fp3, "\n\n");
			//printf("%d\n",counter);

			d_camera(left_x1, left_x2, right_x1, right_x2,output_px,output_py);

			for (int i = 0; output_px[i] != '\0' || output_py[i] != '\0'; i++) {
				//printf(" i = %d  %2.2f,%2.2f\n",i,output_px[i],output_py[i]);
				fprintf(fp3, "%2.2f ", output_px[i]);
				fprintf(fp3, "%2.2f\n", output_py[i]);
				
			}

			fprintf(fp3,"\n");
	}				

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);

	return 0;
}
