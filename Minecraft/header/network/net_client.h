#pragma once

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include <vector>

class Character;

// 원격 플레이어 목록 반환
const std::vector<Character*>& Remotes();

DWORD WINAPI RecvThread(LPVOID arg);