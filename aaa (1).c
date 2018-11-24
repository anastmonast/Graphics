#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glu.h>    
#include <GL/gl.h>
#include <GL/glut.h> 
#include <stdbool.h>
#include <stdlib.h>
//#include <windows.h>

#define POLYGON 0
#define CLIPPING 1
#define EXTRUDE 2
#define EXIT 3
#define LINECOLOR 4
#define FILLCOLOR 5


#define WIDTH 600
#define HEIGHT 500
#define one 1
#define EP 0.000001

//LINE_COLOR & FILL_COLOR menu defines
#define WHITE 1         //glColor3f(1, 1, 1);
#define BLACK 2      	//glColor3f(0, 0, 0);
#define RED 3           //glColor3f(1, 0, 0);
#define DARK_GREEN 4  	//glColor3f(0, 0.5, 0);
#define LIGHT_GREEN 5 	//glColor3f(0, 1, 0);
#define BLUE 6     		//glColor3f(0, 0, 1);
#define AQUA 7   		//glColor3f(0, 1, 1);
#define PINK 8    		//gcColor3f(1, 0, 1);
#define SOFT_PINK 9 	//glColor3f(1, 0.8, 0.9);
#define PURPLE 10  		//glColor3f(0.5, 0, 1);
#define BROWN 11   		//glColor3f(0.5, 0.2, 0);
#define YELLOW 12  		//glColor3f(1, 1, 0);
#define GRAY 13    		//glColor3f(0.5, 0.5, 0.5);
#define ORANGE 14  		//glColor3f(1, 0.5, 0);
#define GOLD 15  		//glColor3f(1, 0.85, 0);
#define BEIGE 16 		//glColor3f(0.95, 0.95, 0.85);

int window, polygons, w, h, yiot;;
int drawingstopped = 0;
int k = 0;
int numofPol = 0;
int new_vertex;

bool letsTriangle = false;

bool LineIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
 
void render_scene (void);
void drawLines();

typedef struct point{
	int x, y;
}point;

typedef struct polygon{
	point vertex [100];
	int howmany;	
}polygon;

point points[100];
point new_point;

polygon allPolygons [100];

polygon result[100];


float Area(polygon myPolygon);
bool isItInside(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);
bool Snip(polygon myPolygon, int u,int v,int w,int n,int *V);
bool Process(polygon myPolygon, int eachpol);

float Area(polygon myPolygon){

	int n = myPolygon.howmany;
	float a =0;
	int p, q;
	for (p=n-1, q=0; q<n; p=q++){
		a += myPolygon.vertex[p].x * myPolygon.vertex[q].y - myPolygon.vertex[q].x * myPolygon.vertex[p].y ;
	}
	return a*0.5;

}

bool isItInside(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py){
	float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  	float cCROSSap, bCROSScp, aCROSSbp;
  	
  	ax = Cx - Bx;  ay = Cy - By;
  	bx = Ax - Cx;  by = Ay - Cy;
  	cx = Bx - Ax;  cy = By - Ay;
  	apx= Px - Ax;  apy= Py - Ay;
  	bpx= Px - Bx;  bpy= Py - By;
  	cpx= Px - Cx;  cpy= Py - Cy;

  	aCROSSbp = ax*bpy - ay*bpx;
 	cCROSSap = cx*apy - cy*apx;
  	bCROSScp = bx*cpy - by*cpx;

  	return ((aCROSSbp >= 0.0) && (bCROSScp >= 0.0) && (cCROSSap >= 0.0));
}

bool Snip(polygon myPolygon, int u,int v,int w,int n,int *V){
	int p;
	float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

	Ax = myPolygon.vertex[V[u]].x;
  	Ay = myPolygon.vertex[V[u]].y;

  	Bx = myPolygon.vertex[V[v]].x;
  	By = myPolygon.vertex[V[v]].y;

  	Cx = myPolygon.vertex[V[w]].x;
  	Cy = myPolygon.vertex[V[w]].y;

	if ( EP > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) {
		return false;
	}

	for (p=0;p<n;p++){
    	if( (p == u) || (p == v) || (p == w) ){
    		continue;
    	} 
    	Px = myPolygon.vertex[V[p]].x;
    	Py = myPolygon.vertex[V[p]].y;
    	if (isItInside(Ax,Ay,Bx,By,Cx,Cy,Px,Py)){ 
    		return false;
    	}
  	}

  	return true;
}

bool Process(polygon myPolygon, int eachpol){
	
	int n = myPolygon.howmany;		///EDWWWWWWWWW
	int v;
	printf("Process how many%d \n", myPolygon.howmany);
	if (n<3){
		return false;
	}
	int *V; 
	V = (int*) malloc (n*sizeof(int));


	if (0 < Area(myPolygon)){
		for (v = 0; v < n; ++v){
			V[v] = v;
		}
	}else{
		for (v = 0; v < n; ++v){
			V[v] = (n-1)-v;
		}
	}
	int nv = n;	
	int count = 2*nv;
	int m;
	int eachpoint =0;

	for (m = 0, v=nv-1; nv>2;){
		if ( 0>= (count--)){
			return false;
		}

		int u = v;
		if (nv<=u){
			u=0;
		}
		v = u +1;
		if (nv <= v){
			v=0;
		}
		int w = v+1;
		if (nv <=w){
			w=0;
		}

		if (Snip(myPolygon, u, v, w, nv, V)){
			int a, b, c, s, t;
			a = V[u];
			b = V[v];
			c = V[w];
			

			result[eachpol].vertex[eachpoint] = myPolygon.vertex[a]; 
			eachpoint ++;
			result[eachpol].vertex[eachpoint] = myPolygon.vertex[b]; 
			eachpoint ++;
			result[eachpol].vertex[eachpoint] = myPolygon.vertex[c]; 
			eachpoint ++;
			result[eachpol].howmany += 3;

			m++;

			for (s=v, t=v+1; t<nv; s++, t++){
				V[s] = V[t];
				nv--;
			}
			count = 2*nv;
		}
	}

	free (V);

	return true;

}



void initGL(){
  	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void window_reshape(int width, int height){
	glutReshapeWindow( width, height); 
}


void mouse(int button, int state, int x, int y) {
	
	if (drawingstopped){
		glutPostRedisplay();
    	return;
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		new_point.x = x;
		new_point.y = y;
		new_vertex = allPolygons[numofPol].howmany;

		allPolygons[numofPol].vertex[new_vertex] = new_point;
		allPolygons[numofPol].howmany ++;
		printf("Korifes %d\n", allPolygons[numofPol].howmany);
	
		//Detach right click from menu so can stop selecting points
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		
	}
	
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ){ //right click to finish drawing a polygon
		drawingstopped = 1;
		
		glutAttachMenu(GLUT_RIGHT_BUTTON); //attach right click to menu again
		//numofPol++;	
		glutPostRedisplay();
		Process(allPolygons[numofPol], numofPol);
		numofPol++;
    	return;
	}
	glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:     // ESC key
         exit(0);
         break;
      case 'T':
      	letsTriangle = !letsTriangle;
		glutPostRedisplay();
      	break;
   }
}



void drawLines(){
	int j=0;
	int z;
	for (z = 0; z <= numofPol; z++){
		if (allPolygons[z].howmany > 1){
			glColor3f(1.0, 0.0, 0.0);
			glLineWidth(1);
			glBegin(GL_LINES);
			
			yiot = allPolygons[z].howmany;	//for each Polygon, all vertexes till new
			while (k <yiot -1){
				if (yiot>3){
					for (j=0; j<(yiot-3); j++){
						if(LineIntersect(allPolygons[z].vertex[yiot-2].x, allPolygons[z].vertex[yiot-2].y, allPolygons[z].vertex[yiot-1].x, allPolygons[z].vertex[yiot-1].y, allPolygons[z].vertex[j].x, allPolygons[z].vertex[j].y, allPolygons[z].vertex[j+1].x, allPolygons[z].vertex[j+1].y)){
							allPolygons[z].howmany = 0; // delete the vertexes
							//free (allPolygons[z]);
							glutPostRedisplay();
							glEnd();
							return;
						}
					}
				}
				glVertex2f( allPolygons[z].vertex[k].x , h-allPolygons[z].vertex[k].y ); 
        		glVertex2f( allPolygons[z].vertex[k+1].x , h-allPolygons[z].vertex[k+1].y);	
        		k++;
			}
			k=0;
			if(drawingstopped==1 || z<numofPol){
				glVertex2f( allPolygons[z].vertex[yiot-1].x , h-allPolygons[z].vertex[yiot-1].y);
				glVertex2f(allPolygons[z].vertex[0].x, h-allPolygons[z].vertex[0].y);
			
		    }
		    glEnd();
		}
	}
	
}

void drawTringles(){

	int i, j, count;
	for ( i = 0; i <numofPol; ++i){
		
		count = result[i].howmany / 3;
		printf("POSA TRIGWNA %d KORIFES %d\n", count, result[i].howmany );
		
		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(1);
		
		glBegin(GL_LINES);
		
		for (j = 0; j < count; ++j){
			printf("for draw\n");
			
			glVertex2f(result[i].vertex[j*3].x, h-result[i].vertex[j*3].y);
			glVertex2f(result[i].vertex[j*3+1].x, h-result[i].vertex[j*3+1].y);

			glVertex2f(result[i].vertex[j*3+1].x, h-result[i].vertex[j*3+1].y);
			glVertex2f(result[i].vertex[j*3+2].x, h-result[i].vertex[j*3+2].y);

			glVertex2f(result[i].vertex[j*3+2].x, h-result[i].vertex[j*3+2].y);
			glVertex2f(result[i].vertex[j*3].x, h-result[i].vertex[j*3].y);
		}
		glEnd();
	}

}

bool LineIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
	double mua, mub;
	double paron,aritha,arithb;
	paron = (y4-y3) * (x2-x1) - (x4-x3) * (y2-y1);
	aritha = (x4-x3) * (y1-y3) - (y4-y3) * (x1-x3);
	arithb = (x2-x1) * (y1-y3) - (y2-y1) * (x1-x3);
	
	//einai paraliles?
	if (abs(paron) < EP){
		return(0);
	}
	
	//einai telika autotemnomena?
	mua = aritha/paron;
	mub = arithb/paron;
	if (mua <0 || mua>1 || mub<0 || mub>1){
		
		return(0);
		
	}
	return(1);
	
}

void display(void) {

	glutSwapBuffers();
	
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0,w,0,h,-1,1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if (letsTriangle){
    	printf("TRIANGLE DISPLAY \n");
    	drawTringles();
    }
    
    drawLines();
    
	glFlush();
	
}	

void processMenuEvents(int option) {

	switch (option) {
		case POLYGON :
			drawingstopped = 0;	
			glutMouseFunc(mouse);
			break; 
		case CLIPPING :
        	break;	
        case EXTRUDE :
        	break;	
        case EXIT :
        	exit(0);
        	break;	
	}
}

void createGLUTMenus() {

	int ACTION, LINE_COLOR, FILL_COLOR, MAINMENU;
	
	ACTION = glutCreateMenu(processMenuEvents);	
	glutAddMenuEntry("Polygon", POLYGON);		/*** Create the ***/
	glutAddMenuEntry("Exit", EXIT);				/*** subMenus' choises ***/
	
	LINE_COLOR = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("White", 1);    
  	glutAddMenuEntry("Black", 2);
  	glutAddMenuEntry("Red", 3);
  	glutAddMenuEntry("Dark Green", 4);
  	glutAddMenuEntry("Light Green", 5);
  	glutAddMenuEntry("Blue", 6);
  	glutAddMenuEntry("Aqua", 7);
  	glutAddMenuEntry("Pink", 8);
  	glutAddMenuEntry("Soft Pink", 9);
  	glutAddMenuEntry("Purple", 10);
  	glutAddMenuEntry("Brown", 11);
  	glutAddMenuEntry("Yellow", 12);
  	glutAddMenuEntry("Gray", 13);
  	glutAddMenuEntry("Orange", 14);
  	glutAddMenuEntry("Gold", 15);
  	glutAddMenuEntry("Beige", 16);
	
	FILL_COLOR = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("White", 1);    
  	glutAddMenuEntry("Black", 2);
  	glutAddMenuEntry("Red", 3);
  	glutAddMenuEntry("Dark Green", 4);
  	glutAddMenuEntry("Light Green", 5);
  	glutAddMenuEntry("Blue", 6);
  	glutAddMenuEntry("Aqua", 7);
  	glutAddMenuEntry("Pink", 8);
  	glutAddMenuEntry("Soft Pink", 9);
  	glutAddMenuEntry("Purple", 10);
  	glutAddMenuEntry("Brown", 11);
  	glutAddMenuEntry("Yellow", 12);
  	glutAddMenuEntry("Gray", 13);
  	glutAddMenuEntry("Orange", 14);
  	glutAddMenuEntry("Gold", 15);
  	glutAddMenuEntry("Beige", 16);
	
	/**** create the menu and tell glut that "processMenuEvents" will handle the events****/
	MAINMENU = glutCreateMenu(processMenuEvents);
	/**** add entries to our menu ****/
	glutAddSubMenu("Action", ACTION);
	glutAddSubMenu("Line Color", LINE_COLOR);
	glutAddSubMenu("Fill Color", FILL_COLOR);

	glutAttachMenu(GLUT_RIGHT_BUTTON);	/**** attach the menu to the right button ****/
}
	
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	window = glutCreateWindow("My Window");
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 600);
	w = glutGet( GLUT_WINDOW_WIDTH );
    	h = glutGet( GLUT_WINDOW_HEIGHT );
	
	glutDisplayFunc(display);
	glutReshapeFunc(window_reshape);
	createGLUTMenus(); 
	glutKeyboardFunc(keyboard);
		
	initGL();

	glutMainLoop(); 
	return 1;
	
	/*  glNewList(polygons, GL_COMPILE);, 
	allou sxediasoyme, allou kaloume sxediasi
	kai allou ta vazoume sti lista */

}





