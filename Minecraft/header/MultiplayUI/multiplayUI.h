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
	// 위치                 // 컬러             // 텍스처 좌표
	0.35f,  0.16f, 0.0f,   1.0f, 0.0f, 0.0f,   0.475f, 0.9f,   // 우측 상단
	0.35f, -0.05f, 0.0f,   0.0f, 1.0f, 0.0f,   0.475f, 0.8f,   // 우측 하단
   -0.49f, -0.05f, 0.0f,   0.0f, 0.0f, 1.0f,   0.000f, 0.8f,   // 좌측 하단
   -0.49f,  0.16f, 0.0f,   1.0f, 1.0f, 0.0f,   0.000f, 0.9f    // 좌측 상단
};

float MUI_enter_ip_words[] = {
	// 위치                   // 컬러             // 텍스처 좌표
	0.045f, 0.308f, 0.0f,   1.0f, 0.0f, 0.0f,   0.605f, 0.07f,   // 우측 상단
	0.045f, 0.230f, 0.0f,   0.0f, 1.0f, 0.0f,   0.605f, 0.03f,   // 우측 하단
   -0.185f, 0.230f, 0.0f,   0.0f, 0.0f, 1.0f,   0.395f, 0.03f,   // 좌측 하단
   -0.185f, 0.308f, 0.0f,   1.0f, 1.0f, 0.0f,   0.395f, 0.07f    // 좌측 상단
};

float MUI_enter_button[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.480f,  0.16f, 0.0f,  1.0f, 0.0f, 0.0f,   0.1f, 0.5f,   // 우측 상단
	0.480f, -0.05f, 0.0f,  0.0f, 1.0f, 0.0f,   0.1f, 0.4f,   // 우측 하단
	0.365f, -0.05f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.4f,   // 좌측 하단
	0.365f,  0.16f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.5f    // 좌측 상단
};

float MUI_enter_button_activate[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.480f,  0.16f, 0.0f,  1.0f, 0.0f, 0.0f,   0.1f, 0.4f,   // 우측 상단
	0.480f, -0.05f, 0.0f,  0.0f, 1.0f, 0.0f,   0.1f, 0.3f,   // 우측 하단
	0.365f, -0.05f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.3f,   // 좌측 하단
	0.365f,  0.16f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.4f    // 좌측 상단
};

float MUI_quit_button[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.205f, -0.30f, 0.0f,  1.0f, 0.0f, 0.0f,   0.475f, 0.8f,   // 우측 상단
	0.205f, -0.50f, 0.0f,  0.0f, 1.0f, 0.0f,   0.475f, 0.7f,   // 우측 하단
   -0.205f, -0.50f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.7f,   // 좌측 하단
   -0.205f, -0.30f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.8f    // 좌측 상단
};

float MUI_quit_button_activate[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.205f, -0.30f, 0.0f,  1.0f, 0.0f, 0.0f,   0.475f, 0.7f,   // 우측 상단
	0.205f, -0.50f, 0.0f,  0.0f, 1.0f, 0.0f,   0.475f, 0.6f,   // 우측 하단
   -0.205f, -0.50f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.6f,   // 좌측 하단
   -0.205f, -0.30f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.7f    // 좌측 상단
};

float MUI_enter_arrow[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.450f, 0.104f, 0.0f,  1.0f, 0.0f, 0.0f,   0.525f, 0.280f,   // 우측 상단
	0.450f, 0.006f, 0.0f,  0.0f, 1.0f, 0.0f,   0.525f, 0.225f,   // 우측 하단
	0.395f, 0.006f, 0.0f,  0.0f, 0.0f, 1.0f,   0.470f, 0.225f,   // 좌측 하단
	0.395f, 0.104f, 0.0f,  1.0f, 1.0f, 0.0f,   0.470f, 0.280f    // 좌측 상단
};

float MUI_quit_words[] = {
	// 위치                 // 컬러             // 텍스처 좌표
	0.16f, -0.361f, 0.0f,  1.0f, 0.0f, 0.0f,   0.645f, 0.17f,   // 우측 상단
	0.16f, -0.439f, 0.0f,  0.0f, 1.0f, 0.0f,   0.645f, 0.13f,   // 우측 하단
   -0.16f, -0.439f, 0.0f,  0.0f, 0.0f, 1.0f,   0.355f, 0.13f,   // 좌측 하단
   -0.16f, -0.361f, 0.0f,  1.0f, 1.0f, 0.0f,   0.355f, 0.17f    // 좌측 상단
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

	void Init()
	{
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

			// 글자는 버퍼가 2개만 필요하므로 2번만 반복
			if (i > 2)continue; 

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


	}

	void render_multiplayUI()
	{
		MultiplayUIShader.setShader("shader/vertex_title.glsl", "shader/fragment_title.glsl"); // title 셰이더 재활용
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
		if (0.365f < x && x < 0.480f && -0.05f < y && y < 0.16f) select_menu = 1;
		// quit button
		else if (-0.205f < x && x < 0.205f && -0.50f < y && y < -0.30f) select_menu = -1;

		else select_menu = 0;
	}

};


#endif