#pragma once
#ifndef MULTIPLAYUI_H
#define MULTIPLAYUI_H

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/ext.hpp"
#include <map>
#include <string>
#include <iostream>
#include "../shader/shader.h"
#include "../stb_image/stb_image.h"

float MUI_background[] = {
	// 위치               // 컬러             // 텍스처 좌표
	1.2f,  1.2f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 우측 상단
	1.2f, -2.4f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 우측 하단
   -1.2f, -2.4f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 좌측 하단
   -1.2f,  1.2f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 좌측 상단
};

float MUI_IP_input_window[] = {
	// 위치                   // 컬러             // 텍스처 좌표
	0.4005f,  0.10f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 우측 상단
	0.4005f, -0.06f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.9f,   // 우측 하단
   -0.4995f, -0.06f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.9f,   // 좌측 하단
   -0.4995f,  0.10f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 좌측 상단
};

float MUI_enter_ip_words[] = {
	// 위치                    // 컬러             // 텍스처 좌표
	0.0945f, 0.234f, 0.0f,   1.0f, 0.0f, 0.0f,   0.605f, 0.07f,   // 우측 상단
	0.0945f, 0.170f, 0.0f,   0.0f, 1.0f, 0.0f,   0.605f, 0.03f,   // 우측 하단
   -0.0945f, 0.170f, 0.0f,   0.0f, 0.0f, 1.0f,   0.395f, 0.03f,   // 좌측 하단
   -0.0945f, 0.234f, 0.0f,   1.0f, 1.0f, 0.0f,   0.395f, 0.07f    // 좌측 상단
};

float MUI_enter_button[] = {
	// 위치                   // 컬러             // 텍스처 좌표
	0.4995f,  0.10f, 0.0f,  1.0f, 0.0f, 0.0f,   0.1f, 0.5f,   // 우측 상단
	0.4995f, -0.06f, 0.0f,  0.0f, 1.0f, 0.0f,   0.1f, 0.4f,   // 우측 하단
	0.4095f, -0.06f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.4f,   // 좌측 하단
	0.4095f,  0.10f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.5f    // 좌측 상단
};

float MUI_enter_button_activate[] = {
	// 위치                   // 컬러             // 텍스처 좌표
	0.4995f,  0.10f, 0.0f,  1.0f, 0.0f, 0.0f,   0.1f, 0.4f,   // 우측 상단
	0.4995f, -0.06f, 0.0f,  0.0f, 1.0f, 0.0f,   0.1f, 0.3f,   // 우측 하단
	0.4095f, -0.06f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.3f,   // 좌측 하단
	0.4095f,  0.10f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.4f    // 좌측 상단
};

float MUI_quit_button[] = {
	// 위치                     // 컬러             // 텍스처 좌표
	0.21375f, -0.32f, 0.0f,   1.0f, 0.0f, 0.0f,   0.475f, 0.8f,   // 우측 상단
	0.21375f, -0.48f, 0.0f,   0.0f, 1.0f, 0.0f,   0.475f, 0.7f,   // 우측 하단
   -0.21375f, -0.48f, 0.0f,   0.0f, 0.0f, 1.0f,   0.000f, 0.7f,   // 좌측 하단
   -0.21375f, -0.32f, 0.0f,   1.0f, 1.0f, 0.0f,   0.000f, 0.8f    // 좌측 상단
};

float MUI_quit_button_activate[] = {
	// 위치                     // 컬러             // 텍스처 좌표
	0.21375f, -0.32f, 0.0f,   1.0f, 0.0f, 0.0f,   0.475f, 0.7f,   // 우측 상단
	0.21375f, -0.48f, 0.0f,   0.0f, 1.0f, 0.0f,   0.475f, 0.6f,   // 우측 하단
   -0.21375f, -0.48f, 0.0f,   0.0f, 0.0f, 1.0f,   0.000f, 0.6f,   // 좌측 하단
   -0.21375f, -0.32f, 0.0f,   1.0f, 1.0f, 0.0f,   0.000f, 0.7f    // 좌측 상단
};

float MUI_enter_arrow[] = {
	// 위치                    // 컬러             // 텍스처 좌표
	0.47925f,  0.064f, 0.0f,  1.0f, 0.0f, 0.0f,   0.525f, 0.280f,   // 우측 상단
	0.47925f, -0.024f, 0.0f,  0.0f, 1.0f, 0.0f,   0.525f, 0.225f,   // 우측 하단
	0.42975f, -0.024f, 0.0f,  0.0f, 0.0f, 1.0f,   0.470f, 0.225f,   // 좌측 하단
	0.42975f,  0.064f, 0.0f,  1.0f, 1.0f, 0.0f,   0.470f, 0.280f    // 좌측 상단
};

float MUI_quit_words[] = {
	// 위치                    // 컬러             // 텍스처 좌표
	0.1305f, -0.368f, 0.0f,  1.0f, 0.0f, 0.0f,   0.645f, 0.17f,   // 우측 상단
	0.1305f, -0.432f, 0.0f,  0.0f, 1.0f, 0.0f,   0.645f, 0.13f,   // 우측 하단
   -0.1305f, -0.432f, 0.0f,  0.0f, 0.0f, 1.0f,   0.355f, 0.13f,   // 좌측 하단
   -0.1305f, -0.368f, 0.0f,  1.0f, 1.0f, 0.0f,   0.355f, 0.17f    // 좌측 상단
};

unsigned int MUI_indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
};

GLuint VAO_MUI_background;
GLuint VAO_MUI_ui[5];
GLuint VAO_MUI_words[3];

GLuint VBO_MUI_background;
GLuint VBO_MUI_ui[5];
GLuint VBO_MUI_words[3];

GLuint EBO_MUI_background;
GLuint EBO_MUI_ui[5];
GLuint EBO_MUI_words[3];

std::map<std::string, unsigned int> multi_textures = { };

// ============================================================================
// Multi : 멀티플레이 IP 입력 화면
// ============================================================================
// 타이틀에서 Multiplayer를 고르면 열리고, 접속 실패나 게임 중 연결 끊김 때도 다시 열린다.
// 기존 Title, Pause와 같은 규칙(render_..., Input_..., go_to_... 플래그)을 따른다.
// 전역 객체 이름은 multi다 (connect로 지으면 Winsock의 connect()와 겹친다).
//
// main.cpp에서 쓰는 법
//   InitBuffer()        : multi.Init()
//   타이틀에서 멀티 선택 : multi.Open("")
//   Display()           : if (multi.is_open) multi.render_multi();
//   Keyboard() 등       : if (multi.is_open) multi.Input_...(...)  (다른 화면보다 먼저 분기)
//   request_connect     : Enter를 눌렀다 -> main이 접속을 시도하고 false로 되돌린다
//   go_to_title         : ESC 또는 Quit to Title -> main이 타이틀로 돌리고 false로 되돌린다
// ============================================================================
class Multi
{
public:
	Shader multiShader;

	bool is_open = false;          // 이 화면이 열려 있는지
	std::string ip;                // 입력한 IP. 화면을 다시 열어도 지우지 않는다
	std::string message;           // IP 칸 아래에 띄울 문구 (Connecting..., Connection failed, Disconnected)
	int select_menu = 0;           // 1 = Enter 버튼, -1 = Quit to Title 버튼, 0 = 선택 없음

	// main에서 요청을 처리한 뒤 false로 되돌림
	bool request_connect = false;
	bool go_to_title = false;

	// ------------------------------------------------------------------------
	// Init : 멀티 화면에 쓸 사각형 VAO를 모두 만든다. InitBuffer()에서 한 번 부른다.
	// ------------------------------------------------------------------------
	void Init()
	{
		// 두 번째 호출부터는 셰이더와 GL 객체를 다시 만들지 않는다 (누수 방지)
		if (initialized) return;
		initialized = true;

		multiShader.setShader("shader/vertex_title.glsl", "shader/fragment_title.glsl"); // title 셰이더 재활용

		// 배경
		CreateQuad(MUI_background, sizeof(MUI_background), VAO_MUI_background, VBO_MUI_background, EBO_MUI_background);

		// 버튼과 입력 칸 : render_multi()가 select_menu에 따라 번호로 골라 그린다
		CreateQuad(MUI_enter_button,          sizeof(MUI_enter_button),          VAO_MUI_ui[0], VBO_MUI_ui[0], EBO_MUI_ui[0]); // 0 : Enter 버튼
		CreateQuad(MUI_quit_button,           sizeof(MUI_quit_button),           VAO_MUI_ui[1], VBO_MUI_ui[1], EBO_MUI_ui[1]); // 1 : Quit to Title 버튼
		CreateQuad(MUI_IP_input_window,       sizeof(MUI_IP_input_window),       VAO_MUI_ui[2], VBO_MUI_ui[2], EBO_MUI_ui[2]); // 2 : IP 입력 칸
		CreateQuad(MUI_enter_button_activate, sizeof(MUI_enter_button_activate), VAO_MUI_ui[3], VBO_MUI_ui[3], EBO_MUI_ui[3]); // 3 : Enter 버튼 (선택됨)
		CreateQuad(MUI_quit_button_activate,  sizeof(MUI_quit_button_activate),  VAO_MUI_ui[4], VBO_MUI_ui[4], EBO_MUI_ui[4]); // 4 : Quit to Title 버튼 (선택됨)

		// 글자 : 같은 번호의 VAO_MUI_ui[0 ~ 2] 위에 겹쳐 그린다
		CreateQuad(MUI_enter_arrow,    sizeof(MUI_enter_arrow),    VAO_MUI_words[0], VBO_MUI_words[0], EBO_MUI_words[0]); // 0 : Enter 버튼의 화살표
		CreateQuad(MUI_quit_words,     sizeof(MUI_quit_words),     VAO_MUI_words[1], VBO_MUI_words[1], EBO_MUI_words[1]); // 1 : "Quit to Title"
		CreateQuad(MUI_enter_ip_words, sizeof(MUI_enter_ip_words), VAO_MUI_words[2], VBO_MUI_words[2], EBO_MUI_words[2]); // 2 : "Enter IP"

		glBindVertexArray(0);
	}

	// ------------------------------------------------------------------------
	// Open : 화면을 열고 IP 칸 아래에 띄울 문구를 정한다
	// ------------------------------------------------------------------------
	// 처음 들어올 때 Open(""), 접속 중 Open("Connecting..."), 실패 Open("Connection failed"),
	// 게임 중 끊김 Open("Disconnected")처럼 모두 이 함수 하나로 연다.
	// 마지막으로 입력한 IP는 그대로 둔다. 깊이 테스트, 컬링, 커서는 부르는 쪽(main)이 정리한다.
	void Open(const std::string& msg)
	{
		is_open = true;
		message = msg;
		request_connect = false;
		go_to_title = false;
		select_menu = 0;
	}

	// ------------------------------------------------------------------------
	// render_multi : 배경, IP 입력 칸, 안내 문구, 버튼, 입력한 IP, 메시지를 그린다
	// ------------------------------------------------------------------------
	void render_multi()
	{
		multiShader.use();

		UseTexture_multi("background");
		glBindVertexArray(VAO_MUI_background);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		UseTexture_multi("button");
		for (int i = 0; i < 3; ++i)
		{
			if (i == 0 && select_menu == 1)
				glBindVertexArray(VAO_MUI_ui[3]);
			else if (i == 1 && select_menu == -1)
				glBindVertexArray(VAO_MUI_ui[4]);
			else
				glBindVertexArray(VAO_MUI_ui[i]);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindVertexArray(VAO_MUI_words[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);

		// 글자는 freeglut의 glutBitmapString()으로 그린다.
		// 비트맵 글자도 셰이더를 거치므로, 셰이더를 끈 고정 파이프라인에서 그려야 지정한 색 그대로 나온다.
		// 다음 화면은 그리기 전에 자기 셰이더를 다시 use()한다.
		glUseProgram(0);

		// 입력한 IP : 입력 칸(MUI_IP_input_window, x -0.4995 ~ 0.4005, y -0.06 ~ 0.10) 안 왼쪽.
		// 0.5초마다 깜빡이는 커서(_)를 뒤에 붙인다.
		std::string ip_text = ip;
		if ((glutGet(GLUT_ELAPSED_TIME) / 500) % 2 == 0) ip_text += '_';
		DrawText(ToPixelX(-0.4995f) + TEXT_PADDING, ToPixelY(0.02f) - TEXT_HALF_HEIGHT, ip_text, 1.0f, 1.0f, 1.0f);

		// 메시지 : 입력 칸(아래 끝 -0.06)과 Quit to Title 버튼(위 끝 -0.32) 사이 가운데
		if (!message.empty())
		{
			const int width = glutBitmapLength(TextFont(), reinterpret_cast<const unsigned char*>(message.c_str()));
			DrawText(ToPixelX(0.0f) - width / 2, ToPixelY(-0.19f) - TEXT_HALF_HEIGHT, message, 0.9f, 0.9f, 0.9f);
		}
	}

	// ------------------------------------------------------------------------
	// Input_keyboard : 일반 키 입력. Keyboard()에서 부른다
	// ------------------------------------------------------------------------
	// 숫자와 점은 IP에 덧붙이고(최대 15자), Backspace는 한 글자 지운다.
	// Enter는 request_connect(Quit to Title을 고른 상태면 go_to_title), ESC는 go_to_title을 켠다.
	void Input_keyboard(unsigned char key)
	{
		switch (key)
		{
		case 8: // BACKSPACE
			if (!ip.empty())
				ip.pop_back();

			select_menu = 1;
			break;

		case 13: // ENTER
			if (select_menu == -1)
				go_to_title = true;
			else if (!ip.empty())
				request_connect = true;
			break;

		case 27: // ESCAPE
			go_to_title = true;
			break;

		default:
			// IPv4 주소: 숫자와 점만 입력, 최대 15글자 (255.255.255.255)
			if ((key >= '0' && key <= '9') || key == '.')
			{
				if (ip.size() < MAX_IP_LENGTH)
					ip += key;

				select_menu = 1;
			}
			break;
		}
	}

	// 방향키 입력 : SpecialKeyboard()에서 부른다. 위아래로 Enter와 Quit to Title을 오간다
	void Input_specialkeyboard(int key)
	{
		switch (key)
		{
		case GLUT_KEY_UP:
			if (select_menu == 0)
				select_menu = 1;
			else
				select_menu *= -1;
			break;

		case GLUT_KEY_DOWN:
			if (select_menu == 0)
				select_menu = -1;
			else
				select_menu *= -1;
			break;
		}
	}

	// 마우스 클릭 : Mouse()에서 부른다. x, y는 OpenGL 좌표(-1 ~ 1)
	void Input_mouse(int button, int state, float x, float y)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			// 클릭한 위치에 맞춰 메뉴 선택 갱신
			Input_motion(x, y);

			// enter button
			if (select_menu == 1 && !ip.empty()) request_connect = true;

			// quit button
			else if (select_menu == -1) go_to_title = true;
		}
	}

	// 마우스 이동 : PassiveMotion()에서 부른다. 커서가 올라간 버튼을 고른다
	void Input_motion(float x, float y)
	{
		// enter button
		if (0.4095f < x && x < 0.4995f && -0.06f < y && y < 0.10f) select_menu = 1;

		// quit button
		else if (-0.21375f < x && x < 0.21375f && -0.48f < y && y < -0.32f) select_menu = -1;

		else select_menu = 0;
	}

	// ------------------------------------------------------------------------
	// 텍스처 : main()에서 EnterTexture_multi()를 한 번 부른다
	// ------------------------------------------------------------------------
	void EnterTexture_multi()
	{
		multi_textures.clear();
		// multi tex
		InsertTexture_multi("background", "resource/texture/title/background.png"); // title background 재활용
		// ui tex
		InsertTexture_multi("button", "resource/texture/ui/multiplay_ui.png");
	}

	void UseTexture_multi(const char* key)
	{
		if (multi_textures.find(key) != multi_textures.end()) {
			glBindTexture(GL_TEXTURE_2D, multi_textures[key]);
		}
		else {
			std::cerr << "ERROR! Can't use undefined key\n";
		}
	}

	void SetTexture_multi(const char* filename, unsigned int& texture)
	{
		stbi_set_flip_vertically_on_load(true);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << filename << " Failed to load texture\n" << std::endl;
		}
		stbi_image_free(data);
	}

	void InsertTexture_multi(const char* key, const char* filename)
	{
		if (multi_textures.find(key) != multi_textures.end()) {
			std::cerr << "ERROR! Duplicate texture key can\'t be used\n";
		}
		else {
			unsigned int texture;
			SetTexture_multi(filename, texture);
			multi_textures.insert({ key, texture });
		}
	}

private:
	static constexpr std::size_t MAX_IP_LENGTH = 15;  // IPv4 주소 최대 길이 (255.255.255.255)

	// 글자 : freeglut 비트맵 글꼴. ASCII만 그릴 수 있어서 메시지는 영어로 쓴다.
	static constexpr int TEXT_PADDING = 12;      // 입력 칸 왼쪽 끝에서 글자까지의 픽셀
	static constexpr int TEXT_HALF_HEIGHT = 6;   // 글자 높이(약 13픽셀)의 절반. 가운데 맞춤에 쓴다
	static void* TextFont() { return GLUT_BITMAP_HELVETICA_18; }

	bool initialized = false; // Init()을 이미 불렀는지

	// 사각형 하나의 VAO, VBO, EBO를 만든다.
	// vertices : 정점 4개 x (위치 3 + 컬러 3 + 텍스처 좌표 2)개의 float, size : 그 바이트 수
	static void CreateQuad(const float* vertices, GLsizeiptr size, GLuint& vao, GLuint& vbo, GLuint& ebo)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MUI_indices), MUI_indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// OpenGL 좌표(-1 ~ 1)를 창의 픽셀 좌표(왼쪽 아래가 0)로 바꾼다. 창 크기가 바뀌어도 맞는다.
	static int ToPixelX(float x) { return static_cast<int>((x + 1.0f) * 0.5f * glutGet(GLUT_WINDOW_WIDTH)); }
	static int ToPixelY(float y) { return static_cast<int>((y + 1.0f) * 0.5f * glutGet(GLUT_WINDOW_HEIGHT)); }

	// 창 픽셀 좌표 (x, y)를 글자 기준선의 시작점으로 해서 text를 그린다.
	// 색은 glWindowPos 전에 정해야 그 글자에 적용된다.
	static void DrawText(int x, int y, const std::string& text, float r, float g, float b)
	{
		glColor3f(r, g, b);
		glWindowPos2i(x, y);
		glutBitmapString(TextFont(), reinterpret_cast<const unsigned char*>(text.c_str()));
	}
};


#endif
