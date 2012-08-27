/**
* @file: CatProject.cpp
* Our first app to display an image
*
* @author:  Scott Vincent
* @date: 8/23/2010
*
* @ note:
*
*/

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatProjectApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

private:
	//float brightness;
	float clrVar, rndVar; 	
};

void CatProjectApp::setup()
{
	/// initialize vars
	//brightness = 1.0; 
	clrVar = 0.0f;
	rndVar = 0.0f;
}

void CatProjectApp::mouseDown( MouseEvent event )
{

}

void CatProjectApp::update()
{
	/**
	// prof code
	brightness = brightness - 0.01f;

	if (brightness < 0.0f){
		brightness = 1.0f;
	}
	*/

	clrVar = sin( getElapsedSeconds() ) * 0.5f + 0.5f;
	rndVar = Rand::randFloat ( .9f );
}

void CatProjectApp::draw()
{
	 // gl::clear( Color( brightness, brightness, brightness ) ); // prof code
	
	/// change the background color via clrVar variable and rndVar
	 gl::clear( Color( clrVar+rndVar, clrVar, clrVar+rndVar ), true); /// perform the draw of random purple background
	
}

CINDER_APP_BASIC( CatProjectApp, RendererGl )
