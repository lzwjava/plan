/*
 *  clip.cpp
 *  This program clips the line with the given window.
 */
#include <windows.h>
#include <glut.h>
#include <math.h>
#define LEFT 1//0001
#define RIGHT 2//0010
#define BOTTOM 4//0100
#define TOP 8//1000
#define XL 0.2f
#define XR 0.8f
#define YB 0.3f
#define YT 0.7f
//参考mfc中的方法选择定义接受和丢弃
#define REJECT(a,b)    (a&b)
#define ACCEPT(a,b)    (!(a|b))
#define INSIDE(a)      (!a)
double e=0.2;
void myinit(void)
{
    glShadeModel (GL_FLAT);
    glClearColor (0.0, 0.0, 0.0, 0.0);
}
 float x1 = 0.1f;
 float y1 = 0.5f;
 float x2 = 0.9f;
 float y2 = 0.8f;


void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);//表示这个视口矩形的宽度和高度，根据窗口的实时变化重绘窗口。
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) 
	gluOrtho2D (0.0, 1.0, 0.0, 1.0*(GLfloat)h/(GLfloat)w);//二维图像投影矩阵,参数 left,right指明平面的左边和右边的垂直坐标;参数bottom,top指明平面底部和顶部的水平坐标。
    else 
	gluOrtho2D (0.0, 1.0*(GLfloat)w/(GLfloat)h, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

//-------------------------------
// your task!!!
//-------------------------------
/*
//////梁友栋－barsky算法
bool Clip_LB(float p, float q, float *u1, float *u2)
{ 
	float r; 
	if(p < 0)
	{ 
		r=q/p;
		if(r > *u2)
			return FALSE;
		else if(r > *u1)
		{ 
			*u1=r;
			return TRUE;
		}
	}
	else if(p > 0)
	{ 
		r=q/p;
		if(r < *u1) 
			return FALSE;
		else if(r < *u2)
		{ 
			*u2=r;
			return TRUE;
		}
	}
	else 
		if(q < 0) 
			return FALSE;
	return TRUE;
}

BOOL LB_LineClip( float XL, float XR, float YB, float YT,float &x1, float &y1, float &x2, float &y2)
// Input: 裁剪窗口的对角顶点(XL,YB)和(XR,YT); 线段起点(x1,y1)和终点(x2,y2)
// Output:裁剪后的线段起点(x1,y1)和终点(x2,y2)
// 返回值: 1) false -- 与裁剪窗口没有交; 
// 2) true -- 线段的一部分或者全部位于裁剪窗口内。
{ 
	float dx,dy,u1,u2;
	u1=0;
	u2=1;
	dx =x2-x1;
	if(ClipTesting(-dx,x1-XL,&u1,&u2))
	if(ClipTesting(dx,XR-x1, &u1,&u2))
	{
	dy =y2-y1;
	if(ClipTesting(-dy,y1-YB, &u1,&u2))
	if(ClipTesting(dy,YT-y1, &u1,&u2))
	{ 
		if (u2 < 1.0)
	{
		x2 = x1+u2*dx;
		y2 = y1+u2*dy;
	}
	if (u1 > 0.0)
{
x1 += u1*dx;
y1 += u1*dy;
}
return TRUE;
}
}
return FALSE;
}
*/


//中点分割法
int encode(int x,int y)
{
	//或者定义为unsigned char code=0x00;则函数返回类型也变
	int c=0;
	//如果在左侧，按位与“0001” 
	if(x<XL)
		c=c|LEFT;
	//如果在右侧，按位与“0010”
	else if(x>XR)
		c=c|RIGHT;
	//如果在上面,按位与“1000”
	if(y>YT)
		c=c|TOP;
	//如果在下面,按位与“0100”
	else if(y<YB)
		c=c|BOTTOM;
	return c;

}


void myclip()
// line clipping algorithm 
{
int code1, code2,codep=0;
//初始化中点
float x=0.0f,y=0.0f;
//为p1,p2编码得c1,c2
code1 = encode(x1,y1);
code2 = encode(x2,y2); 



//3种情况
if(ACCEPT(code1,code2))//1.完全可见
{//（1）C1＝C2＝0，表明两端点全在窗口内，因而整个线段也在窗内，应予保留。
	 glColor3f (255.0, 255.0, 0.0);//可不用透明度着色，单纯设定RGB
	 glLineWidth(10.0f);//指定线宽  
	 glBegin(GL_LINES);//画直线，定义两个端点
	    glVertex2f( x1, y1); // 线段起点
		glVertex2f( x2, y2); // 线段终点
	 glEnd();
}
else if(REJECT(code1,code2))//2.完全不可见
{
//（2）C1&C2≠0,即C1和C2至少有某一位同时为1，表明两端点必定处于某一边界的同一外侧(上下左右四个边界)，因而整个线段全在窗外，应予舍弃。
	//试试不操作
}
else//3.其他
{
	///////////////77
  if(INSIDE(code1))//p1在窗口内，则交换p1,p2,保证p1为窗口外的点,code值也要换
 {
	 float temp_x,temp_y,temp_c;
	 temp_x=x1;
	 x1=x2;
	 x2=temp_x;
	 temp_y=y1;
	 y1=y2;
	 y2=temp_y;
	 temp_c=code1;
	 code1=code2;
	 code2=temp_c;
 }
//暂存器
float p1_x=x1;
float p1_y=y1;
float p2_x=x2;
float p2_y=y2;
float p_x=x;
float p_y=y;
int c1=code1;
int c2=code2;
int cp=codep;


	do
	{
		code1=encode(x1,y1);
		x = (x1+x2)/2;
		y = (y1+y2)/2;
		codep = encode(x,y);
		if(REJECT(code1,codep))//若P1和P同侧，移动P1点；　
		{
			x1=x;
			y1=y;
		}
		else
		{
			x2=x;
			y2=y;
		}
	}while((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)>=e);//直到P1和P2相差一个单位时
 	
	//令交点为P2
	float k;//斜率
	bool notVertical=0;
	if(x2!=x1)//有斜率算出斜率m
	{
		k=(y2-y1)/(x2-x1);
		notVertical = 1;
	}
	if ( notVertical && code2 & LEFT)//点在左边线外侧，求出与左边线交点，并赋予p2
			{
				y2=k*x2+(y-k*x);
				x2=LEFT;
				
			}
			else if( notVertical &&  code2 & RIGHT) //点在右边线外侧，求出与右边线交点，并赋予p2
			{
				y2=k*x2+(y-k*x);
				x2=RIGHT;
			}
			else if(code2 & BOTTOM)//点在下边线外侧，求出与下边线交点，并赋予p2
			{
				if(x2!=x1) //对于垂直于X轴的直线，斜率为无穷大，交点x坐标与p2点相等
					x2=(y2-(y-k*x))/k;
				y2=BOTTOM;
			}
			else if(code2 & TOP) //点在上边线外侧，求出与上边线交点，并赋予p2
			{ 
			if(x2!=x1) //对于垂直于X轴的直线，斜率为无穷大，交点x坐标与p2点相等
					x2=(y2-(y-k*x))/k;
				y2=TOP;
			}
	//取出暂存器的端点赋给P1
	x1=p2_x;
	y1=p2_y;
	 if(INSIDE(code1))//p1在窗口内，则交换p1,p2,保证p1为窗口外的点,code值也要换
 {
	 float temp_x,temp_y,temp_c;
	 temp_x=x1;
	 x1=x2;
	 x2=temp_x;
	 temp_y=y1;
	 y1=y2;
	 y2=temp_y;
	 temp_c=code1;
	 code1=code2;
	 code2=temp_c;
 }

	do
	{
		
		code1=encode(x1,y1);
		x = (x1+x2)/2;
		y = (y1+y2)/2;
		codep = encode(x,y);
		if(REJECT(code1,codep))//若P1和P同侧，移动P1点；　
		{
			x1=x;
			y1=y;
		}
		else
		{
			x2=x;
			y2=y;
		}
	}while((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)>=e);//直到P1和P2相差一个单位时
}

/*///////////////////参考
float p1_x=x1;
float p1_y=y1;
float p2_x=x2;
float p2_y=y2;
float p_x=x;
float p_y=y;
int c1=code1;
int c2=code2;
int cp=codep;
//找离P1最近的窗口交点A
		do
		{
			//P1P完全可见
			if(ACCEPT(code1,codep))
			{
				//将在窗口内的线标示为红色
				glLineWidth(3.0);
				glColor4f (1.0, 0.0, 0.0, 1); 
				glBegin(GL_LINES);
					glVertex2f(x1, y1);
					glVertex2f(x, y);
				glEnd();
				//中点P代替P1
				x1=x;
				y1=y;
				code1=codep;
				//重新计算M点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
			else if(REJECT(code1,codep))
			{
				//画绿线表示不在裁剪窗口内部
				glLineWidth(2.0);
				glColor4f (0.0, 1.0, 0.0, 1); 
				glBegin(GL_LINES);
					glVertex2f(x, y);
					glVertex2f(x1,y1);
				glEnd();
				//中点P代替P1
				x1=x;
				y1=y;
				code1=codep;
				//重新计算P点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
			else
			{
				//中点P代替P2
				x2=x;
				y2=y;
				code2=codep;
				//重新计算P点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
		}while((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)>=e);


		//找到与窗口交点A
		float PA_x=x1;
		float PA_y=y1;

		//重新计算P0、P1及M点位置
x1=p1_x;
y1=p1_y;
x2=p2_x;
y2=p2_y;
x=p_x;
y=p_y;
code1=c1;
code2=c2;
codep=cp;
		//找离P2最近的窗口交点B
		do
		{
			//P2P完全可见
			if(ACCEPT(code2,codep))
			{	
				//将在窗口内的线标示为红色
				glLineWidth(3.0);
				glColor4f (1.0, 0.0, 0.0, 1); 
				glBegin(GL_LINES);
					glVertex2f(x, y);
					glVertex2f(x2, y2);
				glEnd();
				//中点P代替P2
				x2=x;
				y2=y;
				code2=codep;
				//重新计算P点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
			else if(REJECT(code2,codep))
			{
				//画绿线表示不在裁剪窗口内部
				glLineWidth(2.0);
				glColor4f (0.0, 1.0, 0.0, 1); 
				glBegin(GL_LINES);
					glVertex2f(x, y);
					glVertex2f(x2, y2);
				glEnd();

				//中点P代替P2
				x2=x;
				y2=y;
				code2=codep;
				//重新计算P点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
			else
			{
				//中点P代替P1
				x1=x;
				y1=y;
				code1=codep;
				//重新计算P点位置
				x=(x1+x2)/2;
				y=(y1+y2)/2;
				codep=encode(x, y);
			}
		}while((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)>=e);
		//找到与窗口交点B
		float PB_x=x2;
		float PB_y=y2;

		//画红线表示在裁剪窗口内部
		glLineWidth(3.0);
		glColor4f (1.0, 0.0, 0.0, 1); 
		glBegin(GL_LINES);
			glVertex2f(PA_x, PA_y);
			glVertex2f(PB_x, PB_y);
		glEnd();
	}
	*/
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
// The window
// ------------------------------------
//  you can change the window definition on yourself.
// ------------------------------------
	//多边形
	glColor4f (0.0, 1.0, 1.0, 0.75);//RGB+alpha
	glBegin(GL_POLYGON);//画多边形，定义四个顶点
	    glVertex2f( 0.2f, 0.3f); // Bottom Left
		glVertex2f( 0.8f, 0.3f); // Bottom Left
		glVertex2f( 0.8f, 0.7f); // Bottom Right
		glVertex2f( 0.2f, 0.7f); // Bottom Right
	glEnd();
// ------------------------------------
//	please define your own line segment and draw 
//	it here with different color and line width
// ------------------------------------
	//线段
	glColor3f (255.0, 0.0, 0.0);//可不用透明度着色，单纯设定RGB
	glLineWidth(5.0f);//指定线宽  
	glBegin(GL_LINES);//画直线，定义两个端点
	    glVertex2f( x1, y1); // 线段起点
		glVertex2f( x2, y2); // 线段终点
	glEnd();

 	//-------------------------------
	//do the clipping in myclip() funtion 
	//-------------------------------
   myclip();
// ------------------------------------
//	please draw clipped line here with another 
//  color and line width
// ------------------------------------ 
  glColor3f (255.0, 255.0, 0.0);//可不用透明度着色，单纯设定RGB
	 glLineWidth(10.0f);//指定线宽  
	 glBegin(GL_LINES);//画直线，定义两个端点
	    glVertex2f( x1, y1); // 线段起点
		glVertex2f( x2, y2); // 线段终点
	 glEnd();
	 
    glFlush();
}


/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);
   //define size and the relative positon of the applicaiton window on the display
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
 	//init the defined window with "argv[1]" as topic showed on the top the window
   glutCreateWindow (argv[0]);
	// opengl setup
   myinit ();

	//define callbacks
   glutDisplayFunc(display); 
   glutReshapeFunc(myReshape);
   //enter the loop for display
   glutMainLoop();

	return 1;
}