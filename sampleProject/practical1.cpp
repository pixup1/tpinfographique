#include <Viewer.hpp>
#include <glm/glm.hpp>

#define SCR_WIDTH 1024
#define SCR_HEIGHT 768

int main( int argc, char* argv[] )
{
	// Stage 1: Create the window and its OpenGL context
    glm::vec4 background_color = glm::vec4(0.8,0.8,0.8,1.0);
	Viewer viewer(SCR_WIDTH, SCR_HEIGHT, background_color);

	// Stage 2: Load resources like shaders, meshes... and make them part of the virtual scene
	// ...
	
	// Stage 3: Our program loop
	while( viewer.isRunning() )
	{
	    viewer.handleEvent(); 	// user interactivity (keyboard/mouse)
	    viewer.draw();		// rasterization (write in framebuffer)
	    viewer.display();		// refresh window
	}
	
	return EXIT_SUCCESS;
}

