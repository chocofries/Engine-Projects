#include <iostream>
#include "game.h"
#include "ShaderLoader.h"
#include "inputmanager.h"
#include "utils.h"


Game* globalGame;

void init()
{
	globalGame = &Game::GetGameInstance();
	globalGame->Initialise(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void update()
{
	globalGame->Update();
}

void render()
{
	globalGame->Draw();
}

void resize(int x, int y)
{
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void KeyboardDown(unsigned char _key, int x, int y)
{
	globalGame->GetInputManager()->KeyboardDown(_key, x, y);
}

void KeyboardUp(unsigned char _key, int x, int y)
{
	globalGame->GetInputManager()->KeyboardUp(_key, x, y);
}

void MouseClick(int button, int state, int x, int y)
{
	globalGame->GetInputManager()->MouseClick(button, state, x, y);
	globalGame->SetScreenMouseX(x);
	globalGame->SetScreenMouseY(y);
}

void MousePassiveMove(int x, int y)
{
	globalGame->SetScreenMouseX(x);
	globalGame->SetScreenMouseY(y);
}

void MouseMove(int x, int y)
{
	globalGame->SetScreenMouseX(x);
	globalGame->SetScreenMouseY(y);
}

void MouseWheel(int button, int dir, int x, int y)
{
	globalGame->GetInputManager()->MouseWheel(button, dir, x, y);
}

void SpecialDown(int _key, int x, int y)
{
	globalGame->GetInputManager()->SpecialKeyDown(_key, x, y);
}

void SpecialUp(int _key, int x, int y)
{
	globalGame->GetInputManager()->SpecialKeyUp(_key, x, y);
}

void ShutDown()
{
	delete globalGame;
	globalGame = nullptr;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Rhythm Worm 0.0.1");
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //clear

	//Init Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Init Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glewInit();
	init();

	glutFullScreen();
	glutIgnoreKeyRepeat(1);
	glutSetCursor(GLUT_CURSOR_NONE);
	// Register callbacks
	// glutReshapeFunc(resize);

	// Input callbacks
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialDown);
	glutSpecialUpFunc(SpecialUp);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutPassiveMotionFunc(MousePassiveMove);

	// Loop callbacks
	glutIdleFunc(update);
	glutDisplayFunc(render);
	glutCloseFunc(ShutDown);
	glutMainLoop();

	return 0;
}