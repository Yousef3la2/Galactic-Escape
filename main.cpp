#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/freeglut.h>

float screenx=1.8;
float lespeed=0,lescore=0;
int stop =0;
int level=1;
int ii=0,lview=600;
float dronx = 0,drony = -0.7,dronv=0,defbomb=0.02;
int bomb=0;   //(shooter)
int crush = 0;   // (win or lose)
int score = 0 ;
int ret=0;


typedef struct star{
double x;
double y;
double sizee;
}star;

#define LEN 8192
#define STARS_NUM 600

star a[STARS_NUM];   //(CREATE ARRAY OF STAR STRUCT)


float xball=0,yball=1;
float aball = 1;

typedef struct rings{
double x;
double y;
float outrad;
float inrad;
double speed;
float angel;
}rings;
#define RINGS_NUM 30
rings r[RINGS_NUM];


typedef struct bombs{
double x;
double y;
float bombrad;
}bombs;
bombs b[40];



double RandomNumber(double Min, double Max){
	return ((double(rand()) / double(RAND_MAX)) * (Max - Min)) + Min;
}

void star_info(){              //(create star)
for(int i=0;i<STARS_NUM;i++){
  a[i].x = RandomNumber(-screenx, screenx);
  a[i].y = RandomNumber(-1, 1);
  a[i].sizee=RandomNumber(0, 2);
}
}

float definrad=0.01  //(REDIAS MN BARA (BORDER))
,defoutrad=0.04;     //(REDIAS MN GWA )

void ring_info(){                    //(create ring)
for(int i=0;i<RINGS_NUM;i++){
  r[i].x = RandomNumber(-screenx, screenx);
  r[i].y = RandomNumber(2,3);
  r[i].speed = RandomNumber(0.00010,0.00090);
  r[i].inrad = definrad;
  r[i].outrad = defoutrad;
  r[i].angel = 1;
}
}

void OnMouseClick(int button, int state, int x, int y){   // WHEN TRY AGAIN
  if ((button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) && crush == 1){
    crush=0;
    score=0;
    level=1;
    lescore=0;
    lespeed=0;
    ii=0;
    ring_info();
    dronx = 0,drony = -0.7;
  }
}

//----------------------------------------
bool keys[] = { false,false,false,false };

void windowSpecial(int key,int x,int y){     //(CONTROL AT 4 ARROW)
    if (key == GLUT_KEY_RIGHT)
        keys[1]=true;

    else if (key == GLUT_KEY_LEFT)
        keys[3]=true;

    else if (key == GLUT_KEY_UP )
        keys[0]=true;

    else if (key == GLUT_KEY_DOWN)
        keys[2]=true;

}

void windowSpecialUp(int key,int x,int y) {

    if (key == GLUT_KEY_RIGHT){
            keys[1]=false;
    }

    else if (key == GLUT_KEY_LEFT){
            keys[3]=false;
    }

    else if (key == GLUT_KEY_UP ){
        keys[0]=false;
    }

    else if (key == GLUT_KEY_DOWN){
            keys[2]=false;
    }

}



//----------------------------------------------
void windowKey(unsigned char key,int x,int y){   //(to control in space & esc )

 if (key == 27) exit(0);  // key esc

  else if (key == 32)    // key space
    {
    drony-= 0.13;    // to go down
    ret=1;           // to return up again
    bomb++;           // to shooter
    }
}

void timer(int x){
    if (keys[0])
        if(drony < 0.98)
            drony += 0.003;

    if (keys[1])
        if(dronx < 1.7)
           dronx += 0.003;

    if (keys[2])
        if(drony > -0.98)
            drony -= 0.003;

    if (keys[3])
        if(dronx > -1.7)
            dronx -= 0.003;

    glutPostRedisplay();
    glutTimerFunc(1000/500, timer, 0);
}

double screen=1.0;

void reshape(int width,int height){    // ( reshape a character in screen )
  double w2h = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* orthogonal projection */
  glOrtho(-screen*w2h,+screen*w2h, -screen,+screen, -screen,+screen);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void printv(va_list args, const char* format){   // print counter
  char buf[LEN];
  char* ch=buf;
  vsnprintf(buf,LEN,format,args);
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

void print(const char* format, ...){   // print text
  va_list args;
  va_start(args,format);
  printv(args,format);
  va_end(args);
}

void BackGround(){
 glMatrixMode(GL_MODELVIEW);
  glBegin(GL_QUADS);
  glColor3f(0, 0, 0 );
  glVertex2f( screenx,1 );
  glVertex2f( -screenx,1 );
  glColor3f(0.370, 0.062, 0.415);
  glVertex2f( -screenx,-1 );
  glColor3f(0.074, 0.070, 0.290 );
  glVertex2f( screenx,-1 );
  glEnd();
}

void stars_paint_move(){   //( star paint & moving)
for(int i=0;i<STARS_NUM;i++){
   float cl=RandomNumber(0.5, 1);
   glPointSize(a[i].sizee);
   glBegin(GL_POINTS);
   glColor3f(cl,cl,cl );
   glVertex2f( a[i].x , a[i].y );

   a[i].y -= 0.0001;

   if (a[i].y < -1)
    a[i].y=1;

    glEnd();
}

}

void RingsPaint(int i){   //( ring paint)
   glColor3f(0,0.7,1 );
   glTranslatef(r[i].x,r[i].y,0);
   glRotated(r[i].angel,0,1,0);
   glutSolidTorus(r[i].inrad,r[i].outrad,6,6);
}

void Rings_Move(int i){    // (move ring )
r[i].y-=r[i].speed+lespeed;
   if (r[i].y < -1.2)
  r[i].y = 3 ;
  r[i].angel+=1;
}

void bomb_VS_rings(int i){   // (when bomb touch rings)
if(bomb>=0){
      // glLoadIdentity();
   for(int j=0;j<=bomb;j++){
    if((crush == 0)&&(b[j].y > drony+0.1)&&(b[j].y > r[i].y-0.05)&&(b[j].y < r[i].y+0.05)&&(b[j].x > r[i].x-0.05)&&(b[j].x < r[i].x+0.05)&&(r[i].y<1)){
    r[i].inrad = 0;
    r[i].y = RandomNumber(1.5,3);
    r[i].x = RandomNumber(-1,1);
    r[i].inrad =definrad;
    b[j].y=1;
    score++;
    lescore++;
    stop =0;

    }
   }
   }
}

void dron_VS_rings(int i){    // (when dron touch rings (lose cruch =1))
if (((r[i].x < dronx+0.16) && (r[i].x > dronx-0.16) && (r[i].y < drony+0.05) && (r[i].y > drony-0.05))
     ||((r[i].x < dronx+0.09) && (r[i].x > dronx-0.09) && (r[i].y < drony+0.24) && (r[i].y > drony-0.24))){
        crush = 1;
   }
}

void Level_Speed(){           // level speed
if ((lescore == 10) && (stop == 0)){
    lespeed+=0.0005;
    lescore=0;
    stop = 1;
    level++;
    lview=0;    // view number of level
   }
}



void RingsDetails(){   //( bt loop 3ala el rings)

 for(int i=0;i<RINGS_NUM;i++){
    glPushMatrix();
    RingsPaint(i);
    bomb_VS_rings(i);
    dron_VS_rings(i);
    Level_Speed();
    Rings_Move(i);
    glPopMatrix();
    }

}

void bombsDetails(){  //( bt loop 3ala el bombs)
  if(bomb==38)
    bomb=0;
  b[bomb].x = dronx;
  b[bomb].y = drony;
  for(int i=0;i<=bomb;i++){
  glPushMatrix();
  glTranslatef(b[i].x,b[i].y,0);
  glColor3f(1,0,0);
  glutSolidSphere(0.02,16,16);
  if(b[i].y<1.1)
  b[i].y+=0.02;
  glPopMatrix();
  }
}

void RocketDetails(){    // (create a rocket)
//---------------------fire------------------------

glBegin(GL_POLYGON);
glColor3f(1,1,0);
glVertex2f(dronx,(drony));
glVertex2f(dronx-(RandomNumber(0.04,0.06)),(drony-0.15));
glColor3f(1,0,0);
glVertex2f(dronx,(drony-(RandomNumber(0.2,0.3))));
glColor3f(1,1,0);
glVertex2f(dronx+(RandomNumber(0.04,0.06)),(drony-0.15));
glEnd();

//-------------------------------------------------

//---------------------left side-------------------
  glColor3f(1,0,0);
  glPushMatrix();
  glTranslatef(dronx-0.05,(drony-0.05),0);
  glRotated(-90,1,0,0);
  glutSolidCone(0.07,0.1,4,4);
  glPopMatrix();
//------------------------------------------------

//--------------------right side------------------
  glColor3f(1,0,0);
  glPushMatrix();
  glTranslatef(dronx+0.05,(drony-0.05),0);
  glRotated(-90,1,0,0);
  glutSolidCone(0.07,0.1,4,4);
  glPopMatrix();
//------------------------------------------------

//---------------------tail------------------------
  glColor3f(1,0,0);
  glPushMatrix();
  glTranslatef(dronx,(drony-0.1),0);
  glRotated(-90,1,0,0);
  glutSolidCone(0.05,0.1,4,4);
  glPopMatrix();
//-------------------------------------------------


//------------------body---------------------

  //glLoadIdentity();
  glColor3f(1,1,1);
  glPushMatrix();
  glTranslatef(dronx,drony,0);
  glutSolidCube(0.1);
  glPopMatrix();
//-------------------------------------------------


//----------------------head-----------------------
  glColor3f(1,1,1);
  glPushMatrix();
  glTranslatef(dronx,(drony+0.05),0);
  glRotated(-90,1,0,0);
  glutSolidCone(0.05,0.15,40,40);
  glPopMatrix();
//-------------------------------------------------


//----------------------WINDOW-----------------------
  glColor3f(0,0,1);
  glPushMatrix();
  glTranslatef(dronx,(drony+0.1),0);
  glutSolidSphere(0.02,16,16);
  glPopMatrix();
//-------------------------------------------------


//----------------------lines-----------------------

  glColor3f(1,0,0);
  glPushMatrix();

  glBegin(GL_QUADS);
  glVertex2f(dronx+0.03,drony+0.025);
  glVertex2f(dronx-0.03,drony+0.025);
  glVertex2f(dronx-0.03,drony+0.01);
  glVertex2f(dronx+0.03,drony+0.01);

  glVertex2f(dronx+0.03,drony-0.025);
  glVertex2f(dronx-0.03,drony-0.025);
  glVertex2f(dronx-0.03,drony-0.01);
  glVertex2f(dronx+0.03,drony-0.01);
  glEnd();
  glPopMatrix();
//-------------------------------------------------
}

void ViewScoreAndLevel(){
glColor3f(1.0,1.0,1.0);
  //glLoadIdentity();
  glRasterPos3d(-1.75,0.9,0);
  print("Score : %d",score);
  glRasterPos3d(-1.5,0.9,0);
  print("Level : %d",level);
}

void GameOver(){
if (crush ==1){
    glBegin(GL_QUADS);
    glColor3f(0,0,0);
    glVertex2f(screenx,1);
    glVertex2f(-screenx,1);
    glVertex2f(-screenx,-1);
    glVertex2f(screenx,-1);
    glEnd();
    glColor3f(1.0,1.0,1.0);
    glRasterPos3d(-0.18,0.3,0);
    print("Game Over");
    glRasterPos3d(-0.36,0.2,0);
    print("*********************************");
    glRasterPos3d(-0.3,0.1,0);
    print("Level :                         %d",level);
    glRasterPos3d(-0.3,0,0);
    print("Score :                        %d",score);
    glRasterPos3d(-0.36,-0.1,0);
    print("*********************************");
    glRasterPos3d(-0.3,-0.2,0);
    print("Right click : for try again ");
    glRasterPos3d(-0.3,-0.3,0);
    print("Esc : for exit");
}
}

void StartCounter(){
if(ii<2000){
    glColor3f(1.0,1.0,0);
    glRasterPos3d(-0.02,0,0);
    if(ii<500){
    print("3");
    }
    else if(ii<1000){
    print("2");
    }
    else if(ii<1500){
        print("1");
    }
    else {
    glRasterPos3d(-0.03,0,0);
    print("GO");
    }
    ii++;
}
}

void display()
{

  if(ret<14){    // to return to the same position after fight
    drony+= 0.01;
    ret++;
  }


  BackGround();
  stars_paint_move();
  RingsDetails();
  bombsDetails();
  RocketDetails();
  ViewScoreAndLevel();
  StartCounter();

    if(lview<500){
    glColor3f(1.0,1.0,0);
    glRasterPos3d(-0.2,0,0);
    print("Level %d",level);
    }
    lview++;

  GameOver();
  glutSwapBuffers();

}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  star_info();
  ring_info();
  glutInitWindowSize(700,700);
  glutCreateWindow("Space War  ");
  glutIgnoreKeyRepeat(3);
  glutSpecialFunc(windowSpecial);
  glutSpecialUpFunc(windowSpecialUp);

  glutTimerFunc(0, timer, 0);  // (TIMER TO START)
  glutKeyboardFunc(windowKey); //(SPACE AND ESC)
  glutDisplayFunc(&display);
  glutReshapeFunc(reshape);  //(RECHAPE EL CHARACTERS & SET SCORE & LEVEL)
  glutMouseFunc(OnMouseClick); // (PLAY AGAIN)
  glutFullScreen();  // (FULL SCREEN)
  glutMainLoop(); // (TO RUN PROGRAM)

  return EXIT_SUCCESS;
}


