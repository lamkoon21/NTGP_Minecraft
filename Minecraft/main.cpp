#define  _CRT_SECURE_NO_WARNINGS


#include "header/network/net_client.h"
#include "header/texture/texture.h"
#include "header/character/character.h"
#include "header/renderer/renderer.h"
#include "header/world/world.h"
#include "header/title/title.h"
#include "header/multi/multi.h"
#include "header/pause/pause.h"
#include "header/ui/ui.h"
#include <random>
#include <chrono>
#include <filesystem>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dis(0, 1000);

//서버로부터 받은 월드 시드
int pendingServerSeed = 0;
HANDLE serverJoinThread = NULL;
HANDLE serverJoinCancelEvent = NULL;
constexpr unsigned int multiTimerInterval = 50;
static bool multiTimerActive = false;
static bool wasPlaying = false;


int screenWidth = 1280;
int screenHeight = 720;

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

int nowX = screenWidth / 2;
int nowY = screenHeight / 2;

Light light;
UI gui;
bool left_button = false, right_button = false;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void IdleFunction();
void MultiplayerTimer(int);
void SpecialKeyboard(int key, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseWheel(int button, int dir, int x, int y);

//-----------------------------------
//CreateWorld() 함수 선언
void CreateWorld(int seed);

// 기존 월드와 플레이어가 존재하는 지 
// 검사하기 위해 nullptr로 초기화
Character* player = nullptr;
Renderer renderer;
GLuint VBO;
World* world = nullptr;
//------------------------------------
Title title;
Pause pause;
Multi multi;

void UpdateMenu()
{
	// 타이틀 -> 멀티 화면
	if (title.go_to_multi)
	{
		title.go_to_multi = false;
		title.game_start = false;
		title.select_menu = 0;

		multi.Open("");
		if (!multiTimerActive)
		{
			multiTimerActive = true;
			glutTimerFunc(multiTimerInterval, MultiplayerTimer, 0);
		}
	}

	// 멀티 화면 또는 일시정지 -> 타이틀
	else if (multi.go_to_title || pause.go_to_title)
	{
		if (serverJoinThread != NULL)
			SetEvent(serverJoinCancelEvent);

		title.go_to_multi = false;
		title.game_start = false;
		title.select_menu = 0;

		wasPlaying = false;
		multi.is_open = false;
		multi.go_to_title = false;
		multi.request_connect = false;
		multi.select_menu = 0;

		pause.go_to_title = false;
		pause.go_to_pause = false;
		pause.escape = false;
		pause.select_pause_menu = 0;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	glutIdleFunc(title.game_start ? IdleFunction : NULL);
}


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
	glutSpecialFunc(SpecialKeyboard);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(MouseWheel);

	EnterTexture();
	title.EnterTexture_title();
	pause.EnterTexture_pause();
	multi.EnterTexture_multi();

	// 기존 월드, 플레이어 생성 코드
	/*
		world = new World(dis(gen));
		world->GenerateWorld();
		player = new Character(world);
	*/

	InitBuffer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();
	if (serverJoinThread != NULL)
	{
		SetEvent(serverJoinCancelEvent);
		WaitForSingleObject(serverJoinThread, INFINITE);
		FinishServerJoin(false);
		CloseHandle(serverJoinThread);
		CloseHandle(serverJoinCancelEvent);
	}
	return 0;
}

//CreateWorld() 함수 정의
void CreateWorld(int seed)
{
	// 기존 플레이어 삭제
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// 기존 월드 삭제
	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}

	// 새로운 월드 생성
	world = new World(seed);
	world->GenerateWorld();
	world->Init();

	// 새로운 플레이어 생성
	player = new Character(world);
	player->Init();

	// 플레이어 위치 설정
	world->SetPlayerPos(player->camera.Position);

	// Renderer의 플레이어 참조 갱신
	renderer.camera = &player->camera;
	renderer.player = player;
}

void StartMultiplayer()
{
	if (serverJoinThread == NULL)
	{
		if (!multi.is_open || !multi.request_connect)
			return;

		// 접속 스레드용 IP 복사
		static char serverIp[INET_ADDRSTRLEN];
		strcpy_s(serverIp, sizeof(serverIp), multi.ip.c_str());
		serverJoinCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (serverJoinCancelEvent != NULL)
		{
			// 서버 입장 요청
			serverJoinThread = CreateThread(NULL, 0, [](LPVOID address) -> DWORD
			{
				uint8_t playerId = 0;
				ConnectServer((const char*)address, playerId, pendingServerSeed, serverJoinCancelEvent);
				return 0;
			}, serverIp, 0, NULL);

			if (serverJoinThread == NULL)
			{
				CloseHandle(serverJoinCancelEvent);
				serverJoinCancelEvent = NULL;
			}
		}

		multi.Open(serverJoinThread != NULL ? "Connecting..." : "Connection Failed");

		glutPostRedisplay();
		return;
	}

	// 대기 없이 접속 스레드의 종료 여부 확인
	if (WaitForSingleObject(serverJoinThread, 0) != WAIT_OBJECT_0)
		return;

	CloseHandle(serverJoinThread);
	CloseHandle(serverJoinCancelEvent);
	serverJoinThread = NULL;
	serverJoinCancelEvent = NULL;

	// 화면을 나간 접속 요청의 결과 무시
	if (!multi.is_open || multi.message != "Connecting...")
	{
		FinishServerJoin(false);
		return;
	}

	if (!FinishServerJoin(true))
	{
		multi.Open("Connection Failed");
		glutPostRedisplay();
		return;
	}

	// 서버 시드로 월드 초기화
	CreateWorld(pendingServerSeed);
	renderer = Renderer(
		&VBO,
		&player->camera,
		player,
		screenWidth,
		screenHeight
	);
	renderer.SetupGL();

	// 게임 시작
	multi.is_open = false;
	title.game_start = true;
	glutSetCursor(GLUT_CURSOR_NONE);
	glutIdleFunc(IdleFunction);
	glutPostRedisplay();
}

void MultiplayerTimer(int)
{
	static int lastFrame = -1;

	StartMultiplayer();

	// 멀티 화면 애니메이션
	if (multi.is_open)
	{
		int frame = multi.AnimationFrame();
		if (frame != lastFrame)
			glutPostRedisplay();
		lastFrame = frame;
	}

	if (multi.is_open || serverJoinThread != NULL)
		glutTimerFunc(multiTimerInterval, MultiplayerTimer, 0);
	else
	{
		multiTimerActive = false;
		lastFrame = -1;
	}
}

void InitBuffer()
{
	// createworld 에서 진행 -> 기존 사항 비활성화
	/*
		player->Init();
		world->Init();
	*/
	gui.Init();
	title.Init();
	pause.Init();
	multi.Init();
}


void Display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (multi.is_open) multi.render_multi(screenWidth, screenHeight);	// 멀티 IP 입력 화면

	else if (title.game_start == false) title.render_title();		// 타이틀

	else if (pause.go_to_pause) pause.render_pause();				// 게임 중 일시정지

	else															// 게임 중
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
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (multi.is_open) multi.Input_keyboard(key);	// 멀티 IP 입력 화면

	else if (title.game_start == false)						// 타이틀
	{
		if (key == 27) glutLeaveMainLoop(); // ESCAPE
		else
		{
			title.Input_keyboard(key);
			if (title.game_start)							// 게임 시작 누름
			{
				// 게임 시작 시 월드 생성 되도록 main 파일 수정
				CreateWorld(dis(gen));

				renderer.SetupGL();
				renderer = Renderer(&VBO, &player->camera, player, screenWidth, screenHeight);
				glutSetCursor(GLUT_CURSOR_NONE);
			}
		}
	}

	else if (pause.go_to_pause)								// 게임 중 일시정지
	{
		pause.Input_keyboard(key);
		if (pause.go_to_pause == false && pause.go_to_title == false) glutSetCursor(GLUT_CURSOR_NONE);
	}

	else													// 게임 중일 때
	{
		if (key == 27) // ESCAPE
		{
			pause.go_to_pause = true;
			pause.escape = true;

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		else
		{
			player->KeyboardInput(key);
			light.setPos();
		}
	}

	UpdateMenu();
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

                // [미구현] 네트워크 갱신과 게임 중 연결 끊김 처리 (설계: IdleFunction 수정)
                // net.Update()와 net.Disconnect()가 아직 없어서 주석으로 둔다.
                // 두 함수를 구현한 뒤 아래 주석을 풀고 필요한 내용을 추가할 것.
                //
                // - net.Update(world, player): 받은 패킷을 반영하고 내 위치를 보낸다.
                //   연결이 끊겼으면 false를 돌려준다. 싱글플레이에서는 아무 일도 하지 않고 true를 돌려주게 만들어,
                //   멀티인지 따로 검사하지 않고 매 틱 부른다.
                //   일시정지 중에는 이 반복문이 돌지 않으므로 그동안 온 패킷은 큐에 쌓였다가 재개할 때 반영된다.
                // - 끊겼으면 net.Disconnect()로 연결을 정리하고 게임을 멈춘 뒤 IP 입력 화면으로 돌아간다.
                // - multi.Open()은 GL 상태를 바꾸지 않는다. 게임 중 켜 둔 깊이 테스트 · 컬링과 숨긴 커서를
                //   여기서 직접 되돌려야 한다(기존 일시정지 진입과 같은 처리).
                //   빠뜨리면 2D 화면이 컬링 · 깊이 테스트에 걸려 IP 입력 화면이 제대로 그려지지 않는다.
                // - break로 반복문을 빠져나오고 남은 스텝은 버린다.
                /*
                if (!net.Update(world, player))
                {
                    net.Disconnect();
                    title.game_start = false;

                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

                    multi.Open("Disconnected");
                    break;
                }
                */
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
	if (multi.is_open) multi.Input_specialkeyboard(key); // 멀티 IP 입력 화면

	else if (title.game_start == false) title.Input_keyboard(key); // 타이틀

	else if (pause.go_to_pause) pause.Input_keyboard(key); // 게임 중 일시정지

	else player->SpecialKeyboardInput(key); // 게임 중

	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y)
{
	nowX = x;
	nowY = y;

	// 마우스 좌표를 OpenGL 좌표로 변환
	float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
	float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));

	if (multi.is_open) multi.Input_mouse(button, state, glx, gly);	// 멀티 IP 입력 화면

	else if (title.game_start == false)	// 타이틀
	{
		title.Input_mouse(button, state, glx, gly);

		if (title.game_start) // 게임 시작 누름
		{
			// 게임 시작 시 월드 생성 되도록 main파일 수정
			CreateWorld(dis(gen));

			renderer.SetupGL();
			renderer = Renderer(&VBO, &player->camera, player, screenWidth, screenHeight);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}

	else if (pause.go_to_pause)	// 게임 중 일시정지
	{
		pause.Input_mouse(button, state, glx, gly);
		if (pause.go_to_pause == false && pause.go_to_title == false) glutSetCursor(GLUT_CURSOR_NONE);
	}

	else // 게임 중
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) left_button = true;
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) right_button = true;
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) left_button = false;
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) right_button = false;

		player->MouseInput(left_button, right_button);
	}

	UpdateMenu();
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

	// 마우스 좌표를 OpenGL 좌표로 변환
	float glx = ((float)x - (float)screenWidth / 2.0f) * (1.0f / ((float)screenWidth / 2.0f));
	float gly = -((float)y - (float)screenHeight / 2.0f) * (1.0f / ((float)screenHeight / 2.0f));

	if (multi.is_open)
	{
		int selected = multi.select_menu;
		multi.Input_motion(glx, gly);
		if (selected == multi.select_menu)
			return;
	}
	else if (title.game_start == false)
	{
		int selected = title.select_menu;
		title.Input_motion(glx, gly);
		if (selected == title.select_menu)
			return;
	}

	else if (pause.go_to_pause) pause.Input_motion(glx, gly); // 일시정지했을 때

	glutPostRedisplay();
}


void Motion(int x, int y)
{
	nowX = x;
	nowY = y;

	if (title.game_start)
		glutPostRedisplay();
}


//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
