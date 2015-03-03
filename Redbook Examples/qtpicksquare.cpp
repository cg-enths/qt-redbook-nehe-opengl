/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */



//          picksquare.cpp
//   Use of multiple names and picking are demonstrated.  
//   A 3x3 grid of squares is drawn.  When the left mouse 
//   button is pressed, all squares under the cursor position 
//   have their color changed.
//   Author  Travis Astle
//   Oct 19, 2001

#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>

int board[3][3];   /*  amount of color for each square  */

GLuint ii =0, jj=0, names, *ptr;

// Picksquare class declaration

class Picksquare : public QGLWidget
{
public:
    Picksquare( QWidget *parent=0, const char *name=0 ) :
        QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void drawSquares(GLenum mode);
    void keyPressEvent( QKeyEvent *e);
    void processHits (GLint hits, GLuint buffer[]);
    void mousePressEvent(QMouseEvent *e);

};

// Initialize function adapted from existing init function code

void Picksquare::initializeGL(void)
{
   int i, j;
   for (i = 0; i < 3; i++) 
      for (j = 0; j < 3; j ++)
         board[i][j] = 0;
   glClearColor (0.0, 0.0, 0.0, 0.0);
}

// Paint function to replace the display function

void Picksquare::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   drawSquares (GL_RENDER);
   glFlush();
}

/*  The nine squares are drawn.  In selection mode, each 
 *  square is given two names:  one for the row and the 
 *  other for the column on the grid.  The color of each 
 *  square is determined by its position on the grid, and 
 *  the value in the board[][] array.
 */

void Picksquare::drawSquares(GLenum mode)
{
   GLuint i, j;
   for (i = 0; i < 3; i++) {
      if (mode == GL_SELECT)
         glLoadName (i);
      for (j = 0; j < 3; j ++) {
         if (mode == GL_SELECT)
            glPushName (j);
         glColor3f ((GLfloat) i/3.0, (GLfloat) j/3.0, 
                    (GLfloat) board[i][j]/3.0);
         glRecti (i, j, i+1, j+1);
         if (mode == GL_SELECT)
            glPopName ();
      }
   }
}

// Resize function adapted from reshape function code

void Picksquare::resizeGL(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0.0, 3.0, 0.0, 3.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

//  processHits changes color of selected boxes
 
void Picksquare::processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint ii, jj, names, *ptr;

   printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;
   for (i = 0; i < hits; i++) { /*  for each hit  */
      names = *ptr;
      printf (" number of names for this hit = %d\n", names); ptr++;
      printf("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
      printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
      printf ("   names are ");
      for (j = 0; j < names; j++) { /*  for each name */
         printf ("%d ", *ptr);
         if (j == 0)  /*  set row and column  */
            ii = *ptr;
         else if (j == 1)
            jj = *ptr;
         ptr++;
      }
      printf ("\n");
      board[ii][jj] = (board[ii][jj] + 1) % 3;
   }
}


/*  pickSquares() sets up selection mode, name stack, 
 *  and projection matrix for picking.  Then the 
 *  objects are drawn.
 */
#define BUFSIZE 512

void Picksquare::mousePressEvent(QMouseEvent *e)

{
   GLuint selectBuf[BUFSIZE];
   GLint hits;
   GLint viewport[4];

   if (e-> button() != LeftButton) //|| state != GLUT_DOWN)
      return;

   glGetIntegerv (GL_VIEWPORT, viewport);

   glSelectBuffer (BUFSIZE, selectBuf);
   (void) glRenderMode (GL_SELECT);

   glInitNames();
   glPushName(0);

   glMatrixMode (GL_PROJECTION);
   glPushMatrix ();
   glLoadIdentity ();

//  create 5x5 pixel picking region near cursor location        

   gluPickMatrix ((GLdouble) e->x(), (GLdouble) (viewport[3] - e->y()), 
                  5.0, 5.0, viewport);
   gluOrtho2D (0.0, 3.0, 0.0, 3.0);
   drawSquares (GL_SELECT);

   glMatrixMode (GL_PROJECTION);
   glPopMatrix ();
   glFlush ();

   hits = glRenderMode (GL_RENDER);
   processHits (hits, selectBuf);
   update();
} 

// Picksquare function adaped from keyboard function.  This function has
// been modified to accept one argument (the keyboard input), instaed of three
// arguments as is was done in the keyboard function.

void Picksquare::keyPressEvent(QKeyEvent *e)
{
   switch (e -> key()) {
      case 27:
         exit(0);
         break;
   }
}
/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  color index display mode, and handle input events.
 */
int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    Picksquare w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}

