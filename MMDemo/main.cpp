/* MalMaum Experiment Team 02
 * Sound stimuli WAV files recorded from http://www.oddcast.com/home/demos/tts/tts_example.php?sitepal
 * Each set consists of 6 different words with same language
 * PNG files were drawn by own.
 */

#include <gl/freeglut.h>
#include "texture.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// Setting DEBUG flag will print subject's core data
// #define DEBUG

#define MAX_BUF_SIZE 128
#define SLEEP_TIME 1000 // msec

FILE* fd;
int width = 600, height = 600;
int cflag = -1; // flag that exp is running
int qflag = -1; // flag that exp is on control(1) or not(-1).
int checkcnt = -1; // cnt for each language
int setcnt = -1; // cnt for language dist
int atStart = -1; // flag to show splash screen
clock_t begin, end;


int ansarr[2][9] = { {1,0,1,1,0,0,1,0,1}, {1,0,0,1,0,1,1,0,0} }; // array for real answer
int retarr[2][9] = { {1,0,1,1,1,1,1,1,1}, {1,1,1,1,1,1,0,1,1} }; // array for fake return

void Init()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("MalMaum Demo");

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glPointSize(6.0f);

	unsigned int tex;
	int width, height;
	initPNG(&tex, "png\\ready.png", width, height);

	fopen_s(&fd, "output.txt", "w");
}

void prepPic(int _time)
{
	unsigned int tex;
	int width, height;
	initPNG(&tex, "png\\prep.png", width, height);

	// Sleep(_time);
}


// result scene for real exp
void waitPicq(int ans) // ans 1 z, ans 0 / 
{
	unsigned int tex;
	int width, height;
	if (checkcnt != -1)
		if (ans == ansarr[setcnt][checkcnt])
			initPNG(&tex, "png\\wait.png", width, height);
		else
			initPNG(&tex, "png\\waitX.png", width, height);
	else {
		initPNG(&tex, "png\\fwait.png", width, height);
	}
		
}


// result scene for fake exp
void waitPic()
{
	unsigned int tex;
	int width, height;
	if (checkcnt != -1)
		if (retarr[setcnt][checkcnt] == 1)
			initPNG(&tex, "png\\wait.png", width, height);
		else
			initPNG(&tex, "png\\waitX.png", width, height);
	else
		initPNG(&tex, "png\\fwait.png", width, height);
}

void nextPic()
{
	unsigned int tex;
	int width, height;
	char fname[MAX_BUF_SIZE];

	if ((++checkcnt) > 8) { // end of stimulus
		sprintf_s(fname, "png\\gg.png", checkcnt);
	}
	else {
		sprintf_s(fname, "png\\%d.png", checkcnt);
	}

	initPNG(&tex, fname, width, height);

	/* Note that time count start from exactly after from sound start.
	* So, need to ignore length of sound file
	* SND_SYNC option maybe ...?*/
		
	cflag = -1;
}

// draw saved texture by full size
void DrawTex()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f((float)width, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f((float)width,(float)height);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, (float)height);		
		glEnd();
	glDisable(GL_TEXTURE_2D);
}

void displayCallback()
{
	glViewport(0, 0, width, height);
	DrawTex();
	glutSwapBuffers();

	char fname[MAX_BUF_SIZE];
	sprintf_s(fname, "wav\\%d_%d.wav", setcnt, checkcnt);

	if (checkcnt > 8) {
		// TO-DO to deal with more than 3 sets, change below if condition
		// but may need nothing to do for prepare at DisplayCallback...
		if (setcnt == 0)
			return; // TO-DO prepare for next set...
		else if (setcnt == 1)
			return;
	}
	if (atStart == -1) {
		atStart = 1;
		return;
	}

	/* Note that need SND_SYNC ... wait for play end.
	 * for accurate time result, wav file need precise end cut.
	 */
	PlaySound(TEXT(fname), NULL, SND_FILENAME | SND_SYNC);

#ifdef DEBUG
	std::cout << "[SOUND END]" << std::endl;
#endif /* DEBUG */
	begin = clock();
#ifdef DEBUG
	std::cout << "clock start at " << (begin) << std::endl;
#endif /* DEBUG */
}


void reshapeCallback(int nw, int nh)
{
	width = nw;
	height = nh;
}

void ignoreCallback(unsigned char key, int x, int y) {
	// do nothing
}

void keyboardCallback(unsigned char key, int x, int y)
{
	char buf[MAX_BUF_SIZE];

	if (checkcnt > 8 && key != 27) {
		// TO-DO to deal with more than 3 sets, change below if condition
		if (setcnt == 0) { // move to next stimulus
			setcnt = 1;
			checkcnt = -1;
			prepPic(10*SLEEP_TIME);
			DrawTex();
			glutSwapBuffers();
			return;
		}
		else
			return; // do nothing if 2nd set
	}

	// setcnt/qflag initialize
	if (setcnt == -1) {
		if (key == 'z') // z input for controlled exp
			qflag = 1;
		setcnt = 0; 
#ifdef DEBUG
			printf("[EXP]qflag = %d\n", qflag);
#endif /* DEBUG */
	} // end initialized

	if (key == 27) {
		fclose(fd);
		exit(0);
	}
	else if (key == 'z' || key == 'Z') {
		end = clock();
		if (cflag == -1 && checkcnt != -1) {
#ifdef DEBUG
			std::cout << "[YES] [" << checkcnt << "] " << (end - begin) << std::endl;
			std::cout << "Real answer is " << ansarr[setcnt][checkcnt] << std::endl;
#endif /* DEBUG */
			sprintf_s(buf, "[%d] _ YES _ %d milisec\n", checkcnt, (end - begin));
			fwrite(buf, sizeof(char), 26, fd);
		}	
		cflag = 1;
		(qflag==1 ? waitPicq(1) : waitPic());
		DrawTex();
		glutSwapBuffers();

		// change keyCallBack func as ignoreCallBack to ignore key input while sleep
		glutKeyboardFunc(ignoreCallback);
		Sleep(SLEEP_TIME);
		glutKeyboardFunc(keyboardCallback);

		nextPic();
	}		
	else if (key == '/' || key == '?'){
		end = clock();
		if (cflag == -1 && checkcnt != -1) {
#ifdef DEBUG
			std::cout << "[NO] [" << checkcnt << "] " << (end - begin) << std::endl;
#endif /* DEBUG */
			sprintf_s(buf, "[%d] _ NO _ %d milisec\n", checkcnt, (end - begin));
			fwrite(buf, sizeof(char), 26, fd);
		}
		cflag = 1; 
		(qflag==1 ? waitPicq(0) : waitPic());
		DrawTex();
		glutSwapBuffers();

		// change keyCallBack func as ignoreCallBack to ignore key input while sleep
		glutKeyboardFunc(ignoreCallback);
		Sleep(SLEEP_TIME);
		glutKeyboardFunc(keyboardCallback);

		nextPic();
	}
	
	glutPostRedisplay();
}


void mouseCallback(int button, int action, int x, int y)
{
	// no need for mouse callback
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	Init();
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMainLoop();
	return 0;
}