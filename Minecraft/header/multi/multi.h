#pragma once
#ifndef MULTI_H
#define MULTI_H

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/ext.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <map>
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

const float MUI_status_words[][4 * 8] = {
	{
		// 위치                            // 컬러             // 텍스처 좌표
		-0.011f,  -0.04f,   0.0f,    1.0f, 0.0f, 0.0f,   0.5f, 1.0f,   // 오른 위
		-0.011f,  -0.206f,  0.0f,    0.0f, 1.0f, 0.0f,   0.5f, 0.9f,   // 오른 아래
		-0.48f,   -0.206f,  0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.9f,   // 왼쪽 아래
		-0.48f,   -0.04f,   0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 왼쪽 위
	},
	{
		-0.011f,  -0.04f,   0.0f,    1.0f, 0.0f, 0.0f,   0.5f, 0.9f,
		-0.011f,  -0.206f,  0.0f,    0.0f, 1.0f, 0.0f,   0.5f, 0.8f,
		-0.48f,   -0.206f,  0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.8f,
		-0.48f,   -0.04f,   0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 0.9f
	},
	{
		-0.011f,  -0.04f,   0.0f,    1.0f, 0.0f, 0.0f,   0.5f, 0.8f,
		-0.011f,  -0.206f,  0.0f,    0.0f, 1.0f, 0.0f,   0.5f, 0.7f,
		-0.48f,   -0.206f,  0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.7f,
		-0.48f,   -0.04f,   0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 0.8f
	},
	{
		// 위치                            // 컬러             // 텍스처 좌표
		-0.011f,  -0.04f,   0.0f,    1.0f, 0.0f, 0.0f,   0.5f, 0.7f,   // 오른 위
		-0.011f,  -0.206f,  0.0f,    0.0f, 1.0f, 0.0f,   0.5f, 0.6f,   // 오른 아래
		-0.48f,   -0.206f,  0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.6f,   // 왼쪽 아래
		-0.48f,   -0.04f,   0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 0.7f    // 왼쪽 위
	}
};

unsigned int MUI_indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
};

GLuint VAO_MUI_background;
GLuint VAO_MUI_ui[5];
GLuint VAO_MUI_words[4];

GLuint VBO_MUI_background;
GLuint VBO_MUI_ui[5];
GLuint VBO_MUI_words[4];

GLuint EBO_MUI_background;
GLuint EBO_MUI_ui[5];
GLuint EBO_MUI_words[4];

std::map<std::string, unsigned int> multi_textures = { };

class Multi
{
public:
	Shader multiShader;

	void Init()
	{
		multiShader.setShader("shader/vertex_title.glsl", "shader/fragment_title.glsl"); // title 셰이더 재활용

		//background
		glGenVertexArrays(1, &VAO_MUI_background);
		glGenBuffers(1, &VBO_MUI_background);
		glGenBuffers(1, &EBO_MUI_background);

		glBindVertexArray(VAO_MUI_background);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_MUI_background);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_background), MUI_background, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MUI_background);
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

		//ui
		for (int i = 0; i < 5; ++i)
		{
			//button
			glGenVertexArrays(1, &VAO_MUI_ui[i]);
			glGenBuffers(1, &VBO_MUI_ui[i]);
			glGenBuffers(1, &EBO_MUI_ui[i]);

			glBindVertexArray(VAO_MUI_ui[i]);

			glBindBuffer(GL_ARRAY_BUFFER, VBO_MUI_ui[i]);

			switch (i) // UI에 대한 버퍼 데이터 설정
			{
			case 0:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_enter_button), MUI_enter_button, GL_STATIC_DRAW);
				break;
			case 1:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_quit_button), MUI_quit_button, GL_STATIC_DRAW);
				break;
			case 2:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_IP_input_window), MUI_IP_input_window, GL_STATIC_DRAW);
				break;
			case 3:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_enter_button_activate), MUI_enter_button_activate, GL_STATIC_DRAW);
				break;
			case 4:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_quit_button_activate), MUI_quit_button_activate, GL_STATIC_DRAW);
				break;
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MUI_ui[i]);
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

			if (i > 3) continue;

			//words
			glGenVertexArrays(1, &VAO_MUI_words[i]);
			glGenBuffers(1, &VBO_MUI_words[i]);
			glGenBuffers(1, &EBO_MUI_words[i]);

			glBindVertexArray(VAO_MUI_words[i]);

			glBindBuffer(GL_ARRAY_BUFFER, VBO_MUI_words[i]);

			switch (i)
			{
			case 0:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_enter_arrow), MUI_enter_arrow, GL_STATIC_DRAW);
				break;
			case 1:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_quit_words), MUI_quit_words, GL_STATIC_DRAW);
				break;
			case 2:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_enter_ip_words), MUI_enter_ip_words, GL_STATIC_DRAW);
				break;
			case 3:
				glBufferData(GL_ARRAY_BUFFER, sizeof(MUI_status_words), MUI_status_words, GL_STATIC_DRAW);
				break;
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MUI_words[i]);
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

		// 입력 글자 & 커서 버퍼
		glGenVertexArrays(1, &IPtextVAO);
		glGenBuffers(1, &IPtextVBO);
		glBindVertexArray(IPtextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, IPtextVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(IpTextVertex), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(IpTextVertex),
			reinterpret_cast<const void*>(offsetof(IpTextVertex, u)));
		glEnableVertexAttribArray(2);
	}

	void render_multi(int width, int height)
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

		RenderIpText(width, height);

		if (message == "Connecting..." || message == "Connection Failed")
		{
			int frame = message == "Connecting..." ? AnimationFrame() : 3;
			UseTexture_multi("status");
			glBindVertexArray(VAO_MUI_words[3]);
			glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, frame * 4);
		}
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

	void EnterTexture_multi()
	{
		multi_textures.clear();
		// multi tex
		InsertTexture_multi("background", "resource/texture/title/background.png"); // title background 재활용
		// ui tex
		InsertTexture_multi("button", "resource/texture/ui/multiplay_ui.png");
		InsertTexture_multi("ip_font", "resource/texture/ui/ip_font.png");
		InsertTexture_multi("status", "resource/texture/ui/multiplay_status.png");
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

	std::string ip;
	std::string message; // 화면에 표시할 메시지 (Connecting..., Connection failed, Disconnected)
	int select_menu = 0;
	bool is_open = false;

	// main에서 요청을 처리한 뒤 false로 되돌림
	bool request_connect = false;
	bool go_to_title = false;

	// 화면을 연다. 처음 들어올 때와 접속 실패 · 끊김으로 돌아올 때 모두 사용
	// 마지막으로 입력한 IP는 그대로 두고 표시할 메시지만 바꾼다.
	// 깊이 테스트 · 컬링 · 커서 같은 GL 상태는 바꾸지 않는다.
	// 게임 화면에서 돌아올 때는 부르는 쪽(IdleFunction의 끊김 처리)이 먼저 되돌린다.
	void Open(const std::string& msg)
	{
		is_open = true;
		message = msg;

		// 남아 있던 요청을 지워 같은 요청으로 다시 접속하거나 타이틀로 가지 않게 한다.
		request_connect = false;
		go_to_title = false;
		select_menu = 0;

		// 화면을 열자마자 입력 커서가 보이도록 깜빡임을 처음부터 시작한다.
		cursorBlinkStart = std::chrono::steady_clock::now();
	}

	int AnimationFrame() const
	{
		const auto elapsed = std::chrono::steady_clock::now() - cursorBlinkStart;
		if (message == "Connecting...")
			return static_cast<int>((elapsed / std::chrono::milliseconds(400)) % 3);
		return static_cast<int>((elapsed / std::chrono::milliseconds(500)) % 2);
	}

	void Input_keyboard(unsigned char key)
	{
		const bool connecting = message == "Connecting...";
		switch (key)
		{
		case 8: // BACKSPACE
			if (!connecting && !ip.empty())
				ip.pop_back();
			break;

		case 13: // ENTER
			if (select_menu == -1)
				go_to_title = true;
			else if (!ip.empty() && !connecting)
				request_connect = true;
			break;

		case 27: // ESCAPE
			go_to_title = true;
			break;

		default:
			// IPv4 주소: 숫자와 점만 입력, 최대 15글자
			if (!connecting && ip.size() < maxIpLength && ((key >= '0' && key <= '9') || key == '.'))
				ip += key;
			break;
		}
	}

	// 방향키 입력: SpecialKeyboard()에서 호출
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

	void Input_mouse(int button, int state, float x, float y)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			// 클릭한 위치에 맞춰 메뉴 선택 갱신
			Input_motion(x, y);

			// enter button
			if (select_menu == 1 && !ip.empty() && message != "Connecting...")
				request_connect = true;

			// quit button
			else if (select_menu == -1) go_to_title = true;
		}
	}

	void Input_motion(float x, float y)
	{
		// enter button
		if (0.4095f < x && x < 0.4995f && -0.06f < y && y < 0.10f) select_menu = 1;

		// quit button
		else if (-0.21375f < x && x < 0.21375f && -0.48f < y && y < -0.32f) select_menu = -1;

		else select_menu = 0;
	}

private:
	struct IpTextVertex
	{
		float x, y;
		float u, v;
	};

	static constexpr std::size_t maxIpLength = 15;
	std::chrono::steady_clock::time_point cursorBlinkStart;
	GLuint IPtextVAO = 0;
	GLuint IPtextVBO = 0;

	void RenderIpText(int width, int height)
	{
		// 0.5초마다 커서를 깜빡임
		const bool showCursor = message != "Connecting..." && AnimationFrame() == 0;
		if ((ip.empty() && !showCursor) || width <= 0 || height <= 0)
			return;

		const float atlasWidth = 200.0f;
		const float atlasHeight = 150.0f;
		const float cellSize = 50.0f;
		const float glyphHeight = 32.0f;
		const float atlasBlockSize = 4.0f;

		// Enter IP 높이에 맞추고, 글꼴 블록마다 같은 수의 화면 픽셀을 사용
		const float screenPixelWidth = 2.0f / width;
		const float screenPixelHeight = 2.0f / height;
		const float labelHeight = (MUI_enter_ip_words[1] - MUI_enter_ip_words[9]) / screenPixelHeight;
		const float blockSize = std::max(1.0f, std::round(labelHeight / (glyphHeight / atlasBlockSize)));
		const float pixelWidth = (blockSize / atlasBlockSize) * screenPixelWidth;
		const float pixelHeight = (blockSize / atlasBlockSize) * screenPixelHeight;
		const float centerY = (MUI_IP_input_window[1] + MUI_IP_input_window[9]) * 0.5f;
		const float textTop = centerY + glyphHeight * pixelHeight * 0.5f;
		const float top = std::round((textTop + 1.0f) / screenPixelHeight) * screenPixelHeight - 1.0f;
		float left = std::round((MUI_IP_input_window[16] + 0.02f + 1.0f) / screenPixelWidth) * screenPixelWidth - 1.0f;

		std::array<IpTextVertex, (maxIpLength + 1) * 6> vertices;
		int vertexCount = 0;
		const std::size_t textLength = std::min(ip.size(), maxIpLength);
		// 커서가 보이면 문자열 끝에 한 칸 더 그림

		const std::size_t glyphCount = textLength + static_cast<std::size_t>(showCursor);
		for (std::size_t i = 0; i < glyphCount; ++i)
		{
			const bool isCursor = i == textLength;
			const char character = isCursor ? '_' : ip[i];
			const bool isDot = character == '.';
			const int cell = isCursor ? 11 : (isDot ? 10 : character - '0');
			const float glyphWidth = isDot ? 8.0f : 24.0f;
			const float sourceHeight = isCursor ? glyphHeight + atlasBlockSize : glyphHeight;
			const float atlasX = (cell % 4) * cellSize + 15.0f;
			const float atlasY = (cell / 4) * cellSize + 9.0f;
			const float uLeft = atlasX / atlasWidth;
			const float uRight = (atlasX + glyphWidth) / atlasWidth;
			const float vTop = 1.0f - atlasY / atlasHeight;
			const float vBottom = 1.0f - (atlasY + sourceHeight) / atlasHeight;
			const float right = left + glyphWidth * pixelWidth;
			const float bottom = top - sourceHeight * pixelHeight;

			vertices[vertexCount++] = { right, top, uRight, vTop };
			vertices[vertexCount++] = { right, bottom, uRight, vBottom };
			vertices[vertexCount++] = { left, top, uLeft, vTop };
			vertices[vertexCount++] = { right, bottom, uRight, vBottom };
			vertices[vertexCount++] = { left, bottom, uLeft, vBottom };
			vertices[vertexCount++] = { left, top, uLeft, vTop };
			left = right;
		}

		UseTexture_multi("ip_font");
		glBindVertexArray(IPtextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, IPtextVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(IpTextVertex), vertices.data(), GL_STREAM_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
};


#endif
