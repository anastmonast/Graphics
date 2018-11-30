#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glu.h>    
#include <GL/gl.h>
#include <GL/glut.h> 
#include <stdbool.h>
#include <math.h>
#include "triangulate.h"

#define POLYGON 	0
#define CLIPPING 	1
#define EXTRUDE 	2
#define EXIT 		3
#define LINECOLOR 	4
#define FILLCOLOR 	5

#define WIDTH 	500
#define HEIGHT 	600
#define EP 		0.000001

#define BOTTOM 	0
#define LEFT 	1
#define TOP 	2
#define RIGHT 	3

/***************** Colors *****************/
#define WHITE		1	//glColor3f(1, 1, 1);
#define BLACK 		2  	//glColor3f(0, 0, 0);
#define RED 		3 	//glColor3f(1, 0, 0);
#define DARK_GREEN 	4  	//glColor3f(0, 0.5, 0);
#define LIGHT_GREEN 5 	//glColor3f(0, 1, 0);
#define DARK_BLUE	6   //glColor3f(0, 0, 1);
#define LIGHT_BLUE 	7   //glColor3f(0, 1, 1);
#define PINK 		8   //gcColor3f(1, 0, 1);
#define KHAKI 		9 	//glColor3f(0.62, 0.62, 0.37);
#define PURPLE 		10  //glColor3f(0.5, 0, 1);
#define BROWN 		11  //glColor3f(0.5, 0.2, 0);
#define YELLOW 		12  //glColor3f(1, 1, 0);
#define GRAY 		13  //glColor3f(0.5, 0.5, 0.5);
#define ORANGE 		14  //glColor3f(1, 0.5, 0);
#define GOLD 		15  //glColor3f(1, 0.85, 0);
#define SILVER 		16 	//glColor3f(0.90, 0.91, 0.98);

typedef struct point{
	int x, y;
	int realpos; //for INTER
}point;

typedef struct polygon{
	point vertex [100];
	int howmany;
	float linecolor[3];
	float fillcolor[3];
	
}polygon;

typedef struct triangles{
	point vertex [3];
	int whichpol;	
}triangles;

/******************  FUNCTIONS DECLARATION ******************/
bool inside(point mypoint, int side);
polygon clip (polygon myPolygon);
polygon countInter(polygon myPolygon, point clipper[], int side);
bool LineIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void initGL();
void drawLines();
void drawTriangles();
void createGLUTMenus();
void lineColorMenuEvents(int option);
void fillColorMenuEvents(int option);

/****************** GLOBALS ******************/
int window, polygons, w, h, yiot;
int drawingstopped = 0;
int clippoint = 0;
int numofPol = 0;
int numofClipped = 0;
int new_vertex;
int depth =0;
float lineColor[] = {0.0, 0.0, 0.0};	/*** Default line color BLACK ***/
float fillColor[] = {1.0, 1.0, 1.0};	/*** Default fill color WHITE ***/
int phi = 20;
int theta = 20;
int alltriangles =0;

bool clippingMode = false;
bool triangleMode = false;
bool clipperDeclared = false;
bool normalMode = true;
bool extrudeMode = false;

point new_point;
polygon allPolygons [100];
triangles result[100];			/*** for TRIANGLE ***/
point clipper[4];				/*** Clipping area ***/
polygon clippedPolygons [120];

/************************* CLIPPING *************************/

polygon Sort( polygon I, int n, int side){	//sort auksousa seira
   int i, j; 
   for (i = 0; i < n-1; i++){
       // Last i elements are already in place    
       for (j = 0; j < n-i-1; j++){
       		if (side==0){		// BOTTOM OR TOP
       			if (I.vertex[j].x < I.vertex[j+1].x){	//sugkrinw x 
       				point tmp;
       				int tmpreal;
       				tmp = I.vertex[j];
       				tmpreal = I.vertex[j].realpos;
       				I.vertex[j] = I.vertex[j+1];	//ekana swsta swap?
       				I.vertex[j].realpos = I.vertex[j+1].realpos;
       				I.vertex[j+1] = tmp;
       				I.vertex[j+1].realpos = tmpreal;
           		}
			}else if(side==1){
				if (I.vertex[j].y < I.vertex[j+1].y){	//sygkrinw y
           			point tmp;
       				int tmpreal;
       				tmp = I.vertex[j];
       				tmpreal = I.vertex[j].realpos;
       				I.vertex[j] = I.vertex[j+1];	//ekana swsta swap?
       				I.vertex[j].realpos = I.vertex[j+1].realpos;
       				I.vertex[j+1] = tmp;
       				I.vertex[j+1].realpos = tmpreal;
           		}
			}else if(side==2){
				if (I.vertex[j].x > I.vertex[j+1].x){	//sygkrinw y
           			point tmp;
       				int tmpreal;
       				tmp = I.vertex[j];
       				tmpreal = I.vertex[j].realpos;
       				I.vertex[j] = I.vertex[j+1];	//ekana swsta swap?
       				I.vertex[j].realpos = I.vertex[j+1].realpos;
       				I.vertex[j+1] = tmp;
       				I.vertex[j+1].realpos = tmpreal;
           		}
			}else{
				if (I.vertex[j].y > I.vertex[j+1].y){	//sygkrinw y
           			point tmp;
       				int tmpreal;
       				tmp = I.vertex[j];
       				tmpreal = I.vertex[j].realpos;
       				I.vertex[j] = I.vertex[j+1];	//ekana swsta swap?
       				I.vertex[j].realpos = I.vertex[j+1].realpos;
       				I.vertex[j+1] = tmp;
       				I.vertex[j+1].realpos = tmpreal;
           		}
			}	
    	}
	}
	return I;
}

polygon createPol (polygon myPolygon, int pos1, int pos2){	//create new polygon from point pos1 to pos2
	polygon newPolygon;
	newPolygon.howmany = 0;
	int k;
	printf("pos1 %d pos2 %d \n", pos1, pos2);
	for (int i=0; i<=abs(pos1-pos2); i++){
		k = (pos1+i)%myPolygon.howmany;
		newPolygon.vertex[newPolygon.howmany] = myPolygon.vertex[k];
		newPolygon.howmany++;
		if (k==pos2){
			printf("BREAK\n");
			break;
		}
	}
	printf("HOW MANY CREATE: %d \n ", newPolygon.howmany );
	return newPolygon;
}

void checkClip(polygon myPolygon, point clipper[]){
	int j;	
	bool didyoucreate = false;
	for (j=0; j<4; j++){				// gia kathe pleura tou clipper
		polygon temnomena;
		temnomena.howmany = 0;
		temnomena = countInter(myPolygon, clipper, j);	//krata ta temnomena
		printf("temnomena: %d \n", temnomena.howmany);
		if (temnomena.howmany>2){				//an einai perissotera apo 2
			temnomena = Sort(temnomena,temnomena.howmany, j);		//taksinomise ta  se auksousa
			for (int z=1; z<temnomena.howmany-1; z+=2){					//gia kathe dyada (1-2,3-4 etc)
				polygon nPolygon;							
				if (temnomena.vertex[z].realpos + 1 == temnomena.vertex[z+1].realpos){	// an einai sinexomena real me to epomeno
					printf("temn1 %d temn2 %d \n", temnomena.vertex[z-1].realpos, temnomena.vertex[z].realpos );
					if (z==1){
						nPolygon = createPol(myPolygon, temnomena.vertex[z-1].realpos,  temnomena.vertex[z].realpos ); //ftiakse new
						nPolygon.linecolor[0] = myPolygon.linecolor[0];
						nPolygon.linecolor[1] = myPolygon.linecolor[1];
						nPolygon.linecolor[2] = myPolygon.linecolor[2];
						nPolygon.fillcolor[0] = myPolygon.fillcolor[0];
						nPolygon.fillcolor[1] = myPolygon.fillcolor[1];
						nPolygon.fillcolor[2] = myPolygon.fillcolor[2];	
						clippedPolygons[numofClipped] = nPolygon;					//kai valto sta clipped
						numofClipped++;
					}		
					nPolygon = createPol(myPolygon, temnomena.vertex[z+1].realpos,  temnomena.vertex[z+2].realpos );
					nPolygon.linecolor[0] = myPolygon.linecolor[0];
					nPolygon.linecolor[1] = myPolygon.linecolor[1];
					nPolygon.linecolor[2] = myPolygon.linecolor[2];
					nPolygon.fillcolor[0] = myPolygon.fillcolor[0];
					nPolygon.fillcolor[1] = myPolygon.fillcolor[1];
					nPolygon.fillcolor[2] = myPolygon.fillcolor[2];	
					clippedPolygons[numofClipped] = nPolygon;
					numofClipped++;				
					didyoucreate = true;
				}
			}
			if (didyoucreate){
				printf("did you create\n");
				return;
			}	
		}
	}
	
	printf("NOT\n");
	clippedPolygons[numofClipped] = myPolygon;
	numofClipped++;
	return;
}

polygon countInter(polygon myPolygon, point clipper[], int side){ //return ta temnomena simeia uparxoun
	polygon temnomena;
	temnomena.howmany = 0;
	
	if (side==0 || side==2){
		for (int i=0; i<myPolygon.howmany; i++){
			if (myPolygon.vertex[i].y == clipper[side].y){
				temnomena.vertex[temnomena.howmany] = myPolygon.vertex[i];
				temnomena.vertex[temnomena.howmany].realpos = i;
				temnomena.howmany++;	
			}
		}
	}else{
		for (int i=0; i<myPolygon.howmany; i++){
			if (myPolygon.vertex[i].x == clipper[side].x){
				temnomena.vertex[temnomena.howmany] = myPolygon.vertex[i];
				temnomena.vertex[temnomena.howmany].realpos = i;
				temnomena.howmany++;
			}
		}	
	}
	return temnomena;
}

point intersectPoint(point a, point b, point c, point d) { 	// Returns value of point of intersectipn of two lines 
	int x1 = a.x;
	int y1 = a.y;
	int x2 = b.x;
	int y2 = b.y;
	int x3 = c.x;
	int y3 = c.y;

	int x4 = d.x;
	int y4 = d.y;

	int arthmX = (x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4);
    int arthmY = (x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4);
    int par = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
    point I;
    I.x = arthmX/par;
    I.y = arthmY/par;
    return I; 
}

bool inside(point mypoint, int side) {

	if (side==BOTTOM){					// bottom line
		if (mypoint.y <= clipper[side].y ){
			return true;
		}
	}
	if(side==LEFT){						// left line
		if (mypoint.x >= clipper[side].x ){
			return true;
		}
	}
	if(side==TOP){						// top line
		if (mypoint.y >= clipper[side].y ){
			return true;
		}
	}
	if(side==RIGHT){					// right line
		if (mypoint.x <= clipper[side].x ){
			return true;
		}
	}
	return false;
}

polygon clip (polygon myPolygon){
	int j, i, k;
	for (j = 0; j <4; j++){
		int allPoints = myPolygon.howmany;
		int new_points =0;
		polygon newPolygon;
		newPolygon.linecolor[0] = myPolygon.linecolor[0];
		newPolygon.linecolor[1] = myPolygon.linecolor[1];
		newPolygon.linecolor[2] = myPolygon.linecolor[2];
		newPolygon.fillcolor[0] = myPolygon.fillcolor[0];
		newPolygon.fillcolor[1] = myPolygon.fillcolor[1];
		newPolygon.fillcolor[2] = myPolygon.fillcolor[2];
		newPolygon.howmany = 0;
	
		for (i = 0; i < allPoints; i++){
			k = (i+1)%allPoints;
			point p1 = myPolygon.vertex[i];
			point p2 = myPolygon.vertex[k];

			if ( inside(p1, j) && inside(p2, j) ){	// kai ta 2 eswterika
				/* keep the second */
				newPolygon.vertex[new_points] = p2;
				newPolygon.howmany ++;
				new_points++;
			}else if ( inside(p1, j) && !inside(p2, j) ){ // to prwto eswteriko 2o ekswteriko
				/* keep the I */
				newPolygon.vertex[new_points] = intersectPoint (p1, p2, clipper[j], clipper[(j+1)%4]);
				newPolygon.howmany ++;
				new_points++;
				// 	//add one I
			}else if ( !inside(p1, j) && !inside(p2, j)){ // 2 ekswterika
				/* none */
			}else if ( !inside(p1, j) && inside(p2, j)){
				/* keep the I and the second */
				newPolygon.vertex[new_points] = intersectPoint (p1, p2, clipper[j], clipper[(j+1)%4]);
				newPolygon.howmany ++;
				new_points++;

				newPolygon.vertex[new_points] = p2;
				newPolygon.howmany ++;
				new_points++;
			}else{
				/* do nothing */
			}	
		}
			
		myPolygon = newPolygon;
	}

	if (myPolygon.howmany <3){
		myPolygon.howmany ==0;
	}
	return myPolygon;
}

/************************* TRIANGULATION *************************/

void triangulation(){
	int i, j, k, triangles;
	alltriangles =0;
	for (int i=0; i<numofPol; i++){
		Vector2dVector a, myresult;
		for (j=0; j<allPolygons[i].howmany; j++){
			a.push_back(Vector2d(allPolygons[i].vertex[j].x, allPolygons[i].vertex[j].y));
		}
		Triangulate::Process(a, myresult);
		triangles = myresult.size() / 3;
		
		for(k=0; k<triangles; k++){
			
			result[alltriangles].whichpol = i;
			
			result[alltriangles].vertex[0].x = myresult[k*3].GetX();
			result[alltriangles].vertex[0].y = myresult[k*3].GetY();

			result[alltriangles].vertex[1].x = myresult[k*3 + 1].GetX();
			result[alltriangles].vertex[1].y = myresult[k*3 + 1].GetY();
			
			result[alltriangles].vertex[2].x = myresult[k*3 + 2].GetX();
			result[alltriangles].vertex[2].y = myresult[k*3 + 2].GetY();
	
			alltriangles++;
		}	
	}
	
}
/************************* WINDOW-MOUSE-KEYBOARD HANDLE *************************/
void initGL(){
  	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  	glClear( GL_COLOR_BUFFER_BIT );
}

void window_reshape(int width, int height){
	glutReshapeWindow( width, height); 
}

void mouse(int button, int state, int x, int y) {
	if ( (drawingstopped && !clippingMode) || clipperDeclared ){ 
		glutPostRedisplay();
    	return;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		if (clippingMode){
			if (clippoint<2){
				clipper[clippoint].x = x;		//Save top left corner first[0] and then right bottom[2]
				clipper[clippoint].y = y;
				printf("%d point saved\n", clippoint);
				clippoint += 2;	
			}else{
				clipper[clippoint].x = x;		//Save top left corner first[0] and then right bottom[2]
				clipper[clippoint].y = y;
				printf("%d point saved\n", clippoint);
				clipper[1].x = clipper[2].x;
				clipper[1].y = clipper[0].y;
				clipper[3].x = clipper[0].x;
				clipper[3].y = clipper[2].y;
				int i;
				memset(result, 0, 100);	//delete ta trigwna

				for (i=0; i<numofPol; i++){
					allPolygons[i] = clip(allPolygons[i]);	//antikathista ta polugwna me ta kommena
					checkClip (allPolygons[i],clipper);
				}
				for (i=0; i<numofClipped; i++){
					allPolygons[i] = clippedPolygons[i];	//prosthetei ta extra	
				}
				numofPol = numofClipped;
				normalMode = false;
				clipperDeclared = true;
				glutPostRedisplay();
    			return;
			}
			
		}else{
			new_point.x = x;
			new_point.y = y;
			new_vertex = allPolygons[numofPol].howmany;

			allPolygons[numofPol].vertex[new_vertex] = new_point;
			allPolygons[numofPol].howmany ++;
			printf("Korifes %d\n", allPolygons[numofPol].howmany);
	
			//Detach right click from menu so can stop selecting points
			glutDetachMenu(GLUT_RIGHT_BUTTON);
		}	
	}
	
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ){ //right click to finish drawing a polygon
		drawingstopped = 1;
		glutAttachMenu(GLUT_RIGHT_BUTTON); //attach right click to menu again	
		numofPol++;
		triangulation();
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
      case 'T':
      	normalMode = !normalMode;
      	triangleMode = !triangleMode;
      	printf("TRUE\n");
		glutPostRedisplay();
      	break;
   }
}

/**************************** DRAWING ****************************/
void draw3d(){
	int i, j, k;
	int z = depth;
	for (i=0; i<numofPol; i++){
		
		for (j=0; j<allPolygons[i].howmany; j++){
			k = (j+1)%allPolygons[i].howmany;
			glBegin(GL_LINES);
			
			glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, 0); 	//start point of edge
        	glVertex3f( allPolygons[i].vertex[k].x , h-allPolygons[i].vertex[k].y, 0);	//end point in 0 depth
        	
        	glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, z);  //start point of edge
        	glVertex3f( allPolygons[i].vertex[k].x , h-allPolygons[i].vertex[k].y, z);	//end point in z depth
        	
        	glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, 0); //connect 0 to z depth
        	glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, z);	
			
			glEnd();
			
			glBegin(GL_QUADS);
			
			glVertex3f( allPolygons[i].vertex[k].x , h-allPolygons[i].vertex[k].y, 0);	//fill the sides
        	glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, 0); 
        	glVertex3f( allPolygons[i].vertex[k].x , h-allPolygons[i].vertex[k].y, z);	
        	glVertex3f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y, z); 
			glEnd();
		}
	}
	
}

void drawLines(){
	
	int j=0;	int k=0;	int z;

	for (z = 0; z <=numofPol; z++){
		if (allPolygons[z].howmany > 1){
			printf("allPolygons[z].howmany %d \n",allPolygons[z].howmany);
			glColor3f(allPolygons[z].linecolor[0], allPolygons[z].linecolor[1], allPolygons[z].linecolor[2]);
			glLineWidth(2);
			glBegin(GL_LINES);
			
			yiot = allPolygons[z].howmany;				//check INTERSECT for all edges till new
			while (k <yiot -1){
				if (yiot>3){
					for (j=0; j<(yiot-3); j++){
						if(LineIntersect(allPolygons[z].vertex[yiot-2].x, allPolygons[z].vertex[yiot-2].y, allPolygons[z].vertex[yiot-1].x, allPolygons[z].vertex[yiot-1].y, allPolygons[z].vertex[j].x, allPolygons[z].vertex[j].y, allPolygons[z].vertex[j+1].x, allPolygons[z].vertex[j+1].y)){
							allPolygons[z].howmany = 0; // delete the vertexes
							numofPol--;
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
			if(drawingstopped==1 || z<numofPol){		// check and draw the last line
				for (j = 1; j <= (yiot-3); ++j){
					if(LineIntersect(allPolygons[z].vertex[0].x, allPolygons[z].vertex[0].y, allPolygons[z].vertex[yiot-1].x, allPolygons[z].vertex[yiot-1].y, allPolygons[z].vertex[j].x, allPolygons[z].vertex[j].y, allPolygons[z].vertex[j+1].x, allPolygons[z].vertex[j+1].y)){
							allPolygons[z].howmany = 0; 
							numofPol--;
							glutPostRedisplay();
							glEnd();
							return;
						}
				}
				glVertex2f( allPolygons[z].vertex[yiot-1].x , h-allPolygons[z].vertex[yiot-1].y);
				glVertex2f(allPolygons[z].vertex[0].x, h-allPolygons[z].vertex[0].y);	
		    }
		    glEnd();
		}
	}	
}

void drawClipped(){
	int i, j, k;
	for (i=0; i<numofPol; i++){
		glColor3f(allPolygons[i].linecolor[0], allPolygons[i].linecolor[1], allPolygons[i].linecolor[2]);
		glLineWidth(2);
		glBegin(GL_LINES);
		for (j=0; j<allPolygons[i].howmany; j++){
			k = (j+1)%allPolygons[i].howmany;
			glVertex2f( allPolygons[i].vertex[j].x , h-allPolygons[i].vertex[j].y ); 
        	glVertex2f( allPolygons[i].vertex[k].x , h-allPolygons[i].vertex[k].y);	
		}
		glEnd();
	}
}

void drawTriangles(){

	int i;
	for ( i = 0; i <alltriangles; ++i){	
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(1);
		
		glBegin(GL_LINES);
		glVertex2f(result[i].vertex[0].x, h-result[i].vertex[0].y);
		glVertex2f(result[i].vertex[1].x, h-result[i].vertex[1].y);
		
		glVertex2f(result[i].vertex[1].x, h-result[i].vertex[1].y);
		glVertex2f(result[i].vertex[2].x, h-result[i].vertex[2].y);
			
		glVertex2f(result[i].vertex[2].x, h-result[i].vertex[2].y);
		glVertex2f(result[i].vertex[0].x, h-result[i].vertex[0].y);	
		glEnd();	
	}
}

void fillTriangles(){

	int i, j, k;
	for ( i = 0; i <numofPol; ++i){	
		for (j = 0; j <alltriangles; ++j){
			k = result[j].whichpol;
			glBegin(GL_TRIANGLES);
			glColor3f(allPolygons[k].fillcolor[0], allPolygons[k].fillcolor[1], allPolygons[k].fillcolor[2]);
			glVertex2f(result[j].vertex[0].x, h-result[j].vertex[0].y);
			glVertex2f(result[j].vertex[1].x, h-result[j].vertex[1].y);
			glVertex2f(result[j].vertex[2].x, h-result[j].vertex[2].y);
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
	
   	glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0, w, 0, h,0,400);
   
    if (triangleMode){ 	
    	drawTriangles();
    }
    
    if (normalMode){
    	drawLines();
    	fillTriangles();
	} 
	if (clipperDeclared){
		drawClipped();
		triangulation();
		fillTriangles();
		normalMode = true;	//done with clipping back to normalMode
	}
	if (extrudeMode){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, w/h, 0.1f, 100.0f);
		
		glOrtho(0 ,w ,h ,0 , 0, 500);
		
  		glMatrixMode(GL_MODELVIEW);
  		glLoadIdentity();

 		float eyeX, eyeY, eyeZ;
 		float radius = 150;
 		eyeX = radius*sin(theta);
 		eyeY = radius*sin(phi);
 		eyeZ = radius*cos(theta);
 		gluLookAt(eyeX, eyeY, eyeZ, 0, 0, -100, 0, 1.0f, 0);
  	
  		glLoadIdentity();
 
  		/*  Set View Angle */
  		//glRotated(20,1,0,0);
  		//glRotated(-20,0,1,0);
		draw3d();
	}
	glFlush();
	glutSwapBuffers();
}	

/**************************** MENU HANDLE ****************************/
void actionMenuEvents(int option) {
	switch (option) {
		case POLYGON :
			normalMode = true;
			allPolygons[numofPol].linecolor[0] = lineColor[0];
			allPolygons[numofPol].linecolor[1] = lineColor[1];
			allPolygons[numofPol].linecolor[2] = lineColor[2];
			allPolygons[numofPol].fillcolor[0] = fillColor[0];
			allPolygons[numofPol].fillcolor[1] = fillColor[1];
			allPolygons[numofPol].fillcolor[2] = fillColor[2];
			lineColor[0] = 0.0;		lineColor[1] = 0.0;		lineColor[2] = 0.0; /** Set default color BLACK **/
			fillColor[0] = 1.0;		fillColor[1] = 1.0;		fillColor[2] = 1.0; /** Set default color WHITE **/
			drawingstopped = 0;	
			glutMouseFunc(mouse);
			break; 
		case CLIPPING :
			clippingMode = true;
			glutMouseFunc(mouse);
        	break;	
        case EXTRUDE :
        	printf("Extrude mode ON. Give me the depth from 0 to 30: ");
        	scanf("%d \n", &depth);
        	extrudeMode = true;
        	glutPostRedisplay();
        	break;	
        case EXIT :
        	exit(0);
        	break;	
	}
}

void createGLUTMenus() {

	int ACTION, LINE_COLOR, FILL_COLOR, MAINMENU;
	
	ACTION = glutCreateMenu(actionMenuEvents);	
	glutAddMenuEntry("Polygon", POLYGON);		/*** Create the  subMenus' choises ***/
	glutAddMenuEntry("Clipping", CLIPPING);		
	glutAddMenuEntry("Extrude", EXTRUDE);	
	glutAddMenuEntry("Exit", EXIT);				
	/**** create the menu and tell glut that "lineColorMenuEvents" will handle the events ****/
	LINE_COLOR = glutCreateMenu(lineColorMenuEvents);
	glutAddMenuEntry("White", 1);    
  	glutAddMenuEntry("Black", 2);
  	glutAddMenuEntry("Red", 3);
  	glutAddMenuEntry("Dark Green", 4);
  	glutAddMenuEntry("Light Green", 5);
  	glutAddMenuEntry("Dark Blue", 6);
  	glutAddMenuEntry("Light Blue", 7);
  	glutAddMenuEntry("Pink", 8);
  	glutAddMenuEntry("Khaki", 9);
  	glutAddMenuEntry("Purple", 10);
  	glutAddMenuEntry("Brown", 11);
  	glutAddMenuEntry("Yellow", 12);
  	glutAddMenuEntry("Gray", 13);
  	glutAddMenuEntry("Orange", 14);
  	glutAddMenuEntry("Gold", 15);
  	glutAddMenuEntry("Silver", 16);
	
	FILL_COLOR = glutCreateMenu(fillColorMenuEvents);
	glutAddMenuEntry("White", 1);    
  	glutAddMenuEntry("Black", 2);
  	glutAddMenuEntry("Red", 3);
  	glutAddMenuEntry("Dark Green", 4);
  	glutAddMenuEntry("Light Green", 5);
  	glutAddMenuEntry("Dark Blue", 6);
  	glutAddMenuEntry("Light Blue", 7);
  	glutAddMenuEntry("Pink", 8);
  	glutAddMenuEntry("Khaki", 9);
  	glutAddMenuEntry("Purple", 10);
  	glutAddMenuEntry("Brown", 11);
  	glutAddMenuEntry("Yellow", 12);
  	glutAddMenuEntry("Gray", 13);
  	glutAddMenuEntry("Orange", 14);
  	glutAddMenuEntry("Gold", 15);
  	glutAddMenuEntry("Silver", 16);
	
	/**** create the menu and tell glut that "processMenuEvents" will handle the events****/
	MAINMENU = glutCreateMenu(actionMenuEvents);
	/**** add entries to our menu ****/
	glutAddSubMenu("Action", ACTION);
	glutAddSubMenu("Line Color", LINE_COLOR);
	glutAddSubMenu("Fill Color", FILL_COLOR);

	glutAttachMenu(GLUT_RIGHT_BUTTON);	/**** attach the menu to the right button ****/
}
	
void lineColorMenuEvents(int option){
	
	switch(option){
    case WHITE:
      lineColor[0]=1;
      lineColor[1]=1;
      lineColor[2]=1;
      break;
    case BLACK:
      lineColor[0]=0;
      lineColor[1]=0;
      lineColor[2]=0;
      break;
    case RED:
      lineColor[0]=1;
      lineColor[1]=0;
      lineColor[2]=0;
      break;
    case DARK_GREEN:
      lineColor[0]=0;
      lineColor[1]=0.5;
      lineColor[2]=0;
      break;
    case LIGHT_GREEN:
      lineColor[0]=0;
      lineColor[1]=1;
      lineColor[2]=0;
      break;
    case DARK_BLUE:
      lineColor[0]=0;
      lineColor[1]=0;
      lineColor[2]=1;
      break;
 	case LIGHT_BLUE:
      lineColor[0]=0;
      lineColor[1]=1;
      lineColor[2]=1;
      break;
    case PINK:
      lineColor[0]=1;
      lineColor[1]=0;
      lineColor[2]=1;
      break;
    case KHAKI:
      lineColor[0]=0.62;
      lineColor[1]=0.62;
      lineColor[2]=0.37;
      break;
    case PURPLE:
      lineColor[0]=0.5;
      lineColor[1]=0;
      lineColor[2]=1;
      break;
    case BROWN:
      lineColor[0]=0.5;
      lineColor[1]=0.2;
      lineColor[2]=0;
      break;
    case YELLOW:
      lineColor[0]=1;
      lineColor[1]=1;
      lineColor[2]=0;
      break;
    case GRAY:
      lineColor[0]=0.5;
      lineColor[1]=0.5;
      lineColor[2]=0.5;
      break;
    case ORANGE:
      lineColor[0]=1;
      lineColor[1]=0.5;
      lineColor[2]=0;
      break;
 
    case GOLD:
      lineColor[0]=1;
      lineColor[1]=0.85;
      lineColor[2]=0;
      break;
    case SILVER:
      lineColor[0]=0.90;
      lineColor[1]=0.91;
      lineColor[2]=0.98;
      break;
	}
	
	
	
}	

void fillColorMenuEvents(int option){
	switch(option){
    case WHITE:
      fillColor[0]=1;
      fillColor[1]=1;
      fillColor[2]=1;
      break;
    case BLACK:
      fillColor[0]=0;
      fillColor[1]=0;
      fillColor[2]=0;
      break;
    case RED:
      fillColor[0]=1;
      fillColor[1]=0;
      fillColor[2]=0;
      break;
    case DARK_GREEN:
      fillColor[0]=0;
      fillColor[1]=0.5;
      fillColor[2]=0;
      break;
    case LIGHT_GREEN:
      fillColor[0]=0;
      fillColor[1]=1;
      fillColor[2]=0;
      break;
    case DARK_BLUE:
      fillColor[0]=0;
      fillColor[1]=0;
      fillColor[2]=1;
      break;
 	case LIGHT_BLUE:
      fillColor[0]=0;
      fillColor[1]=1;
      fillColor[2]=1;
      break;
    case PINK:
      fillColor[0]=1;
      fillColor[1]=0;
      fillColor[2]=1;
      break;
     case KHAKI:
      fillColor[0]=0.62;
      fillColor[1]=0.62;
      fillColor[2]=0.37;
      break;
    case PURPLE:
      fillColor[0]=0.5;
      fillColor[1]=0;
      fillColor[2]=1;
      break;
    case BROWN:
      fillColor[0]=0.5;
      fillColor[1]=0.2;
      fillColor[2]=0;
      break;
    case YELLOW:
      fillColor[0]=1;
      fillColor[1]=1;
      fillColor[2]=0;
      break;
    case GRAY:
      fillColor[0]=0.5;
      fillColor[1]=0.5;
      fillColor[2]=0.5;
      break;
    case ORANGE:
      fillColor[0]=1;
      fillColor[1]=0.5;
      fillColor[2]=0;
      break;
    case GOLD:
      fillColor[0]=1;
      fillColor[1]=0.85;
      fillColor[2]=0;
      break;
    case SILVER:
      fillColor[0]=0.90;
      fillColor[1]=0.91;
      fillColor[2]=0.98;
      break;
	}
}	
	
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	window = glutCreateWindow("My Window");
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	w = glutGet( GLUT_WINDOW_WIDTH );
    h = glutGet( GLUT_WINDOW_HEIGHT );
	initGL();
	glutReshapeFunc(window_reshape);
	glutDisplayFunc(display);
	createGLUTMenus(); 
	glutKeyboardFunc(keyboard);

	glutMainLoop(); 
	return 1;
}
