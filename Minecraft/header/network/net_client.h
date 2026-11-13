#pragma once

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include <cstdint>
#include <vector>

class Character;

// 원격 플레이어 목록 반환
const std::vector<Character*>& Remotes();

// 내 플레이어의 위치, 시선, 든 블록 정보를 서버로 전송
void SendMove(const Character* player);

DWORD WINAPI RecvThread(LPVOID arg);
bool ConnectServer(const char* ipv4, uint8_t& outPlayerId, int& outSeed, HANDLE cancelEvent = NULL);
bool FinishServerJoin(bool acceptJoin);
