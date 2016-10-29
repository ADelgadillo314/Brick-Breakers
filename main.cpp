#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream> 
#include <string> 


//start of the program 

GLuint loadTexture(const std::string &fileName)
{
	SDL_Surface *image = IMG_Load(fileName.c_str() );

	SDL_DisplayFormatAlpha(image);

	unsigned object(0);

	glGenTextures(1, &object);
	glBindTexture(GL_TEXTURE_2D, object);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image->w,image->h,0,GL_RGBA,GL_UNSIGNED_BYTE,
		image->pixels);
	//free surface
	SDL_FreeSurface(image);

	return object;

}


bool init()
{
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Set opengl memory usage
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	//Caption of the window 
	SDL_WM_SetCaption("Brick Breakers",NULL);

	//Size of the window 
	SDL_SetVideoMode(600,400,32,SDL_OPENGL);

	//Specify the clear color (white) 
	glClearColor(0,0,0,1); //Red,Green, Blue, Alpha

	//Portion of screen displayed 
	glViewport(0,0,600,400);

	//Shading Fixer
	glShadeModel(GL_SMOOTH);

	//promise to 2d render + save
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//disable depth checking
	glDisable(GL_DEPTH_TEST);

	return true;
}

void keyEvents(SDL_Event& event,bool& isRunning);
void mouseEvents(SDL_Event& event);
void exampleDrawing();

bool checkCol(double Ax, double Ay, double Aw, double Ah, double Bx, double By, double Bw, double Bh); 


struct Brick
{
	float x;
	float y;
	float width;
	float hieght;
	bool  alive;
};



int main(int argc, char**)
{
	//initialize everything 
	if(! init() )
		return 1;



	bool isRunning = true;

	//how to detect key strokes 
	SDL_Event event;

	double rectX = 300;
	double rectY = 350;
	double width = 80;
	double height = 20;

	bool left = false;
	bool right = false;

	//ball code
	double ballX = 100;
	double ballY = 350;
	double ballWH = 20; 
	//object moves itself 
	double velX = 0.4;
	double velY = -0.4;

	bool paused = false;

	//brick elements
	const static int BRICKS= 36;
	Brick bricks[BRICKS];

	for( int i = 0, x = 6, y = 10; i < BRICKS; i++, x+= 66)
	{
		if( x > 560 )
		{
			x = 6;
			y += 30;
		}
		bricks[i].x = x;
		bricks[i].y = y;
		bricks[i].width = 60;
		bricks[i].hieght = 20;
		bricks[i].alive = true;
	}


	//main game loop 
	while(isRunning )
	{
		//INPUT/EVENTS
		while(SDL_PollEvent(&event) )
		{
			keyEvents(event,isRunning);
			mouseEvents(event);

			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_LEFT)
				{
					left = true;
				}
				if(event.key.keysym.sym == SDLK_RIGHT)
				{
					right = true;
				}
			}
			else if(event.type == SDL_KEYUP)
			{
				if(event.key.keysym.sym == SDLK_LEFT)
				{
					left = false;
				}
				if(event.key.keysym.sym == SDLK_RIGHT)
				{
					right = false;
				}
			}

			if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE)
			{
				if( velX == 0 && velY == 0)
				{
					paused = false;
				}
				else
					paused = true;
			}
		}

		//LOGIC
		if( left)
			rectX -= 1.2;
		if(right)
			rectX += 1.2;
		if(rectX < 0)
			rectX = 0;
		if( rectX + width > 600)
			rectX = 600 - width; 


		//ball logic
		ballX += velX;

		for( int i = 0; i < BRICKS; i++)
		{
			if( bricks[i].alive == true)
			{
				if( checkCol(ballX,ballY,ballWH,ballWH,bricks[i].x,bricks[i].y,bricks[i].width,bricks[i].hieght) )
				{
					velX = -velX;
					bricks[i].alive = false;
					break;
				}
			}

		}

		ballY += velY;

		for( int i = 0; i < BRICKS; i++)
		{
			if( bricks[i].alive == true)
			{
				if( checkCol(ballX,ballY,ballWH,ballWH,bricks[i].x,bricks[i].y,bricks[i].width,bricks[i].hieght) )
				{
					velY = -velY;
					bricks[i].alive = false;
					break;
				}
			}
		}


		if(ballX < 0 )
			velX = -velX;
		else if(ballX+ ballWH > 600)
			velX = - velX;

		if( ballY < 0)
			velY = -velY;
		else if ( ballY + ballWH > 400)
		{
			rectX = 300;
			rectY = 350;
			width = 80;
			height = 20;

			left = false;
			right = false;

			//ball code
			ballX = 100;
			ballY = 350;
			ballWH = 20; 
			//object moves itself 
			velX = .4;
			velY = -.4;


			for( int i = 0, x = 6, y = 10; i < BRICKS; i++, x+= 66)
			{
				bricks[i].alive = true;
			}

		}

		if( checkCol(ballX, ballY, ballWH,ballWH, rectX, rectY, width, height) )
			velY = -velY;

		double tempVelX = velX;
		double tempVelY = velY;

		if( paused == true) 
		{
			velX = 0;
			velY = 0; 
		}
		else if( paused == false)
		{
			velX = tempVelX;
			velY = tempVelY;
		}
		




		//RENDERING (drawing to screen) 

		//clears the screen (white because we set that. )
		glClear(GL_COLOR_BUFFER_BIT	);

		//Start of Drawing brings grid()
		glPushMatrix();
		{
			glOrtho(0,600,400,0,-1,1); //set the matrix (grid)


			glColor4ub(0,200,0,255);
			glBegin(GL_QUADS);
			{
				glVertex2d(rectX,rectY);
				glVertex2d(rectX + width, rectY);
				glVertex2d(rectX + width, rectY + height);
				glVertex2d(rectX, rectY + height);
			}
			glEnd();
			//exampleDrawing();




			glBegin(GL_QUADS);
			{
				glColor4ub(255,255,0,255);

				glVertex2d(ballX,ballY);
				glVertex2d(ballX + ballWH, ballY);
				glVertex2d(ballX + ballWH, ballY + ballWH);
				glVertex2d(ballX, ballY + ballWH);
			}
			glEnd();

			glBegin(GL_QUADS);
			{

				for( int i = 0 ; i < BRICKS; i++)
				{
					if( bricks[i].alive)
					{

						if( i %2 == 0)
							glColor4ub(0,0,150,255);
						else 
							glColor4ub(0,0,250,255);
						glVertex2f(bricks[i].x,bricks[i].y);
						glVertex2f(bricks[i].x + bricks[i].width, bricks[i].y);
						glVertex2f(bricks[i].x + bricks[i].width, bricks[i].y + bricks[i].hieght);
						glVertex2f(bricks[i].x , bricks[i].y + bricks[i].hieght);
					}
				}

			}
			glEnd();

		}
		glPopMatrix();




		SDL_GL_SwapBuffers();
		SDL_Delay(1); //slows down the FPS
	}




	SDL_Quit();
	return 0;
}

void keyEvents(SDL_Event& event, bool& isRunning)
{
	if((event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE) ||
		event.type == SDL_QUIT )
		isRunning = false;
	//Code for detecting specific keys 
	if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
	{
		//change color 
		glClearColor(1,0,0,0);
		std::cout << " R has been pressed" << std::endl;
	}

	if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_r)
	{
		glClearColor(0,1,0,1);
		std::cout << " R has been released" << std::endl;
	}
	if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_b)
	{
		glClearColor(0,0,0,1);
	}

}

void mouseEvents(SDL_Event& event)
{
	if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
	{
		int x,y;

		//assign values to coordinate of screen 
		SDL_GetMouseState(&x,&y);

		std::cout << " You clicked at the Coordinates :  " << x << " , " << y << std::endl;

	}
}

void exampleDrawing()
{

	//start definition of shape: GL_POINTS , GL_LINES GL_STRIP, GL_LINE_LOOP, GL_QUADS
	glLineWidth(30.0);
	glBegin(GL_LINES); 
	{
		//define color
		glColor4ub(255,0,0,255);

		//last letter indicates type
		glVertex2d(0,0);
		glVertex2d(600,400);

	} 
	glEnd();

	//has to outside definition of shape. 
	glLineWidth(1);
	glBegin(GL_QUADS);
	{
		//will stay previous
		glColor4ub(0,0,255,255);
		//changing colors inbetween drawing = fading 

		glVertex2d(20,20); //topleft
		glVertex2d(30,20);//top right
		glVertex2d(30,30); //bottom right
		glVertex2d(20,30);  //bottom left
	}
	glEnd();

	// Z- Tetromino
	glBegin(GL_LINE_LOOP);
	{
		glColor4ub(0,255,0,255);

		glVertex2d(50,50); 
		glVertex2d(50,150);
		glVertex2d(150,150);

		glVertex2d(150,250);
		glVertex2d(350,250);
		glVertex2d(350,150);
		glVertex2d(250,150);
		glVertex2d(250,50);

	}
	glEnd();

}

bool checkCol(double Ax, double Ay, double Aw, double Ah, double Bx, double By, double Bw, double Bh)
{
	//check if it is in the four states.. if not it is a collison

	if( Ay + Ah < By) return false;
	if( Ay > By + Bh) return false;
	if( Ax + Aw < Bx) return false;
	if( Ax > Bx + Bw) return false;

	return true;
}
