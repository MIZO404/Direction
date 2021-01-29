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
#define c_r_1 270

//カメラの設置の高さ(m) カメラ#2 左下0,0
#define c_z_2 2.5  //高さ
#define c_y_2 5.57 //縦
//#define c_x_2 3.49 //横
#define c_x_2 4.49 //横
#define c_r_2 90


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
	//if (deg < 270.0)deg = 270.0 - deg;
	//if (270.0 < deg && deg < 360.0)deg = 360.0 - deg + 270.0;
	deg = deg + c_r_1;
	if (360.0 < deg)deg = 360.0 - deg;
	return deg;
}

double deg_adj_c2(double deg) {
	/*if (deg < 90.0)deg = 90.0 - deg;
	if (90.0 < deg)deg = 360.0 - deg + 90.0;*/
	deg = deg + c_r_2;
	if (360.0 < deg)deg = 360.0 - deg;
	return deg;
}


//double d_room(double leftx_1[p], double leftx_2[p], double rightx_1[p], double rightx_2[p], double height1[p], double height2[p], double output_px[p], double output_py[p], double ptx[30][4], double pty[30][4], double room[14][18], double past_px[p], double past_py[p], double past_ph[p], double tmp_room[p]) {
//	// room[12][16]
//	double a1, a2; //camera1
//	double a3, a4; //camera2
//	double b, d;
//	int tmp_lx1[17] = {}, tmp_rx1[17] = {}; //値を入れる位置
//	int tmp_lx2[17] = {}, tmp_rx2[17] = {};
//
//	double room_c1[14][18] = {};
//	double room_c2[14][18] = {};
//
//	for (int i = 0; (leftx_1[i] != NULL); i++) {
//		for (int m = 0; (leftx_2[m] != NULL); m++) {
//			/*printf(" leftx_1 = %2.2f\n", leftx_1[i]);
//			printf(" leftx_2 = %2.2f\n", leftx_2[m]);*/
//
//			//a1 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_1[i] * 360) / 3860)))); //直線の傾き #1
//			//a2 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #2
//			//a3 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_2[m] * 360) / 3860)))); //直線の傾き #3
//			//a4 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_2[m] * 360) / 3860)))); //直線の傾き #4
//
//			/* camera1 */
//			for (int j = 0; j < 13; j++) { //縦
//				int counter1_l = 0;
//				int counter1_r = 0;
//				int counter2_l = 0;
//				int counter2_r = 0;
//
//				//一行ずつ計算していく
//				for (int k = 0; k < 17; k++) { //横
//
//					/*leftx_1*/
//					//a1 = tan(deg_to_rad(deg_trans((double)c_r_1 - ((leftx_1[i] * 360.0) / 3860.0)))); //直線の傾き #1
//					a1 = tan(deg_to_rad((rad_adj_c1((double)((leftx_1[i] * 360.0) / 3860.0))))); //直線の傾き #1
//
//					//b = (ROOM_H - c_y_1 )- a1 * (c_x_1);
//					b = (c_y_1)-a1 * (c_x_1);
//					d = 0.25 + 0.5 * (double)j;
//
//					//if ((int)((d - b) / a1) > 0.0 && (int)((d - b) / a1) < 18.0) {
//					if ((int)((d - b) / a1) == k) {
//						tmp_lx1[counter1_l] = (int)((d - b) / a1);
//						counter1_l++;
//					}
//
//					/*rightx_1,*/
//					a2 = tan(deg_to_rad((rad_adj_c1((double)((rightx_1[i] * 360.0) / 3860.0))))); //直線の傾き #1
//
//					//b = (ROOM_H - c_y_1) - a2 * c_x_1;
//					b = (c_y_1)-a2 * c_x_1;
//					d = 0.25 + 0.5 * (double)j;
//
//					//if ((int)((d - b) / a2) < 0.0) {
//					//	tmp_rx1[counter1_r] = 0; //x座標
//					//	counter1_r++;
//					//}
//					if ((int)((d - b) / a2) == k) {
//						//if ((int)((d - b) / a2) > 0.0 && (int)((d - b) / a2) < 18.0) {
//						tmp_rx1[counter1_r] = (int)((d - b) / a2);
//						counter1_r++;
//					}
//
//
//					/*leftx_2*/
//					a3 = tan(deg_to_rad((rad_adj_c2((double)((leftx_2[m] * 360.0) / 3860.0))))); //直線の傾き #1
//
//					//b = (ROOM_H - c_y_2 )- a3 * c_x_2;
//					b = (c_y_2)-a3 * c_x_2;
//					d = 0.25 + 0.5 * (double)j;
//
//					//if ((d - b) / a3 < 0) {
//					//	tmp_lx2[counter2_l] = 0; //x座標
//					//	counter2_l++;
//					//}
//					//if ((int)((d - b) / a3) > 0 && (int)((d - b) / a3) < 18) {
//					if ((int)((d - b) / a3) == k) {
//						tmp_lx2[counter2_l] = (int)((d - b) / a3);
//						counter2_l++;
//					}
//
//
//					/*rightx_2,*/
//					a4 = tan(deg_to_rad((rad_adj_c2((double)((rightx_2[m] * 360.0) / 3860.0))))); //直線の傾き #1
//
//					//b = (ROOM_H - c_y_2) - a4 * c_x_2;
//					b = (c_y_2)-a4 * c_x_2;
//					d = 0.25 + 0.5 * (double)j;
//
//					//if ((d - b) / a4 < 0) {
//					//	tmp_rx2[counter2_r] = 0; //x座標
//					//	counter2_r++;
//					//}
//					//if ((int)((d - b) / a4) > 0 && (int)((d - b) / a4) < 18) {
//					if ((int)((d - b) / a4) == k) {
//						tmp_rx2[counter2_r] = (int)((d - b) / a4);
//						counter2_r++;
//					}
//				}
//
//				for (int l = 0; counter1_l > l; l++) {
//					int tmplx1 = tmp_lx1[l];
//					int tmprx1 = tmp_rx1[l];
//					int tmpax1 = (tmp_lx1[l] + tmp_rx1[l]) / 2;
//					int tmplx2 = tmp_lx2[l];
//					int tmprx2 = tmp_rx2[l];
//					int tmpax2 = (tmp_lx2[l] + tmp_rx1[l]) / 2;
//
//					if (0 <= tmplx2 && tmplx2 < 16)room_c2[j][tmplx2] = (room_c2[j][tmplx2] + 0.5) / 2.0;
//					if (0 <= tmprx2 && tmprx2 < 16)room_c2[j][tmprx2] = (room_c2[j][tmprx2] + 0.5) / 2.0;
//					if (0 <= tmpax2 && tmpax2 < 16)room_c2[j][tmpax2] = (room_c2[j][tmpax2] + 1.0) / 2.0;
//
//					//if (0 <= tmplx1 && tmplx1 < 16)room_c1[j][17-tmplx1] = (room_c1[j][17 - tmplx1] + 0.5) / 2.0;
//					//if (0 <= tmprx1 && tmprx1 < 16)room_c1[j][17 - tmprx1] = (room_c1[j][17 - tmprx1] + 0.5) / 2.0;
//					//if (0 <= tmpax1 && tmpax1 < 16)room_c1[j][17 - tmpax1] = (room_c1[j][17 - tmpax1] + 1.0) / 2.0;
//
//					if (0 <= tmplx1 && tmplx1 < 16)room_c1[j][tmplx1] = (room_c1[j][tmplx1] + 0.5) / 2.0;
//					if (0 <= tmprx1 && tmprx1 < 16)room_c1[j][tmprx1] = (room_c1[j][tmprx1] + 0.5) / 2.0;
//					if (0 <= tmpax1 && tmpax1 < 16)room_c1[j][tmpax1] = (room_c1[j][tmpax1] + 1.0) / 2.0;
//
//					/*if (0 <= 17 - tmplx1 && 17 - tmplx1 < 17)room_c1[12 - j][17 - tmplx1] = (room_c1[12 - j][16 - tmplx1] + 0.5) / 2.0;
//					if (0 <= 17 - tmprx1 && 17 - tmprx1 < 17)room_c1[12 - j][17 - tmprx1] = (room_c1[12 - j][16 - tmprx1] + 0.5) / 2.0;
//					if (0 <= 17 - tmpax1 && 17 - tmpax1 < 17)room_c1[12 - j][17 - tmpax1] = (room_c1[12 - j][16 - tmpax1] + 1.0) / 2.0;*/
//				}
//
//			}
//			printf("camera1\n");
//			for (int j = 0; j < 13; j++) {
//				for (int k = 0; k < 17; k++) {
//
//					//閾値を0.5に仮定
//					//if (room[j][k] < 0.5)room[j][k] = 0.0;
//					//printf("test\n");
//					printf("%2.2lf ", room_c1[j][k]);
//				}
//				printf("\n");
//			}
//			printf("\n");
//
//			//printf("camera2\n");
//			//for (int j = 0; j < 13; j++) {
//			//	for (int k = 0; k < 17; k++) {
//
//			//		//閾値を0.5に仮定
//			//		//if (room[j][k] < 0.5)room[j][k] = 0.0;
//			//		//printf("test\n");
//			//		printf("%2.2lf ", room_c2[j][k]);
//			//	}
//			//	printf("\n");
//			//}
//			//printf("\n");
//		}
//
//		//printf("座標取得\n");
//		//printf("途中経過\n");
//		for (int j = 0; j < 13; j++) {
//			for (int k = 0; k < 17; k++) {
//				room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2;
//				//printf("%2.2lf ", room[j][k]);
//			}
//			//printf("\n");
//		}
//		//printf("\n");
//	}
//
//	//座標の読み込み終了
//
//	//printf("計算結果\n");
//	//for (int j = 0; j < 13; j++) {
//	//	for (int k = 0; k < 17; k++) {
//	//		room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2.0;
//	//		//閾値を0.5に仮定
//	//		//if (room[j][k] < 0.4)room[j][k] = 0.0;
//	//		printf("%2.2lf ", room[j][k]);
//	//	}
//	//	printf("\n");
//	//}
//	//printf("\n");
//
//	return 0;
//}

//double d_room(double leftx_1[p], double leftx_2[p], double rightx_1[p], double rightx_2[p], double height1[p], double height2[p], double output_px[p], double output_py[p], double ptx[30][4], double pty[30][4], double room[14][18], double past_px[p], double past_py[p], double past_ph[p], double tmp_room[p]) {
//	double room_c1[14][18] = {};
//	double room_c2[14][18] = {};
//	double a1, a2, a3, a4, b;
//	int tmp_x1 = 0, tmp_x2 = 0, tmp_x3 = 0, tmp_x4 = 0;
//	for (int i = 0; (leftx_1[i] != NULL); i++) {
//		tmp_x1 = 0;
//		tmp_x2 = 0;
//		printf("leftx_1[%d] = %2.2lf\n", i, leftx_1[i]);
//		/* camera1 */
//		for (int j = 13; j > 1; j--) { //縦
//
//				/*leftx_1*/
//			//a1 = tan(deg_to_rad((rad_adj_c1((double)((leftx_1[i] * 360.0) / 3860.0))))); //直線の傾き #1
//			a1 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_1[i] * 360) / 3860)))); //直線の傾き #1
//			printf("a1 = %2.2f\n", a1);
//
//			//b = (ROOM_H - c_y_1 )- a1 * (c_x_1);
//			b = (c_y_1)-a1 * (c_x_1);
//			printf("b1 = %2.2f\n", b);
//
//			if (0 <= (ROOM_H - 0.5 * j - b) / a1 && (ROOM_H - 0.5 * j - b) / a1 <= 17) {
//				tmp_x1 = (ROOM_H - j * 0.5 - b) / a1 * 2;
//				printf("tmp_x1 = %d\n", tmp_x1);
//				//room_c1[13 - j][tmp_x1] = (room_c1[13 - j][tmp_x1] + 1.0) / 2.0;
//				room_c1[13 - j][tmp_x1] = (room_c1[13 - j][tmp_x1] + 1.0);
//			}
//			else {
//				for (int m = 0; m < 18; m++) {
//					//room_c1[13 - j][m] = (room_c1[13 - j][m] + 0.0) / 2.0;
//					room_c1[13 - j][m] = (room_c1[13 - j][m] + 0.0);
//				}
//			}
//
//			/*rightx_1*/
//			//a2 = tan(deg_to_rad((rad_adj_c1((double)((rightx_1[i] * 360.0) / 3860.0))))); //直線の傾き #1
//			a2 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #1
//			printf("a2 = %2.2f\n", a2);
//
//			//b = (ROOM_H - c_y_1 )- a1 * (c_x_1);
//			b = (c_y_1)-a2 * (c_x_1);
//			printf("b2 = %2.2fn", b);
//
//			if (0 <= (ROOM_H - 0.5 * j - b) / a2 && (ROOM_H - 0.5 * j - b) / a2 <= 17) {
//				tmp_x2 = (ROOM_H - j * 0.5 - b) / a2 * 2;
//				printf("tmp_x2 = %d\n", tmp_x2);
//				//room_c1[13 - j][tmp_x2] = (room_c1[13 - j][tmp_x2] + 1.0) / 2.0;
//				room_c1[13 - j][tmp_x2] = (room_c1[13 - j][tmp_x2] + 1.0);
//			}
//			else {
//				for (int m = 0; m < 18; m++) {
//					//room_c1[13 - j][m] = (room_c1[13 - j][m] + 0.0) / 2.0;
//					room_c1[13 - j][m] = (room_c1[13 - j][m] + 0.0);
//				}
//			}
//
//
//			for (int k = tmp_x1; k < tmp_x2 && k < 17 && 0 < tmp_x1; k++) {
//				//room_c1[13 - j][k] = (room_c1[13 - j][k] + 1.0) / 2.0;
//				room_c1[13 - j][k] = (room_c1[13 - j][k] + 1.0);
//
//				//if ((tmp_x1 + tmp_x2) / 2 == k)room_c1[13 - j][tmp_x1] = (room_c1[13 - j][tmp_x1] + 1.0) / 2.0;
//				if ((tmp_x1 + tmp_x2) / 2 == k)room_c1[13 - j][tmp_x1] = (room_c1[13 - j][tmp_x1] + 1.0);
//
//			}
//			printf("j = %d, tmp_x1 = %d, tmp_x2 = %d\n", j, tmp_x1, tmp_x2);
//		}
//	}
//
//
//	for (int i = 0; (leftx_2[i] != NULL); i++) {
//		tmp_x3 = 0;
//		tmp_x4 = 0;
//		//printf("leftx_2[%d] = %2.2lf\n", i, leftx_2[i]);
//		/* camera2 */
//		for (int j = 13; j > 1; j--) { //縦
//
//				/*leftx_1*/
//			//a3 = tan(deg_to_rad((rad_adj_c2((double)((leftx_2[i] * 360.0) / 3860.0))))); //直線の傾き #1
//			a3 = tan(deg_to_rad(deg_trans(c_r_2 - ((leftx_2[i] * 360) / 3860)))); //直線の傾き #1
//			//printf("a1 = %2.2f\n",a1);
//
//			//b = (ROOM_H - c_y_1 )- a1 * (c_x_1);
//			b = (c_y_2)-a3 * (c_x_2);
//
//			if (0 <= (ROOM_H - j * 0.5 - b) / a3 && (ROOM_H - j * 0.5 - b) / a3 <= 17) {
//				tmp_x3 = (ROOM_H - j * 0.5 - b) / a3 * 2;
//				//printf("tmp_x3 = %d\n", tmp_x3);
//				//room_c2[13 - j][tmp_x3] = (room_c2[13 - j][tmp_x3] + 1.0) / 2.0;
//				room_c2[13 - j][tmp_x3] = (room_c2[13 - j][tmp_x3] + 1.0);
//			}
//			else {
//				for (int m = 0; m < 18; m++) {
//					//room_c2[13 - j][m] = (room_c2[13 - j][m] + 0.0) / 2.0;
//					room_c2[13 - j][m] = (room_c2[13 - j][m] + 0.0);
//				}
//			}
//
//
//			/*rightx_2*/
//			//a4 = tan(deg_to_rad((rad_adj_c2((double)((rightx_2[i] * 360.0) / 3860.0))))); //直線の傾き #1
//			a4 = tan(deg_to_rad(deg_trans(c_r_2 - ((rightx_2[i] * 360) / 3860)))); //直線の傾き #1
//			//printf("a1 = %2.2f\n",a1);
//
//			//b = (ROOM_H - c_y_1 )- a1 * (c_x_1);
//			b = (c_y_2)-a4 * (c_x_2);
//
//			if (0 <= (ROOM_H - j * 0.5 - b) / a4 && (ROOM_H - j * 0.5 - b) / a4 <= 17) {
//				tmp_x4 = (ROOM_H - j * 0.5 - b) / a4 * 2;
//				printf("tmp_x4 = %d\n", tmp_x4);
//				//room_c2[13 - j][tmp_x4] = (room_c2[13 - j][tmp_x4] + 1.0) / 2.0;
//				room_c2[13 - j][tmp_x4] = (room_c2[13 - j][tmp_x4] + 1.0);
//			}
//			else {
//				for (int m = 0; m < 18; m++) {
//					//room_c2[13 - j][m] = (room_c2[13 - j][m] + 0.0) / 2.0;
//					room_c2[13 - j][m] = (room_c2[13 - j][m] + 0.0);
//				}
//			}
//
//
//			for (int k = tmp_x3; k < tmp_x4 && k < 17 && 0 < tmp_x3; k++) {
//
//				//room_c2[13 - j][k] = (room_c2[13 - j][k] + 0.5) / 2.0;
//				room_c2[13 - j][k] = (room_c2[13 - j][k] + 0.5);
//
//				//if ((tmp_x3 + tmp_x4) / 2)room_c2[13 - j][tmp_x3] = (room_c2[13 - j][tmp_x3] + 1.0) / 2.0;
//				if ((tmp_x3 + tmp_x4) / 2)room_c2[13 - j][tmp_x3] = (room_c2[13 - j][tmp_x3] + 1.0);
//			}
//
//		}
//	}
//
//	printf("room in output %2.2lf\n", output_px[0]);
//
//	for (int j = 0; output_px[j] != NULL;j++) {
//		for (int k = 0; output_py[k] != NULL;k++) {
//			if (0 <= output_px[j] && output_px[j] <= ROOM_W && 0 <= output_py[k] && output_py[k] <= ROOM_H) {
//				//room[(int)output_px[j] * 2][(int)output_py[k] * 2] = room[(int)output_px[j] * 2][(int)output_py[k] * 2] + 1.0 / 2.0;
//				room[(int)output_px[j] * 2][(int)output_py[k] * 2] = room[(int)output_px[j] * 2][(int)output_py[k] * 2] + 1.0;
//			}
//			printf("plus\n");
//		}
//	}
//
//	for (int j = 13; j >0; j--) {
//		for (int k = 0; k < 17; k++) {
//
//			//閾値を0.5に仮定
//			//if (room[j][k] < 0.5)room[j][k] = 0.0;
//			//printf("test\n");
//			//room[j][k] = (room_c1[j][k] + room_c2[j][k]);
//			room[j][k] = (room_c1[j][k]);
//			printf("%2.2lf ", room_c1[j][k]);
//		}
//		printf("\n");
//	}
//	printf("\n");
//	return 0;
//}

//double d_camera(double leftx_1[p], double leftx_2[p], double rightx_1[p], double rightx_2[p], double height1[p], double height2[p], double output_px[p], double output_py[p], double ptx[30][4], double pty[30][4], double room[p], double past_px[p], double past_py[p], double past_ph[p])
//{
//	double point_x1 = 0.0, point_y1 = 0.0;
//	double point_x2 = 0.0, point_y2 = 0.0;
//	double point_x3 = 0.0, point_y3 = 0.0;
//	double point_x4 = 0.0, point_y4 = 0.0;
//	double men = 0.0;
//	int k = 0;
//	double line1 = 0.0, line2 = 0.0, line3 = 0.0, line4 = 0.0, line5 = 0.0, line6 = 0.0;
//	double line_ave = 0.0;
//	// 部屋の左下を0,0とする 向かい合うカメラは　2　と仮定する
//	// 2直線の交点 ((d-b)/(a-c),(ad-bc)/(a-c))
//	// y = ax+b, y = cx+d のとき
//	//d = c_x_2 - c_y_2		b = c_x_1 - c_y_2
//	//test
//	double a1, a2, a3, a4, a5, c1, c2, c3, c4, c5, b, d;
//	for (int i = 0; (leftx_1[i] != NULL); i++)
//	{
//
//		for (int j = 0; (leftx_2[j] != NULL); j++)
//		{//test
//			printf("test");
//			/*点A	leftx_1,leftx_2	の交点	*/
//			a1 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_1[i] * 360) / 3860)))); //直線の傾き #1
//			c1 = tan(deg_to_rad(deg_trans(c_r_2 - ((leftx_2[j] * 360) / 3860)))); //直線の傾き #2
//
//			b = c_y_1 - a1 * c_x_1;
//			d = c_y_2 - c1 * c_x_2;
//
//			point_x1 = (d - b) / (a1 - c1);
//			point_y1 = (a1 * d - b * c1) / (a1 - c1);
//
//			printf("point x %2.2f\n", point_x1);
//			/*対象外条件*/
//			if (0 > point_x1 || point_x1 > ROOM_W)continue;
//			if (0 > point_y1 || point_y1 > ROOM_H)continue;
//
//			/*点2	leftx_1, rightx_2 の交点*/
//			a2 = tan(deg_to_rad(deg_trans(c_r_1 - ((leftx_1[i] * 360) / 3860))));  //直線の傾き #1
//			c2 = tan(deg_to_rad(deg_trans(c_r_2 - ((rightx_2[j] * 360) / 3860)))); //直線の傾き #2
//
//			b = c_y_1 - a2 * c_x_1;
//			d = c_y_2 - c2 * c_x_2;
//
//			point_x2 = (d - b) / (a2 - c2);
//			point_y2 = (a2 * d - b * c2) / (a2 - c2);
//
//			/*対象外条件*/
//			if (0 > point_x2 || point_x2 > ROOM_W)continue;
//			if (0 > point_y2 || point_y2 > ROOM_H)continue;
//
//			/*点3	rightx_1, rightx_2 の交点*/
//			a3 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #1
//			c3 = tan(deg_to_rad(deg_trans(c_r_2 - ((rightx_2[j] * 360) / 3860)))); //直線の傾き #2
//
//			b = c_y_1 - a3 * c_x_1;
//			d = c_y_2 - c3 * c_x_2;
//
//			point_x3 = (d - b) / (a3 - c3);
//			point_y3 = (a3 * d - b * c3) / (a3 - c3);
//
//			/*対象外条件*/
//			if (0 > point_x3 || point_x3 > ROOM_W)continue;
//			if (0 > point_y3 || point_y3 > ROOM_H)continue;
//
//			/*点4	leftx_2, rightx_1 の交点*/
//			a4 = tan(deg_to_rad(deg_trans(c_r_1 - ((rightx_1[i] * 360) / 3860)))); //直線の傾き #1
//			c4 = tan(deg_to_rad(deg_trans(c_r_2 - ((leftx_2[j] * 360) / 3860))));  //直線の傾き #2
//
//			b = c_y_1 - a4 * c_x_1;
//			d = c_y_2 - c4 * c_x_2;
//
//			point_x4 = (d - b) / (a4 - c4);
//			point_y4 = (a4 * d - b * c4) / (a4 - c4);
//
//			/*対象外条件*/
//			if (0 > point_x4 || point_x4 > ROOM_W)continue;
//			if (0 > point_y4 || point_y4 > ROOM_H)continue;
//
//			//men = (point_x4 - point_x2) * (point_y4 - point_y3) - 1 / 2 * ((point_x1 - point_x2) * (point_y4 - point_y2) + (point_x3 - point_x1) * (point_y2 - point_y3) + (point_x4 - point_x2) * (point_y4 - point_y1));
//			men = abs(((point_x1 - point_x2) * (point_y1 + point_y2) + (point_x2 - point_x3) * (point_y2 + point_y3) + (point_x3 - point_x4) * (point_y3 + point_y4) + (point_x4 - point_x1) * (point_y4 + point_y1)) / 2);
//			//printf("面積　%2.2lf\n",men);
//			if (men < 0.2 || men > 0.7)continue;
//
//			line1 = sqrt(pow(point_x1 - point_x2, 2) + pow(point_y1 - point_y2, 2));
//			line2 = sqrt(pow(point_x1 - point_x3, 2) + pow(point_y1 - point_y3, 2));
//			line3 = sqrt(pow(point_x1 - point_x4, 2) + pow(point_y1 - point_y4, 2));
//			line4 = sqrt(pow(point_x2 - point_x3, 2) + pow(point_y2 - point_y3, 2));
//			line5 = sqrt(pow(point_x2 - point_x4, 2) + pow(point_y2 - point_y4, 2));
//			line6 = sqrt(pow(point_x3 - point_x4, 2) + pow(point_y3 - point_y4, 2));
//			line_ave = (line1 + line2 + line3 + line4 + line5 + line6) / 6;
//			double line_thres = 0.7;
//
//			if (line1 > 1.5 || (line_ave + line_thres < line1 || line1 < line_ave - line_thres))continue; //1-2
//			if (line2 > 1.5 || (line_ave + line_thres < line2 || line2 < line_ave - line_thres))continue; //1-3
//			if (line3 > 1.5 || (line_ave + line_thres < line3 || line3 < line_ave - line_thres))continue; //1-4
//			if (line4 > 1.5 || (line_ave + line_thres < line4 || line4 < line_ave - line_thres))continue; //2-3
//			if (line5 > 1.5 || (line_ave + line_thres < line5 || line5 < line_ave - line_thres))continue; //2-4
//			if (line6 > 1.5 || (line_ave + line_thres < line6 || line6 < line_ave - line_thres))continue; //3-4 //√2を考慮して1.5
//
//			double tri_x1 = (point_x1 + point_x2 + point_x3) / 3;
//			double tri_y1 = (point_y1 + point_y2 + point_y3) / 3;
//			double tri_x2 = (point_x1 + point_x3 + point_x4) / 3;
//			double tri_y2 = (point_y1 + point_y3 + point_y4) / 3;
//
//
//			//output_px[k] = (tri_x1 + tri_x2) / 2;
//			//output_py[k] = (tri_y1 + tri_y2) / 2;
//
//			// 許容範囲を1.0mと仮定
//			for (int l = 0; l < p; l++)
//			{
//
//				if (past_px[l] == NULL || (past_px[l] - 0.3 <= (tri_x1 + tri_x2) / 2 && (tri_x1 + tri_x2) / 2 <= past_px[l] + 0.3)
//					&& (past_py[l] - 0.5 <= (tri_y1 + tri_y2) / 2 && (tri_y1 + tri_y2) / 2 <= past_py[l] + 0.5)
//					/*|| (past_ph[l] - 0.5 <= height1[k] && height1[k] <= past_ph[l] + 0.5)*/)
//				{
//					output_px[k] = (tri_x1 + tri_x2) / 2;
//					output_py[k] = (tri_y1 + tri_y2) / 2;
//
//					past_px[l] = output_px[k];
//					past_py[l] = output_py[k];
//				}
//				else {
//					continue;
//				}
//			}
//
//
//			//printf("%d %2.2f,%2.2f\n", k, output_px[k], output_py[k]);
//
//			ptx[k][0] = point_x1;
//			ptx[k][1] = point_x2;
//			ptx[k][2] = point_x3;
//			ptx[k][3] = point_x4;
//
//			pty[k][0] = point_y1;
//			pty[k][1] = point_y2;
//			pty[k][2] = point_y3;
//			pty[k][3] = point_y4;
//			k++;
//		}
//		//printf("\n");//
//	}
//	return 0;
//
//}

//double top5(double room[13][18], int person, double top5[5]) {
//	int i, j, tmp;
//
//	/* 数値の総数を入力 */
//	int total = person;
//
//	/* 数値を昇順にソート */
//	for (i = 0; i < total; ++i) {
//		for (j = i + 1; j < total; ++j) {
//			if (top5[i] > top5[j]) {
//				tmp = top5[i];
//				top5[i] = top5[j];
//				top5[j] = tmp;
//			}
//		}
//	}
//	return 0;
//}

//double d_camera(double leftx_1[p], double leftx_2[p], double rightx_1[p], double rightx_2[p], double height1[p], double height2[p], double output_px[p], double output_py[p], double ptx[30][4], double pty[30][4], double room[p], double past_px[p], double past_py[p], double past_ph[p])

double camera1(double leftx_1[p], double rightx_1[p], double height1[p], double leftx_2[p],double rightx_2[p],double height[p],double room[14][18]) {
	
	double left_deg1 = 0.0,left_deg2 = 0.0;
	double right_deg1 = 0.0, right_deg2 = 0.0;
	double left_rad1 = 0.0, left_rad2 = 0.0;
	double right_rad1 = 0.0, right_rad2 = 0.0;
	double room_c1[14][18]={},room_c2[14][18] = {};
	int point_c1 = 0, point_c2 = 0;

	// カメラの角度情報が入った配列を見ていく
	for (int n = 0; leftx_1[n] != NULL; n++) {

		/*camera 1*/
		left_deg1 = deg_adj_c1(leftx_1[n] / 3860.0);
		right_deg1 = deg_adj_c1(rightx_1[n] / 3860.0);

		left_rad1 = deg_to_rad(left_deg1);
		right_rad1 = deg_to_rad(right_deg1);

		/*printf("カメラ１確認用\n");
		printf("leftx_1[n] = %lf\n", leftx_1[n]);
		printf("rightx_1[n] = %lf\n", rightx_1[n]);

		printf("left_deg1 = %lf\n", left_deg1);
		printf("right_deg1 = %lf\n", right_deg1);

		printf("left_rad1 = %lf\n", left_rad1);
		printf("right_rad1 = %lf\n", right_rad1);*/


		// 確率密度関数に入れていく　17*13
		for (int i = 0; i < 17; i++) {
			point_c1 = (int)((left_rad1 * ((double)i*0.5)) + (c_x_1 - left_rad1 * c_y_1))*2;
			if (0 <= point_c1 && point_c1 <= 13) {
				room_c1[point_c1][i] = (room_c1[point_c1][i] + 1.0) / 2.0;
			}
			else {
				for (int j = 0; j < 13; j++) {
					room_c1[j][i] = room_c1[j][i] / 2.0;
				}
			}
			//printf("test = %d\n",point_c1);
		}
	}


	// カメラの角度情報が入った配列を見ていく
	for (int n = 0; leftx_2[n] != NULL; n++) {

		/*camera 2*/
		left_deg2 = deg_adj_c2(leftx_2[n] / 3860.0);
		right_deg2 = deg_adj_c2(rightx_2[n] / 3860.0);

		left_rad2 = deg_to_rad(left_deg2);
		right_rad2 = deg_to_rad(right_deg2);

		/*printf("カメラ2確認用\n");
		printf("leftx_2[n] = %lf\n", leftx_2[n]);
		printf("rightx_2[n] = %lf\n", rightx_2[n]);

		printf("left_deg2 = %lf\n", left_deg2);
		printf("right_deg2 = %lf\n", right_deg2);

		printf("left_rad2 = %lf\n", left_rad2);
		printf("right_rad2 = %lf\n", right_rad2);*/


		// 確率密度関数に入れていく　17*13
		for (int i = 0; i < 17; i++) {
			point_c2 = (int)((left_rad2 * ((double)i*0.5)) + (c_x_2 - left_rad2 * c_y_2))*2;
			if (0 <= point_c2 && point_c2 <= 13) {
				room_c2[point_c2][i] = (room_c2[point_c2][i] + 1.0) / 2.0;
			}
			else {
				for (int j = 0; j < 13; j++) {
					room_c2[j][i] = room_c2[j][i] / 2.0;
				}
			}
			//printf("test = %d\n",point_c1);
		}
	}


		printf("計算結果\n");
	for (int j = 0; j < 13; j++) {
		for (int k = 0; k < 17; k++) {
			room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2.0;
			//閾値を0.5に仮定
			//if (room[j][k] < 0.4)room[j][k] = 0.0;
			//room[j][k] = (room_c1[j][k] + room_c2[j][k]) / 2.0;
			printf("%2.2lf ", room[j][k]);
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
			/*1回目：ナンバー #2 */
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
		//d_camera(left_x1, left_x2, right_x1, right_x2, height1, height2, output_px, output_py, tmp_pointx, tmp_pointy, rad_room, past_px, past_py, past_ph);
		//d_room(left_x1, left_x2, right_x1, right_x2, height1, height2, output_px, output_py, tmp_pointx, tmp_pointy, room, past_px, past_py, past_ph, rad_room);
		//d_room(left_x1, left_x2, right_x1, right_x2, height1, height2, output_px, output_py, tmp_pointx, tmp_pointy, room, past_px, past_py, past_ph, rad_room);
		camera1(left_x1,right_x1,height1,left_x2,right_x2,height2,room);

		//printf("フレーム切り替え\n");

		cv::Mat img(cv::Size(rec_x2 + 450, rec_y2 + 50), CV_8UC3, cv::Scalar(255, 255, 255));
		cv::namedWindow("ROOM", cv::WINDOW_AUTOSIZE);
		cv::rectangle(img, cv::Point(rec_x1, rec_y1), cv::Point(rec_x2, rec_y2), cv::Scalar(0, 0, 0), 2, 4); //部屋の描写
		//cv::circle(img, cv::Point(c_x_1 * 100.0 + rec_x1, rec_y2 - c_y_1 * 100.0), 10, cv::Scalar(0, 0, 0), 2, 4);//カメラ1
		//cv::putText(img, "camera 1", cv::Point(c_x_1 * 100.0 + 50.0, rec_y2 - c_y_1 * 100.0), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 200), 2, CV_AA);

		//cv::circle(img, cv::Point(c_x_2 * 100.0 + rec_x1, ROOM_H * 100.0 - c_y_2 * 100.0), 10, cv::Scalar(0, 0, 0), 2, 4);//カメラ2
		//cv::putText(img, "camera 2", cv::Point(c_x_2 * 100.0 + 50.0, ROOM_H * 100.0 - c_y_2 * 100.0), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 200), 2, CV_AA);

		//top5(room, person, d_top5);
		//if (d_top5[4] < 0.7)d_top5[3] = 0.5;

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


		//printf("\n");

		//for (int i = 0; output_px[i] != '\0' && output_py[i] != '\0'; i++)
		//{
		//	output_counter++;

		//	fprintf(fp3, "%2.2f ", output_px[i]);
		//	fprintf(fp3, "%2.2f\n", output_py[i]);

		//	//printf("count\n");

		//	flame_pointx[i] = output_px[i];

		//	cv::circle(img, cv::Point(output_px[i] * 100 + rec_x1, rec_y2 - output_py[i] * 100), 7, cv::Scalar(0, 0, 0), -1, CV_AA);//人物位置

		//	sprintf_s(point_text, 255, "%d x:%2.2f  y:%2.2f", i, output_px[i], output_py[i]);
		//	cv::putText(img, point_text, cv::Point(rec_x2 + 30, rec_y1 + (double)i * 30.0), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 200), 2, CV_AA);

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
				if (room[j][k] > 0.0) {
					double wid = 0.5 * k * 50.0 + rec_x1;
					double hei = 0.5 * j * 50.0 + rec_y1;
					cv::Point pt[4];
					pt[0] = cv::Point(wid, hei);
					pt[1] = cv::Point(wid + 50.5, hei);
					pt[2] = cv::Point(wid + 25.5, hei + 25.5);
					pt[3] = cv::Point(wid, hei + 50.5);
					/*pt[2] = cv::Point(400,400);
					pt[3] = cv::Point(500,500);*/
					//printf("img");
					//cv::fillConvexPoly(img, pt, 4, cv::Scalar(255, 255, 185), -1, CV_AA);
					cv::rectangle(img, pt[0], pt[2], cv::Scalar(255, 255, 185), -1, CV_AA);
					//cv::rectangle(img, cv::Point(200, 350), cv::Point(300, 450), cv::Scalar(200, 0, 0), -1, CV_AA);
					//cv::imshow("ROOM", img);
				}
			}
		}

		output_counter++;
		cv::imshow("ROOM", img);
		cv::waitKey(30);
		cv::imwrite("data/output_" + std::to_string(output_counter) + ".png", img);
		//output_counter++;
		////cv::imwrite("data/output_" + std::to_string(output_counter) + ".png", img);
		//cv::imshow("ROOM", img);


		//cv::waitKey(30);

		//fprintf(fp3, "\n");
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	cv::destroyAllWindows();

	return 0;
}