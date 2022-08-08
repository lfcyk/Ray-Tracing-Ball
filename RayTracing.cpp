#define _CRT_SECURE_NO_WARNINGS
#define SCREEN_SIZE_X 500
#define SCREEN_SIZE_Y 500

#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

double dViewPos[3], dCrossColor[3], dCrossPos[3], dTargetPos[3], dScreenSize[2], dLightNormVect[3], dLightColor[3], dSphereCenter[3], dSphereRadius, dSphereColor[3], dIColor[3];

int ReadData(void)
{
	char strBuf[256];

	FILE* pf;
	int nflag = 0;
	pf = fopen("ViewVolume.txt", "r");
	if (pf == NULL) return 0;
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dViewPos[0], &dViewPos[1], &dViewPos[2]);
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dTargetPos[0], &dTargetPos[1], &dTargetPos[2]);
	nflag = fscanf(pf, "%s %lf,%lf", strBuf, &dScreenSize[0], &dScreenSize[1]);
	fclose(pf);


	pf = fopen("Light.txt", "r");
	if (pf == NULL) return 0;
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dLightNormVect[0], &dLightNormVect[1], &dLightNormVect[2]);
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dLightColor[0], &dLightColor[1], &dLightColor[2]);
	fclose(pf);

	pf = fopen("Sphere.txt", "r");
	if (pf == NULL) return 0;
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dSphereCenter[0], &dSphereCenter[1], &dSphereCenter[2]);
	nflag = fscanf(pf, "%s %lf", strBuf, &dSphereRadius);
	nflag = fscanf(pf, "%s %lf,%lf,%lf", strBuf, &dSphereColor[0], &dSphereColor[1], &dSphereColor[2]);
	fclose(pf);
}

int CalCrossSectionSphere(double* pdSt, double* pdEd, double* pdP) {
	double A, B, C, D, t, L;

	L = sqrt((pdEd[0] - pdSt[0]) * (pdEd[0] - pdSt[0]) + (pdEd[1] - pdSt[1]) * (pdEd[1] - pdSt[1]) + (pdEd[2] - pdSt[2]) * (pdEd[2] - pdSt[2]));
	double R[3] = { (pdEd[0] - pdSt[0]) / L,(pdEd[1] - pdSt[1]) / L,(pdEd[2] - pdSt[2]) / L };
	A = 1;
	B = 2 * R[0] * (pdSt[0] - dSphereCenter[0]) + 2 * R[1] * (pdSt[1] - dSphereCenter[1]) + 2 * R[2] * (pdSt[2] - dSphereCenter[2]);
	C = (pdSt[0] - dSphereCenter[0]) * (pdSt[0] - dSphereCenter[0]) + (pdSt[1] - dSphereCenter[1]) * (pdSt[1] - dSphereCenter[1]) + (pdSt[2] - dSphereCenter[2]) * (pdSt[2] - dSphereCenter[2]) - pow(dSphereRadius, 2);

	D = B * B - 4 * A * C;
	if (D > 0) {
		t = (-B - sqrt(D)) / 2 * A;
		for (int i = 0; i < 3; i++) {
			pdP[i] = pdSt[i] + t * R[i];
		}
		return 1;
	}
	else if (D == 0) {
		t = (-B) / 2 * A;
		for (int i = 0; i < 3; i++) {
			pdP[i] = pdSt[i] + t * R[i];
		}
		return 1;
	}
	else {
		return 0;
	}
}

void CalCrossColor(double* Lp, double* Lc, double* Pp, double* Pc, double* Ic) {
	double N[3] = { (Pp[0] - dSphereCenter[0]) / dSphereRadius,(Pp[1] - dSphereCenter[1]) / dSphereRadius,(Pp[2] - dSphereCenter[2]) / dSphereRadius };

	for (int i = 0; i < 3; i++) {
		Lp[i] = -Lp[i];
	}

	double C = Lp[0] * N[0] + Lp[1] * N[1] + Lp[2] * N[2];

	if (C < 0) {
		for (int i = 0; i < 3; i++) {
			Ic[i] = 0;
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
			Ic[i] = C * Lc[i] * Pc[i];
		}

	}

}

// ユーザの初期化
void Initialize(void)
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					// 表示モードの指定
	glutInitWindowSize(500, 500);									// 画面サイズの指定
	glutInitWindowPosition(100, 100);								// ウインドウ位置の指定
	glutCreateWindow("OpenGL Win32 2D Base Program");				// ウインドウの作成とタイトル設定
	glClearColor(0.0, 0.0, 0.0, 1.0);								// ウインドウの背景色の設定
	glOrtho(-500 / 2, 500 / 2, -500 / 2, 500 / 2, -1, 1);			// 平行投影
}

// 描画関数
void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);									// 画面のクリア
	glBegin(GL_POINTS);												// 点を書く
	for (int j = -250; j < 250; j++) {
		for (int i = -250; i < 250; i++) {
			dTargetPos[1] = j;
			dTargetPos[2] = i;
			if (CalCrossSectionSphere(dViewPos, dTargetPos, dCrossPos)) {
				CalCrossColor(dLightNormVect, dLightColor, dCrossPos, dSphereColor, dCrossColor);
				glColor3d(dCrossColor[0], dCrossColor[1], dCrossColor[2]);
				glVertex2f(i, j);
			}
			else {
				glColor3d(0, 0, 1);
				glVertex2f(i, j);
			}
		}
	}
	glEnd();														// 直線データの終了
	glFlush();														// 描画処理実行
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);											// GLUTの初期化
	ReadData();
	Initialize();													// ユーザの初期化関数
	glutDisplayFunc(draw);											// 描画関数の指定
	glutMainLoop();													// GLUTのメインループ
	return 0;
}
