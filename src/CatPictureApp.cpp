/**
 * @file CatPictureApp .cpp
 * CSE 274 - Fall 2012
 * My solution for HW01.
 *
 * @authorScott Vincent
 * @date 2012-08-28
 *
 * @note This file is (c) 2012. It is licensed under the 
 * CC BY 3.0 license (http://creativecommons.org/licenses/by/3.0/),
 * which means you are free to use, share, and remix it as long as you
 * give attribution. Commercial uses are allowed.
 *
 *
 * @note This project satisfies goals 
 *	A.1 (rectangle), A.3 (line), 
 *  B.1 (blur), 
 *  E.2 (transparency),  E.5 (animation) and E.6 (mouse interaction)
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "cinder/Rand.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);
	
  private:
	Surface* mySurface_; //The Surface object whose pixel array we will modify
	Surface yose_pic;
	

	//integer for random color
    int rndColor;
	
	//Track how many frames we have shown, for animation purposes
	int frame_number_;
	boost::posix_time::ptime app_start_time_;
	
		
	uint8_t* my_blur_pattern_;
	
	//Width and height of the screen
	static const int AppWidth=800;
	static const int AppHeight=600;
	static const int TextureSize=1024; //Must be the next power of 2 bigger or equal to app dimensions
	
	
	/**
	 * Blur one image, using another as a template.
	 *
	 * Blur the image_to_blur using a standard convolution blur, but the strength of the blur depends on the blur_pattern.
	 * Pixels are fulling blurred if they are black not blurred at all if they are white. Both images must be exactly the
	 * same size as the app window.
	 *
	 * This satisfies the "blur" requirement, goal B.1
	 */
	void selectiveBlur(uint8_t* image_to_blur, uint8_t* blur_pattern);
	
	
	/**
	 * Draw concentric circles, each of width 7, until reaching r.
	 *
	 * This satisfies the "circle" requirement, goal A.2 and "transparency" (E.2)
	 */
	void drawRings(uint8_t* pixels, int x, int y, int r, Color8u c);
	
	/**
	 * This was the original version of drawRings, which was buggy, but looked cool,
	 * so I kept it.
	 */
	void drawAccident(uint8_t* pixels, int x, int y, int r, Color8u c);


	/**
	* This method draws a line across the top of the screen
	* pixels: the Surface array
	* x_start: the x coordinate to start
	* line_width: how long the line is
	* line_height: how many pixels high the line is
	*
	* This satisfies requirement A.3 (line). 
	*/
	void drawLine (uint8_t* pixels, int x_start, int y_start, int line_width, int line_height);

	/**
	* This method draws a rectangle of width x height as given by the call 
	* x_width: the width of rectangle
	* y_height: height of rectangle
	* x_origin: beginning x coordinate
	* y_origin: beginning y coordinate
	*
	* This satisfies requirement A.1 (rectangle)
	*/
	void drawRectangle (uint8_t* pixels, int x_width, int y_height, int x_origin, int y_origin, Color8u c);


	/**
	* This method copies the rectangle above of width x height as given by the call 
	*
	* x_width: the width of rectangle
	* y_height: height of rectangle
	* x_origin: beginning x coordinate
	* y_origin: beginning y coordinate
	* y_original_rect: origin row of the parent rectangle
	*
	* This satisfies requirement  A.5 (copy) and E.2 (transparency)
	*
	*/
	void copyRectangle (uint8_t* pixels, int x_length, int y_height, int x_origin, int y_origin, int y_original_rect, Color8u c);

	/**
	* This method draws a triangle with an x,y origin given 
	*
	* pixels: Surface array
	* side_length: length of triangle side
	* pt_x: origin x-coordinate
	* pt_y: origin y-coordiante
	*
	* This satisfies requirement A.7 (triangle)
	*
	*/
	void drawTriangle (uint8_t* pixels, int side_length, int pt_x, int pt_y, Color8u c );
	
	
	/**
	** This method draws a line 
	*
	* pixels: Surface array
	* x0: beginning x coordinate
	* x1: ending x coordinate
	* y0: beginning y coordinate
	* y1: ending y coordinate
	* y_original_rect: origin row of the parent rectangle
	*
	* This satisfies requirement A.3 (line). 
	*/
	void makeLine ( uint8_t* pixels, int x0, int y0, int x1, int y1, Color8u c);
	

	
	/**
	* This method overlays a color over the entire surface by averaging the base colors with a red tint (255,0,0).
	*
	* This satisfies requirement A.6.
	*/
	void tintOverlay (uint8_t* pixels, Color8u color);


}; // end public AppBasic


void CatPictureApp::prepareSettings(Settings* settings){
	(*settings).setWindowSize(AppWidth,AppHeight);
	(*settings).setResizable(false);
}





/// This method  draws a random colored line at the top of the screen
void CatPictureApp::drawLine(uint8_t* pixels, int x_start, int y_start, int line_width, int line_height){

	rndColor = Rand::randInt (0,255);
	//int y_start = 25;

	if (  (x_start + line_width) <= AppWidth ) {
	for (int y = y_start; y <= (y_start + line_height); y++) {

		for ( int x = x_start; x <= line_width-1; x++){
			pixels [3* (x + y*(TextureSize))  ] = rndColor;
			pixels [3* (x + y*(TextureSize))+1] = rndColor;
			pixels [3* (x + y*(TextureSize))+2] = rndColor;
			}	

		}	    //end y

	}		// end if

}

/// This method  draws a colored rectangle on the screen
void CatPictureApp::drawRectangle(uint8_t* pixels, int x_width, int y_height, int x_origin, int y_origin, Color8u c){

	//int offset = 3*(int x + int y*TextureSize);

	/** from prof
		//Do some bounds checking
	if(endx < 0) return; //Visible part of rectangle is off screen
	if(endy < 0) return; //Visible part of rectangle is off screen
	if(startx >= AppWidth) return; //Visible part of rectangle is off screen
	if(starty >= AppHeight) return; //Visible part of rectangle is off screen
	if(endx >= AppWidth) endx = AppWidth-1;
	if(endy >= AppHeight) endy = AppHeight-1;
	*/


	if ( (x_width < AppWidth) && (y_height < AppHeight) ){
		
		// start at y_origin
		for ( int y = y_origin; y <= (y_height); y++ ){
			for ( int  x = x_origin; x <= (x_origin + x_width); x++ ) {
				pixels [3* (x + y*(TextureSize))  ] = c.r;
				pixels [3* (x + y*(TextureSize))+1] = c.g;
				pixels [3* (x + y*(TextureSize))+2] = c.b;
				//rect_width-1
			}
		}
	} //end if

}

/// This method  draws a colored rectangle on the screen
void CatPictureApp::copyRectangle(uint8_t* pixels, int x_width, int y_height, int x_origin, int y_origin, int y_original_rect, Color8u c){

	//int offset = 3*(int x + int y*TextureSize);
	// assumes y_original of copied rect will be > than the y_original_rect row
	int y_offset = (y_origin - y_original_rect);

	if ( (x_width < AppWidth) && ( (y_height + y_origin) < AppHeight) ){
		
		
		// start at y=300
		//for ( int yi = 300; yi <= (y_height+300); yi++ ){
		for ( int y = y_origin; y <= (y_height + y_origin); y++ ){

			for ( int x = x_origin; x <= (x_origin + x_width); x++ ) {						
				pixels [3* (x + y*TextureSize)  ] = pixels [3* (x + (y - y_offset)*TextureSize)   ]/2 + c.r/2;
				pixels [3* (x + y*TextureSize)+1] = pixels [3* (x + (y - y_offset)*TextureSize) +1]/2 + c.g/2;
				pixels [3* (x + y*TextureSize)+2] = pixels [3* (x + (y - y_offset)*TextureSize) +2]/2 + c.b/2;
					} // end x
				
				}	  // end y
	
	}				  // end if

}


/// This method draws a triangle when gizen the side length
void CatPictureApp::drawTriangle (uint8_t* pixels, int side_length, int pt_x, int pt_y, Color8u c){

	// bounds checking 
	if ( (pt_x <= AppWidth) && ( (pt_x + side_length) <= AppWidth) ) {  // ( sqrt  ((pt_one*pt_one) + (side_length*side_length))  < AppWidth) ) {

	//for (int tri = 0; tri <= 2; tri++){
		int x = pt_x;
		int y = pt_y;
		
		//move one: go right, x+1, y+1
		for ( int m1 = 0; m1 <= side_length; m1++ ){
				pixels [3* (x + (y * TextureSize))  ] = c.r;
				pixels [3* (x + (y * TextureSize))+1] = c.g;
				pixels [3* (x + (y * TextureSize))+2] = c.b; 

				x = x+1;
				y = y+1;
		}// end move 1

		//move two: go left. x-1, y stays the same
		for ( int m1 = 0; m1 <= side_length*2; m1++ ){
				pixels [3* (x + (y * TextureSize))  ] = c.r;
				pixels [3* (x + (y * TextureSize))+1] = c.g;
				pixels [3* (x + (y * TextureSize))+2] = c.b; 

				x = x-1;
		}// end move 2


		//move three: go up. x+1, y-1
		for ( int m1 = 0; m1 <= side_length; m1++ ){
				pixels [3* (x + (y * TextureSize))  ] = c.r;
				pixels [3* (x + (y * TextureSize))+1] = c.g;
				pixels [3* (x + (y * TextureSize))+2] = c.b; 

				x = x+1;
				y = y-1;
		}// end move 3

	}    // end if

	//*/
} // end drawTriangle



void CatPictureApp::makeLine ( uint8_t* pixels, int x0, int y0, int x1, int y1, Color8u c ){ 
								
	 int     x, offset;       
	 float   dy, dx, y, m;
	 

	//formula from geometry
	 dy = y1 - y0;
	 dx = x1 - x0;

	 m = dy / dx;
	 y = y0;

			   // iterate and draw
				for ( x = x0; x <= x1; x++ ) {
				offset = 3 * (x + (y * TextureSize)); 
     
				 pixels [offset  ]=  c.r;
				 pixels [offset +1]= c.g;
				 pixels [offset +2]= c.b; 
      
				  y += m;   //* Step y by slope m 
			   }			// end for loop

}							// end makeLine



	
void CatPictureApp::selectiveBlur(uint8_t* image_to_blur, uint8_t* blur_pattern){
	//Convolution filters tend to overwrite the data that you need, so
	// we keep a temporary copy of the image_to_blur. There are certainly
	// more efficient ways to deal with this problem, but this is simple to
	// understand. 
	static uint8_t work_buffer[3*TextureSize*TextureSize];
	//This memcpy is not much of a performance hit.
	memcpy(work_buffer,image_to_blur,3*TextureSize*TextureSize);
	
	//These are used in right shifts.
	//Both of these kernels actually darken as well as blur.
	uint8_t kernelA[9] = 
	  	//{1/9,1/9,1/9,
		//1/9,1/9,1/9,
		//1/9,1/9,1/9};
		{1,1,1,
		1,1,1,
		1,1,1};
	uint8_t kernelB[9] = 
	  	//{4,3,4,
		//4,2,4,
		//4,3,4};
		{1,1,1,
		1,1,1,
		1,1,1};
	
	uint8_t total_red  =0;
	uint8_t total_green=0;
	uint8_t total_blue =0;
	int offset;
	int k;
	int y,x,ky,kx;
	
	//Visit every pixel in the image, except the ones on the edge.
	//TODO Special purpose logic to handle the edge cases
	for( y=1;y<AppHeight-1;y++){
		for( x=1;x<AppWidth-1;x++){
			
			offset = 3*(x + y*AppWidth);
			if(blur_pattern[offset] < 256/3){
				//Compute the convolution of the kernel with the region around the current pixel
				//I use ints for the totals and the kernel to avoid overflow
				total_red=0;
				total_green=0;
				total_blue=0;
				for( ky=-1;ky<=1;ky++){
					for( kx=-1;kx<=1;kx++){
						offset = 3*(x + kx + (y+ky)*TextureSize);
						k = kernelA[kx+1 + (ky+1)*3];
						total_red   += (work_buffer[offset  ] >> k);
						total_green += (work_buffer[offset+1] >> k);
						total_blue  += (work_buffer[offset+2] >> k);
					}
				}
			} else if(blur_pattern[offset] < 2*256/3){
				//Compute the convolution of the kernel with the region around the current pixel
				//I use ints for the totals and the kernel to avoid overflow
				total_red=0;
				total_green=0;
				total_blue=0;
				for( ky=-1;ky<=1;ky++){
					for( kx=-1;kx<=1;kx++){
						offset = 3*(x + kx + (y+ky)*TextureSize);
						k = kernelB[kx+1 + (ky+1)*3];
						total_red   += (work_buffer[offset  ] >> k);
						total_green += (work_buffer[offset+1] >> k);
						total_blue  += (work_buffer[offset+2] >> k);
					}
				}
			} else {
				offset = 3*(x + y*TextureSize);
				total_red   = work_buffer[offset];
				total_green = work_buffer[offset+1];
				total_blue  = work_buffer[offset+2];
			}
			
			offset = 3*(x + y*TextureSize);
			image_to_blur[offset]   = total_red;
			image_to_blur[offset+1] = total_green;
			image_to_blur[offset+2] = total_blue;
		}
	}
}



void CatPictureApp::drawRings(uint8_t* pixels, int center_x, int center_y, int r, Color8u c){
	//Bounds test
	if(r <= 0) return;
	
	for(int y=center_y-r; y<=center_y+r; y++){
		for(int x=center_x-r; x<=center_x+r; x++){
			//Bounds test, to make sure we don't access array out of bounds
			if(y < 0 || x < 0 || x >= AppWidth || y >= AppHeight) continue;
			
			int dist = (int)sqrt((double)((x-center_x)*(x-center_x) + (y-center_y)*(y-center_y)));
			if(dist <= r){
				if((dist/7)%2 == 1 ){
					int offset = 3*(x + y*TextureSize);
					//By blending the colors I get a semi-transparent effect
					pixels[offset] = pixels[offset]/2 + c.r/2;
					pixels[offset+1] = pixels[offset+1]/2 + c.g/2;
					pixels[offset+2] = pixels[offset+2]/2 + c.b/2;
				}
			}
		}
	}
}

void CatPictureApp::drawAccident(uint8_t* pixels, int center_x, int center_y, int r, Color8u c){
	//Bounds test
	if(r <= 0) return;
	
	int r2 = r*r;
	for(int y=center_y-r; y<=center_y+r; y++){
		for(int x=center_x-r; x<=center_x+r; x++){
			//Bounds test, to make sure we don't access array out of bounds
			if(y < 0 || x < 0 || x >= AppWidth || y >= AppHeight) continue;
			
			int dist = (x-center_x)*(x-center_x) + (y-center_y)*(y-center_y);
			if(dist <= r2){
				if((dist/49)%2 == 0){
					int offset = 3*(x + y*TextureSize);
					pixels[offset] = c.r;
					pixels[offset+1] = c.g;
					pixels[offset+2] = c.b;
				}
			}
		}
	}
}


void CatPictureApp::tintOverlay(uint8_t* pixels, Color8u color)
{	for(int y = 0; y < AppHeight; y++){
		for(int x = 0; x < AppWidth; x++){	

		// fade from purple to green
		pixels[3*(x + y*TextureSize)]   -= color.r;	
		pixels[3*(x + y*TextureSize)+1] -= color.g;	
		pixels[3*(x + y*TextureSize)+2] -= color.b;		
		}	
	}
}

void CatPictureApp::setup()
{
	frame_number_=0;
	
	//This is the setup that everyone needs to do
	mySurface_ = new Surface(TextureSize,TextureSize,false);

	
	///Setup for my convolution function
		//load image into a Surface
		Surface yose_picture(loadImage( loadResource(RES_YOSE) ));

		// declare the address of the image data
		uint8_t* blur_data = yose_picture.getData();	

		// make a new array the size of the image
		my_blur_pattern_ = new uint8_t[AppWidth*AppHeight*3];
	
		// make a copy of the ram locales of the image into the new array
		for(int y=0;y<AppHeight;y++){
			for(int x=0;x<AppWidth;x++){
				int offset = 3*(x + y*AppWidth);
				my_blur_pattern_[offset] = blur_data[offset];
			}
		}
	

} //end setup


void CatPictureApp::mouseDown( MouseEvent event )
{

	//Satisfies E.6, draws a line of random length, height and color at the mouse click
	uint8_t* dataArray = (*mySurface_).getData();
	int rndLength = Rand::randInt (0,400);
	int rndHeight = Rand::randInt (0,6);
		
	int x = event.getX();
	int y = event.getY();
	drawLine (dataArray, x, y, rndLength, rndHeight);


}


void CatPictureApp::update()
{
	//  Get our array of pixel information
	/// see notes [C]
	uint8_t* dataArray = (*mySurface_).getData();
		
		
	/// make drawing calls ///

	// drawLine method
	drawLine (dataArray, 50, 25, 200, 2);

	
	//drawRectangle call
	drawRectangle (dataArray, 200, 200, 0, 50, Color8u(0, 255, 0) );

	//copyRectangle call
	// 100, 100 wil copy just a subset of the above rectangle
	copyRectangle(dataArray, 200, 200, 0, 300, 50, Color8u(255, 255, 255) );

	// drawTriangle call
	drawTriangle (dataArray, 100, 400, 200, Color8u(0, 0, 255) );

	// makeLine call
	makeLine (dataArray, 0, 0, 300, 500, Color8u(0, 255, 0) );

	//tintOverlay call 
	tintOverlay(dataArray, Color8u(, 255, 0));

	// Convolution call
	// convolution(dataArray, convolution_pattern_);


	//With just this method called, frame rate drops from 54 to 11.93
	//selectiveBlur(dataArray, my_blur_pattern_);
	
	
	//
	// End calls
	
	
	
	/// interesting stuff from prof ///
	
	//Only save the first frame of drawing as output
	if(frame_number_ == 0){
		writeImage("vincensd.png",*mySurface_);
		//We do this here, instead of setup, because we don't want to count the writeImage time in our estimate
		app_start_time_ = boost::posix_time::microsec_clock::local_time();
	}
	//keeps track of how many frames we have shown.
	frame_number_++;
	
	//For debugging: Print the actual frames per second
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration msdiff = now - app_start_time_;
	/// prints to debug ouput
    console() << (1000.0*frame_number_) / msdiff.total_milliseconds() << std::endl;
}

void CatPictureApp::draw()
{
	//Draw our texture to the screen, using graphics library
	gl::draw(*mySurface_);

		
}

CINDER_APP_BASIC( CatPictureApp , RendererGl )
