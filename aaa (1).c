#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glu.h>    
#include <GL/gl.h>
#include <GL/glut.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <iostream>
//#include <windows.h>

#define POLYGON 	0
#define CLIPPING 	1
#define EXTRUDE 	2
#define EXIT 		3
#define LINECOLOR 	4
#define FILLCOLOR 	5

#define WIDTH 	600
#define HEIGHT 	500
#define EP 		0.000001

#define BOTTOM 	0
#define LEFT 	1
#define TOP 	2
#define RIGHT 	3

//LINE_COLOR & FILL_COLOR menu defines
#define WHITE		1	//glColor3f(1, 1, 1);
#define BLACK 		2  	//glColor3f(0, 0, 0);
#define RED 		3 	//glColor3f(1, 0, 0);
#define DARK_GREEN 	4  	//glColor3f(0, 0.5, 0);
#define LIGHT_GREEN 5 	//glColor3f(0, 1, 0);
#define BLUE 		6   //glColor3f(0, 0, 1);
#define AQUA 		7   //glColor3f(0, 1, 1);
#define PINK 		8   //gcColor3f(1, 0, 1);
#define SOFT_PINK 	9 	//glColor3f(1, 0.8, 0.9);
#define PURPLE 		10  //glColor3f(0.5, 0, 1);
#define BROWN 		11  //glColor3f(0.5, 0.2, 0);
#define YELLOW 		12  //glColor3f(1, 1, 0);
#define GRAY 		13  //glColor3f(0.5, 0.5, 0.5);
#define ORANGE 		14  //glColor3f(1, 0.5, 0);
#define GOLD 		15  //glColor3f(1, 0.85, 0);
#define BEIGE 		16 	//glColor3f(0.95, 0.95, 0.85);


typedef struct point{
	int x, y;
	int realpos; //for INTER
}point;

typedef struct polygon{
	point vertex [100];
	int howmany;
	float color[3];	
}polygon;

typedef struct inter{
	point vertex [100];
	int howmany;
}inter;

bool inside(point mypoint, int side);
polygon clip (polygon myPolygon);
inter countInter(polygon myPolygon, point clipper[], int side);
bool LineIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
float Area(polygon myPolygon);
bool isItInside(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);
bool Snip(polygon myPolygon, int u,int v,int w,int n,int *V);
bool Process(polygon myPolygon, int eachpol);
void initGL();
void drawLines();
void drawTriangles();
void createGLUTMenus();
void lineColorMenuEvents(int option);
void fillColorMenuEvents(int option);

int window, polygons, w, h, yiot;
int drawingstopped = 0;

int clippoint = 0;
int k = 0;
int numofPol = 0;
int numofClipped = 0;
int new_vertex;
float lineColor[] = {0.0, 0.0, 0.0};	/*** Default line color BLACK ***/
float fillColor[] = {0.0, 1.0, 0.0};	/*** Default color for triangles GREEN ***/

bool clippingMode = false;
bool triangleMode = false;
bool clipperDeclared = false;
bool normalMode = true;

point new_point;
polygon allPolygons [80];
polygon result[100];		//for Triangle

point clipper[4];		//for Clipping
polygon clippedPolygons [100];

inter Sort( inter I, int n, int side){	//sort auksousa seira
   int i, j; 
   for (i = 0; i < n-1; i++){
       // Last i elements are already in place    
       for (j = 0; j < n-i-1; j++){
       		if (side%2==0){		// BOTTOM OR TOP
       			if (I.vertex[j].x > I.vertex[j+1].x){	//sugkrinw x 
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
			}	
    	}
	}
	return I;
}

polygon createPol (polygon myPolygon, int pos1, int pos2){	//create new pol apo point pos1 mexri pos2
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
		inter temnomena;
		temnomena = countInter(myPolygon, clipper, j);	//krata ta temnomena
		printf("temnomena: %d \n", temnomena.howmany);
		if (temnomena.howmany>2){				//an einai perissotera apo 2
			temnomena = Sort(temnomena,temnomena.howmany, j);		//taksinomise ta  se auksousa
			for (int z=1; z<temnomena.howmany-1; z+=2){					//gia kathe dyada (1-2,3-4 etc)
				polygon nPolygon;									
				if (temnomena.vertex[z].realpos + 1 == temnomena.vertex[z+1].realpos){	// an einai sinexomena real me to epomeno
					printf("temn1 %d temn2 %d \n", temnomena.vertex[z-1].realpos, temnomena.vertex[z].realpos );
					nPolygon = createPol(myPolygon, temnomena.vertex[z-1].realpos,  temnomena.vertex[z].realpos ); //ftiakse new
					clippedPolygons[numofClipped] = nPolygon;					//kai valto sta clipped
					numofClipped++;
						
					nPolygon = createPol(myPolygon, temnomena.vertex[z+1].realpos,  temnomena.vertex[z+2].realpos );
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

inter countInter(polygon myPolygon, point clipper[], int side){ //return ta temnomena simeia uparxoun
	inter temnomena;
	
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
// Returns value of point of intersectipn of two lines 
point intersectPoint(point a, point b, point c, point d) { 
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

	if (side==BOTTOM){		//bottom line
	//	printf("117 se inside\n");
		if (mypoint.y <= clipper[side].y ){
			return true;
		}
	}
	if(side==LEFT){			//left line
		if (mypoint.x >= clipper[side].x ){
		//	printf("mypoint.x %d > %d clipper[side].x \n",mypoint.x, clipper[side].x);
			return true;
		}
	}
	if(side==TOP){			//top line
		if (mypoint.y >= clipper[side].y ){
			return true;
		}
	}
	if(side==RIGHT){		//right line
		if (mypoint.x <= clipper[side].x ){
			return true;
		}
	}
	return false;
}
// This functions clips all the edges w.r.t one clip edge of clipping area 
polygon clip (polygon myPolygon){
	int j, i, k;
	for (j = 0; j <4; j++){
		int allPoints = myPolygon.howmany;
		int new_points =0;
		polygon newPolygon;
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

float Area(polygon myPolygon){

	int n = myPolygon.howmany;
	float a =0.0f;
	int p, q;
	for (p=n-1, q=0; q<n; p=q++){
		a += myPolygon.vertex[p].x * myPolygon.vertex[q].y - myPolygon.vertex[q].x * myPolygon.vertex[p].y ;
	}
	return (a*0.5);

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
	result[eachpol].howmany = 0;
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
					Process(allPolygons[i], i);	//trigwnopoihsh ksana gia kathe neo		
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
      	normalMode = !normalMode;
      	triangleMode = !triangleMode;
      	printf("TRUE\n");
		glutPostRedisplay();
      	break;
   }
}

void drawLines(){
	
	int j=0;
	int z;
	int k=0;
	for (z = 0; z <=numofPol; z++){
		if (allPolygons[z].howmany > 1){
			printf("allPolygons[z].howmany %d \n",allPolygons[z].howmany);
			glColor3f(allPolygons[z].color[0], allPolygons[z].color[1], allPolygons[z].color[2]);
			glLineWidth(1);
			glBegin(GL_LINES);
			
			yiot = allPolygons[z].howmany;	//for each Polygon, all vertexes till new
			while (k <yiot -1){
				if (yiot>3){
					for (j=0; j<(yiot-3); j++){
						if(LineIntersect(allPolygons[z].vertex[yiot-2].x, allPolygons[z].vertex[yiot-2].y, allPolygons[z].vertex[yiot-1].x, allPolygons[z].vertex[yiot-1].y, allPolygons[z].vertex[j].x, allPolygons[z].vertex[j].y, allPolygons[z].vertex[j+1].x, allPolygons[z].vertex[j+1].y)){
							allPolygons[z].howmany = 0; // delete the vertexes
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
				//for (j = 1; j <= (yiot-3); ++j){
				//	if(LineIntersect(allPolygons[z].vertex[0].x, allPolygons[z].vertex[0].y, allPolygons[z].vertex[yiot-1].x, allPolygons[z].vertex[yiot-1].y, allPolygons[z].vertex[j].x, allPolygons[z].vertex[j].y, allPolygons[z].vertex[j+1].x, allPolygons[z].vertex[j+1].y)){
				//			allPolygons[z].howmany = 0; // delete the vertexes
				//			glutPostRedisplay();
				//			glEnd();
				//			return;
				//		}
			//	}
				glVertex2f( allPolygons[z].vertex[yiot-1].x , h-allPolygons[z].vertex[yiot-1].y);
				glVertex2f(allPolygons[z].vertex[0].x, h-allPolygons[z].vertex[0].y);
			
		    }
		    glEnd();
		}
		lineColor[0] = 0.0;	// Set to default again
		lineColor[1] = 0.0;
		lineColor[2] = 0.0;
	}
	
}

void drawTriangles(){

	int i, j, count;
	for ( i = 0; i <numofPol; ++i){
		
		count = result[i].howmany / 3;
		printf("POSA TRIGWNA %d KORIFES %d\n", count, result[i].howmany );
		
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(1);
		glBegin(GL_LINES);
		
		for (j = 0; j <= count; ++j){
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
	
	//glutSwapBuffers();
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0,w,0,h,-1,1);
    //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();

    if (triangleMode){ 	
    	drawTriangles();
    }
    
    if (normalMode){
    	drawLines();
	} 
	if (clipperDeclared){
		drawLines();
	}
	glFlush();
	glutSwapBuffers();
}	

void processMenuEvents(int option) {

	switch (option) {
		case POLYGON :
			normalMode = true;
			allPolygons[numofPol].color[0] = lineColor[0];
			allPolygons[numofPol].color[1] = lineColor[1];
			allPolygons[numofPol].color[2] = lineColor[2];
			drawingstopped = 0;	
			glutMouseFunc(mouse);
			break; 
		case CLIPPING :
			clippingMode = true;
			glutMouseFunc(mouse);
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
	glutAddMenuEntry("Clipping", CLIPPING);
	glutAddMenuEntry("Exit", EXIT);				/*** subMenus' choises ***/
	
	LINE_COLOR = glutCreateMenu(lineColorMenuEvents);
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
    case BLUE:
      lineColor[0]=0;
      lineColor[1]=0;
      lineColor[2]=1;
      break;
 	case AQUA:
      lineColor[0]=0;
      lineColor[1]=1;
      lineColor[2]=1;
      break;
    case PINK:
      lineColor[0]=1;
      lineColor[1]=0;
      lineColor[2]=1;
      break;
    case SOFT_PINK:
      lineColor[0]=1;
      lineColor[1]=0.8;
      lineColor[2]=0.9;
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
    case BEIGE:
      lineColor[0]=0.95;
      lineColor[1]=0.95;
      lineColor[2]=0.85;
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
 
    case BLUE:
      fillColor[0]=0;
      fillColor[1]=0;
      fillColor[2]=1;
      break;
 	case AQUA:
      fillColor[0]=0;
      fillColor[1]=1;
      fillColor[2]=1;
      break;

    case PINK:
      fillColor[0]=1;
      fillColor[1]=0;
      fillColor[2]=1;
      break;
 
    case SOFT_PINK:
      fillColor[0]=1;
      fillColor[1]=0.8;
      fillColor[2]=0.9;
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
    case BEIGE:
      fillColor[0]=0.95;
      fillColor[1]=0.95;
      fillColor[2]=0.85;
      break;
	}
}	
	
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	window = glutCreateWindow("My Window");
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 600);
	w = glutGet( GLUT_WINDOW_WIDTH );
    h = glutGet( GLUT_WINDOW_HEIGHT );
	initGL();
	glutReshapeFunc(window_reshape);
	glutDisplayFunc(display);
	createGLUTMenus(); 
	glutKeyboardFunc(keyboard);

	glutMainLoop(); 
	return 1;
	/*  glNewList(polygons, GL_COMPILE);, 
	allou sxediasoyme, allou kaloume sxediasi
	kai allou ta vazoume sti lista */

}
