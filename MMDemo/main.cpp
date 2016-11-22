/* MalMaum Experiment Team 02
 * Sound stimuli WAV files recorded from http://www.oddcast.com/home/demos/tts/tts_example.php?sitepal
 * Each set consists of 6 different words with same language
 * PNG files were drawn by own.
 *
 * To run exe file, need 'freeglut.dll', 'glew32.dll', 'glfw3.dll' and vc redist package
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
#define DEBUG

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

DWORD* dwInitVolume;

int ansarr[3][6] = { {1,0,1,1,0,0}, {1,0,1,1,0,0}, {1,0,1,1,0,0}}; // array for real answer
int retarr[3][6] = { {1,1,1,1,1,1}, {1,1,1,1,1,1}, {1,1,1,1,1,1}}; // array for fake return

void Init()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("MalMaum Demo");
	
	glutFullScreen(); // for real exp
#ifdef DEBUG
	glutFullScreenToggle();
#endif /* DEBUG */

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glPointSize(6.0f);

	unsigned int tex;
	int width, height;
	initPNG(&tex, "png\\intro.png", width, height);

	fopen_s(&fd, "output.txt", "w");
}

void prepPic(int _time)
{
	unsigned int tex;
	int width, height;
	initPNG(&tex, "png\\sti_next.png", width, height);

	// Sleep(_time);
}

// result scene for real exp
void waitPicq(int ans) // ans 1 z, ans 0 / 
{
	unsigned int tex;
	int width, height;
	if (checkcnt != -1)
		if (ans == ansarr[setcnt][checkcnt])
			initPNG(&tex, "png\\wait_yes.png", width, height);
		else
			initPNG(&tex, "png\\wait_no.png", width, height);
	else {
		initPNG(&tex, "png\\wait_start.png", width, height);
	}
		
}

// result scene for fake exp
void waitPic()
{
	unsigned int tex;
	int width, height;
	if (checkcnt != -1)
		if (retarr[setcnt][checkcnt] == 1)
			initPNG(&tex, "png\\wait_yes.png", width, height);
		else
			initPNG(&tex, "png\\wait_no.png", width, height);
	else
		initPNG(&tex, "png\\wait_start.png", width, height);
}

void nextPic()
{
	unsigned int tex;
	int width, height;
	char fname[MAX_BUF_SIZE];

	if ((++checkcnt) > 5) { // end of stimulus
		if(setcnt != 2)
			sprintf_s(fname, "png\\sti_end.png", checkcnt);
		else
			sprintf_s(fname, "png\\exp_end.png", checkcnt);
	}
	else {
		sprintf_s(fname, "png\\cross.png", checkcnt);
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

	if (checkcnt > 5) {
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
	if(checkcnt != -1 && setcnt != -1)
		PlaySound(TEXT(fname), NULL, SND_FILENAME | SND_SYNC);

	// current... need volume balancing
	PlaySound("wav\\2_4.wav", NULL, SND_FILENAME | SND_SYNC);
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

	if (checkcnt > 5 && key != 27) {
		// TO-DO to deal with more than 3 sets, change below if condition
		if (setcnt < 3) { // move to next stimulus
			setcnt++;
			checkcnt = -1;
			prepPic(10*SLEEP_TIME);
			DrawTex();
			glutSwapBuffers();
			return;
		}
		else
			return; // do nothing if exp end
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
			sprintf_s(buf, "[%d-%d] _ YES _ %5d milisec\n", setcnt, checkcnt, (end - begin));
			fwrite(buf, sizeof(char), 28, fd);
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
			sprintf_s(buf, "[%d-%d] _ N O _ %5d milisec\n", setcnt, checkcnt, (end - begin));
			fwrite(buf, sizeof(char), 28, fd);
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