#include <GL/glut.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

using namespace std;

#define GRID_WIDTH 20
#define GRID_HEIGHT 20

struct SnakeSegment
{
    float x;
    float y;
};

vector<SnakeSegment> snake;

float foodX, foodY;

int dir = 1;
int score = 0;
int speed = 150;

bool gameOver = false;
bool paused = false;

void drawCircle(float x,float y,float r)
{
    glBegin(GL_POLYGON);

    for(int i=0;i<40;i++)
    {
        float angle = 2 * 3.1416 * i / 40;
        glVertex2f(x + r*cos(angle), y + r*sin(angle));
    }

    glEnd();
}

void drawGrid()
{
    glColor3f(0.2,0.2,0.2);

    glBegin(GL_LINES);

    for(int i=0;i<=GRID_WIDTH;i++)
    {
        glVertex2f(i,0);
        glVertex2f(i,GRID_HEIGHT);
    }

    for(int i=0;i<=GRID_HEIGHT;i++)
    {
        glVertex2f(0,i);
        glVertex2f(GRID_WIDTH,i);
    }

    glEnd();
}

void drawApple(float x,float y)
{
    glColor3f(1,0,0);
    drawCircle(x+0.5,y+0.5,0.4);

    glColor3f(0,1,0);

    glBegin(GL_TRIANGLES);

        glVertex2f(x+0.5,y+0.9);
        glVertex2f(x+0.6,y+1.1);
        glVertex2f(x+0.4,y+1.1);

    glEnd();
}

void drawSnake()
{
    for(int i=0;i<snake.size();i++)
    {
        if(i==0)
            glColor3f(0,0.6,0);
        else
            glColor3f(0,1,0);

        drawCircle(snake[i].x+0.5,snake[i].y+0.5,0.45);
    }

    // Draw snake eyes
    glColor3f(1,1,1);

    drawCircle(snake[0].x+0.35,snake[0].y+0.7,0.07);
    drawCircle(snake[0].x+0.65,snake[0].y+0.7,0.07);

    // Draw tongue
    glColor3f(1,0,0);

    glBegin(GL_TRIANGLES);

        glVertex2f(snake[0].x+0.5,snake[0].y+0.2);
        glVertex2f(snake[0].x+0.4,snake[0].y);
        glVertex2f(snake[0].x+0.6,snake[0].y);

    glEnd();
}

void drawText(float x,float y,const char* text)
{
    glRasterPos2f(x,y);

    while(*text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*text);
        text++;
    }
}

void spawnFood()
{
    foodX = rand()%GRID_WIDTH;
    foodY = rand()%GRID_HEIGHT;
}

void resetGame()
{
    snake.clear();

    SnakeSegment head;

    head.x = GRID_WIDTH/2;
    head.y = GRID_HEIGHT/2;

    snake.push_back(head);

    dir = 1;
    score = 0;
    gameOver = false;
    paused = false;

    speed = 150;

    spawnFood();
}

void update(int value)
{
    if(gameOver || paused)
    {
        glutTimerFunc(speed,update,0);
        glutPostRedisplay();
        return;
    }

    for(int i=snake.size()-1;i>0;i--)
        snake[i] = snake[i-1];

    if(dir==0) snake[0].y++;
    if(dir==1) snake[0].x++;
    if(dir==2) snake[0].y--;
    if(dir==3) snake[0].x--;

    if(snake[0].x<0 || snake[0].x>=GRID_WIDTH ||
       snake[0].y<0 || snake[0].y>=GRID_HEIGHT)
        gameOver = true;

    for(int i=1;i<snake.size();i++)
        if(snake[0].x==snake[i].x && snake[0].y==snake[i].y)
            gameOver = true;

    if(snake[0].x==foodX && snake[0].y==foodY)
    {
        Beep(800,120);

        SnakeSegment newPart = snake.back();
        snake.push_back(newPart);

        score +=10;

        if(score%50==0 && speed>60)
            speed -=10;

        spawnFood();
    }

    glutPostRedisplay();
    glutTimerFunc(speed,update,0);
}

void specialKeys(int key,int x,int y)
{
    if(key==GLUT_KEY_UP && dir!=2) dir=0;
    if(key==GLUT_KEY_RIGHT && dir!=3) dir=1;
    if(key==GLUT_KEY_DOWN && dir!=0) dir=2;
    if(key==GLUT_KEY_LEFT && dir!=1) dir=3;
}

void keyboard(unsigned char key,int x,int y)
{
    if(key=='r' || key=='R')
        resetGame();

    if(key=='p' || key=='P')
        paused = !paused;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawGrid();
    drawSnake();
    drawApple(foodX,foodY);

    char scoreText[50];
    sprintf(scoreText,"Score: %d",score);

    glColor3f(1,1,1);
    drawText(0.5,GRID_HEIGHT+0.5,scoreText);

    if(paused)
        drawText(7,10,"PAUSED");

    if(gameOver)
    {
        glColor3f(1,0,0);
        drawText(6,10,"GAME OVER");
        drawText(4,8,"Press R to Restart");
    }

    glFlush();
}

void init()
{
    glClearColor(0,0,0,1);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0,GRID_WIDTH,0,GRID_HEIGHT+2);

    resetGame();
}

int main(int argc,char** argv)
{
    srand(time(0));

    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(600,600);

    glutCreateWindow("OpenGL Snake Game");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    glutTimerFunc(speed,update,0);

    glutMainLoop();

    return 0;
}