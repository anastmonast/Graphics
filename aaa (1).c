#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glu.h>    
#include <GL/gl.h>
#include <GL/glut.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

#define POLYGON 0
#define EXIT 1
#define LINECOLOR 2
#define FILLCOLOR 3

#define WIDTH 600
#define HEIGHT 500
#define one 1
#define EP 0.00001

bool LineIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
 
int window, polygons;
int drawingstopped = 0;
int k = 0;
int w,h, yiot;
int numofPol = 0;
int new_vertex;


int ACTION, MAINMENU, LINE_COLOR, FILL_COLOR;

void render_scene (void);

void drawLines();

typedef struct point{
	int x, y;
}point;

typedef struct polygon{
	point vertex [50];
	int howmany;	
}polygon;

int i = 0;
point points[100];
point new_point;

polygon allPolygons [100];

void initGL(){
  	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void window_reshape(int width, int height){
	glutReshapeWindow( width, height); 
}
/*
void addToList (polygon new_pol){
	allPolygons[numofPol] = new_pol;
	numofPol++;
	allPolygons = (polygon*) realloc (allPolygons, sizeof(polygon));
}*/


void mouse(int button, int state, int x, int y) {
	
	if (drawingstopped){
		glutPostRedisplay();
    	return;
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		printf("Point: %d saved\n", i);
		new_point.x = x;
		new_point.y = y;
		new_vertex = allPolygons[numofPol].howmany;

		allPolygons[numofPol].vertex[new_vertex] = new_point;
		allPolygons[numofPol].howmany ++;
	
		i++;	//vertex(how many points)	
	}
	
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ){ //right click to finish drawing a polygon
		printf("Start drawing");
		drawingstopped = 1;
		glutAttachMenu(GLUT_RIGHT_BUTTON); //attach right click to menu again
		numofPol++;	
		glutPostRedisplay();
    	return;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:     // ESC key
         exit(0);
         break;
   }
}



void drawLines(){
	int j=0;
	int z;
	for (z = 0; z < numofPol; z++){
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
			if(drawingstopped==1){
				glVertex2f( allPolygons[z].vertex[yiot-1].x , h-allPolygons[z].vertex[yiot-1].y);
				glVertex2f(allPolygons[z].vertex[0].x, h-allPolygons[z].vertex[0].y);
			
		    }
		    glEnd();
		}
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
		return(FALSE);
	}
	
	//einai telika autotemnomena?
	mua = aritha/paron;
	mub = arithb/paron;
	if (mua <0 || mua>1 || mub<0 || mub>1){
		
		return(FALSE);
		
	}
	return(TRUE);
	
}

void display(void) {
//	float ratio = 1.0* w / h;
	glutSwapBuffers();
	
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0,w,0,h,-1,1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    drawLines(); //AUTI PREPEI NA KALEITE SE DISPLAY GIA NA GINEI
    
	glFlush();
	
}	

void processMenuEvents(int option) {

	switch (option) {
		case POLYGON :
			drawingstopped = 0;
			glutDetachMenu(GLUT_RIGHT_BUTTON); 	//Detach right click from menu so can stop selecting points (mouse)
			glutMouseFunc(mouse);
			printf("POLYGON MODE/n");
			break; 
        case EXIT :
        	exit(0);
        	break;	
        case LINECOLOR :
        	break;
        case FILLCOLOR :
        	break;
	}
}

void createGLUTMenus() {

	int ACTION;
	int MAINMENU;
	ACTION = glutCreateMenu(processMenuEvents);
	
	glutAddMenuEntry("Polygon", POLYGON);
	glutAddMenuEntry("Exit", EXIT);
	LINE_COLOR = glutCreateMenu(processMenuEvents);
	FILL_COLOR = glutCreateMenu(processMenuEvents);
	// create the menu and tell glut that "processMenuEvents"
	//  will handle the events
	MAINMENU = glutCreateMenu(processMenuEvents);
	//add entries to our menu
	glutAddSubMenu("Action", ACTION);
	glutAddMenuEntry("Line Color", LINE_COLOR);
	glutAddMenuEntry("Fill Color", FILL_COLOR);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
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
		
	initGL();

	glutMainLoop(); 
	return 1;
	
	/* Prepei na tsekaroume ti paizei me glNewList(polygons, GL_COMPILE);, 
	allou sxediasoyme, allou kaloume sxediasi
	kai allou ta vazoume sti lista ELA LIGO GIA PAME*/

}



