#pragma once
#ifndef MULTIPLAYUI_H
#define MULTIPLAYUI_H

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/ext.hpp"
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

std::map<std::string, unsigned int> multiplayUI_textures = { };

class MultiplayUI
{
public:
	Shader MultiplayUIShader;

	// 멀티 화면에 쓸 사각형 VAO를 모두 만든다. InitBuffer()에서 한 번 부른다.
	void Init()
	{
		// 두 번째 호출부터는 셰이더와 GL 객체를 다시 만들지 않는다 (누수 방지)
		if (initialized) return;
		initialized = true;

		MultiplayUIShader.setShader("shader/vertex_title.glsl", "shader/fragment_title.glsl"); // title 셰이더 재활용

		// 배경
		CreateQuad(MUI_background, sizeof(MUI_background), VAO_MUI_background, VBO_MUI_background, EBO_MUI_background);

		// 버튼과 입력 칸 : render_multiplayUI()가 select_menu에 따라 번호로 골라 그린다
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

	void render_multiplayUI()
	{
		MultiplayUIShader.use();

		UseTexture_multiplayUI("background");
		glBindVertexArray(VAO_MUI_background);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		UseTexture_multiplayUI("button");
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


	}

	void UseTexture_multiplayUI(const char* key)
	{
		if (multiplayUI_textures.find(key) != multiplayUI_textures.end()) {
			glBindTexture(GL_TEXTURE_2D, multiplayUI_textures[key]);
		}
		else {
			std::cerr << "ERROR! Can't use undefined key\n";
		}
	}

	void EnterTexture_multiplayUI()
	{
		multiplayUI_textures.clear();
		// multiplayUI tex
		InsertTexture_multiplayUI("background", "resource/texture/title/background.png"); // title background 재활용
		// ui tex
		InsertTexture_multiplayUI("button", "resource/texture/ui/multiplay_ui.png");
	}

	void SetTexture_multiplayUI(const char* filename, unsigned int& texture)
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

	void InsertTexture_multiplayUI(const char* key, const char* filename)
	{
		if (multiplayUI_textures.find(key) != multiplayUI_textures.end()) {
			std::cerr << "ERROR! Duplicate texture key can\'t be used\n";
		}
		else {
			unsigned int texture;
			SetTexture_multiplayUI(filename, texture);
			multiplayUI_textures.insert({ key, texture });
		}
	}

	std::string ip;
	int select_menu = 0;

	// main에서 요청을 처리한 뒤 false로 되돌림
	bool request_connect = false;
	bool go_to_title = false;

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
			// IPv4 주소: 숫자와 점만 입력, 최대 15글자
			if ((key >= '0' && key <= '9') || key == '.')
			{
				if (ip.size() < 15)
					ip += key;

				select_menu = 1;
			}
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
			if (select_menu == 1 && !ip.empty()) request_connect = true;

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

};


#endif