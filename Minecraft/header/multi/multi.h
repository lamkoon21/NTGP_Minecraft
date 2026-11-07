#pragma once

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../MultiplayUI/multiplayUI.h"

extern MultiplayUI multiplayUI;

class Multi
{
public:
    std::string message;

    void render_multi();
};

inline void Multi::render_multi()
{
    // 기존 OpenGL 상태 저장
    GLint previousProgram = 0;
    GLint previousVAO = 0;

    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // 기존 배경, 입력 칸, 버튼, 안내 문구 출력
    multiplayUI.render_multiplayUI();

    // 비트맵 문자 출력 준비
    glUseProgram(0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // UI 좌표(-1 ~ 1)를 화면 픽셀 좌표로 변환
    auto drawText = [&](float x, float y,
        const std::string& text)
        {
            if (text.empty())
                return;

            const float pixelX =
                viewport[0] + (x + 1.0f) * 0.5f * viewport[2];

            const float pixelY =
                viewport[1] + (y + 1.0f) * 0.5f * viewport[3];

            glWindowPos2f(pixelX, pixelY);

            glutBitmapString(
                GLUT_BITMAP_HELVETICA_18,
                reinterpret_cast<const unsigned char*>(text.c_str())
            );
        };

    // 입력 칸 안에 IP 출력
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.47f, 0.0f, multiplayUI.ip);

    // 입력 칸 아래에 상태 메시지 출력
    glColor3f(1.0f, 0.85f, 0.35f);
    drawText(-0.47f, -0.20f, message);

    // 기존 OpenGL 상태 복원
    glPopAttrib();
    glBindVertexArray(static_cast<GLuint>(previousVAO));
    glUseProgram(static_cast<GLuint>(previousProgram));
}