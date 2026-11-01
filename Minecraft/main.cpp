#define  _CRT_SECURE_NO_WARNINGS


#include "header/texture/texture.h"
#include "header/character/character.h"
#include "header/renderer/renderer.h"
#include "header/world/world.h"
#include "header/title/title.h"
#include "header/pause/pause.h"
#include "header/ui/ui.h"
#include <random>
#include <chrono>
#include <filesystem>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dis(0, 1000);


int screenWidth = 1280;
int screenHeight = 720;

// git test

class Light
{
public:
	glm::vec3 pos = glm::vec3(0.0f, 10.0f, 0.0f);
	glm::vec3 colors[4] = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.3f, 0.3f},
		{0.3f, 1.0f, 0.3f},
		{0.3f, 0.3f, 1.0f}
	};
	float r = 7.0f;
	float deg = 0.0f;

	void setPos()
	{
		pos.x = r * glm::sin(glm::radians(deg));
		pos.z = r * glm::cos(glm::radians(deg));
	}
};


void InitBuffer();


int lastX = screenWidth / 2;
int nowX = screenWidth / 2;
int lastY = screenHeight / 2;
int nowY = screenHeight / 2;
Light light;
UI gui;
bool left_button = false, right_button = false;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void IdleFunction();
void SpecialKeyboard(int key, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseWheel(int button, int dir, int x, int y);


Character* player;
Renderer renderer;
GLuint VBO;
World* world;
Title title;
Pause pause;


static bool SetResourceDirectory()
{
    std::vector<wchar_t> path(32768);
    const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (length == 0 || length >= path.size()) {
        std::cerr << "Cannot locate the executable directory.\n";
        return false;
    }
    const auto directory = std::filesystem::path(path.data()).parent_path();
    std::error_code error;
    std::filesystem::current_path(directory, error);
    if (error) {
        std::cerr << "Cannot select the resource directory: " << error.message() << '\n';
        return false;
    }
    if (!std::filesystem::is_directory("resource", error) ||
        !std::filesystem::is_directory("shader", error)) {
        std::cerr << "Keep the resource and shader folders beside MineCraft.exe.\n";
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    if (!SetResourceDirectory()) return EXIT_FAILURE;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(300, 150);

	glutCreateWindow("Minecraft alpha 0.0.0");
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW OK\n";
	}

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(IdleFunction);
	glutSpecialFunc(SpecialKeyboard);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(MouseWheel);

	EnterTexture();
	title.EnterTexture_title();
	pause.EnterTexture_pause();
	world = new World(dis(gen));
	world->GenerateWorld();
	player = new Character(world);

	InitBuffer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();
	return 0;
}


void InitBuffer()
{
	player->Init();
	world->Init();
	gui.Init();
	title.Init();
	pause.Init();
}


void Display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (title.game_start == false)
		title.render_title();
	else if (pause.go_to_pause)
	{
		pause.render_pause();
	}
	else
	{
		renderer.RenderWorld(world);
		renderer.RenderCharacter(player);
		renderer.RenderUI(&gui, player);
	}

	glutSwapBuffers();
}


void Reshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	glViewport(0, 0, w, h);
	renderer.SetScreenSize(w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:                                // ESCAPE
		if (title.game_start)
		{
			pause.go_to_pause = true;
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		else
			glutLeaveMainLoop();
		break;
	}

	if (title.game_start == false)         // 타이틀일 때
	{
		title.Input_keyboard(key);

		if (title.game_start)              // 게임시작 누르면
		{
			renderer.SetupGL();
			renderer = Renderer(&VBO, &player->camera, player, screenWidth, screenHeight);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}
	else if (pause.go_to_pause)                 // 일시정지했을 때
	{
		pause.Input_keyboard(key);

		if (pause.go_to_pause == false)
			glutSetCursor(GLUT_CURSOR_NONE);

		if (pause.go_to_title)
		{
			title.game_start = false;
			pause.go_to_title = false;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
	}
	else
	{
		player->KeyboardInput(key);
		light.setPos();
	}

	glutPostRedisplay();
}


// Existing movement/physics constants are per simulation tick, not per frame.
static int SimulationSteps(double elapsed, double& accumulator)
{
    constexpr double step = 1.0 / 60.0;
    // Discard excessive catch-up time after a debugger stop or long stall.
    if (elapsed > 0.25) elapsed = 0.25;
    accumulator += elapsed;
    int count = 0;
    while (accumulator + 1e-9 >= step) {
        accumulator -= step;
        ++count;
    }
    return count;
}

void IdleFunction()
{
    using Clock = std::chrono::steady_clock;
    static auto previous = Clock::now();
    static double accumulator = 0.0;
    static bool wasPlaying = false;
    const auto now = Clock::now();
    const double elapsed = std::chrono::duration<double>(now - previous).count();
    previous = now;
    const bool playing = title.game_start && !pause.go_to_pause;

    if (playing && wasPlaying) {
        const int steps = SimulationSteps(elapsed, accumulator);
        if (steps > 0) {
            const int midX = screenWidth / 2;
            const int midY = screenHeight / 2;
            // Mouse displacement is already accumulated: consume it only once.
            player->camera.ProcessMouseMovement(nowX - midX, midY - nowY);
            glutWarpPointer(midX, midY);
            nowX = midX;
            nowY = midY;

            for (int i = 0; i < steps; ++i) {
                player->Input();
                player->Update();
                world->SetPlayerPos(player->camera.Position);
                world->Update();
            }
        }
    }
    else {
        accumulator = 0.0;
        if (playing) {
            nowX = screenWidth / 2;
            nowY = screenHeight / 2;
            glutWarpPointer(nowX, nowY);
        }
    }
    wasPlaying = playing;
    // Rendering is independent of the simulation frequency; no FPS cap or sleep.
    glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y)
{
	if (title.game_start == false)
		title.Input_keyboard(key);

	else if (pause.go_to_pause)
		pause.Input_keyboard(key);

	else
		player->SpecialKeyboardInput(key);
	

	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y)
{
	nowX = x;
	nowY = y;

	if (title.game_start == false)          // 타이틀일 때
	{
		float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
		float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));
		title.Input_mouse(button, state, glx, gly);

		if (title.game_start)               // 게임 시작 누르면
		{
			renderer.SetupGL();
			renderer = Renderer(&VBO, &player->camera, player, screenWidth, screenHeight);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}
	else if (pause.go_to_pause)             // 일시정지 했을 때
	{
		float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
		float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));
		pause.Input_mouse(button, state, glx, gly);

		if (pause.go_to_pause == false)
			glutSetCursor(GLUT_CURSOR_NONE);

		if (pause.go_to_title)
		{
			title.game_start = false;
			pause.go_to_title = false;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}

	}
	else
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			left_button = true;
		}
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			right_button = true;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			left_button = false;
		}
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
			right_button = false;
		}

		player->MouseInput(left_button, right_button);
	}

	

	glutPostRedisplay();
}

void MouseWheel(int button, int dir, int x, int y)
{
	if (title.game_start)
	{
		player->MouseWheelInput(dir);
	}
}

void PassiveMotion(int x, int y)
{
	nowX = x;
	nowY = y;

	if (title.game_start == false)
	{
		float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
		float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));
		title.Input_motion(glx, gly);
	}
	else if (pause.go_to_pause)
	{
		float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
		float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));
		pause.Input_motion(glx, gly);
	}

	glutPostRedisplay();
}


void Motion(int x, int y)
{
	nowX = x;
	nowY = y;

	glutPostRedisplay();
}


//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);