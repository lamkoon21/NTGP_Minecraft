#ifndef WORLD_H
#define WORLD_H


#include "glm/glm.hpp"
#include "../block/blocktype.h"
#include <map>
#include <vector>
#include "../PerlinNoise-master/PerlinNoise.hpp"
#include "../skybox/skybox.h"
#include <tuple>

class Chunk;
class Character;


const int ViewDistance = 5;
const int WORLD_MAX = 20;


class World
{
private:
	int seed;
	glm::vec3 prevPlayerPos = glm::vec3(0.0f);
	glm::vec3 currentPlayerPos = glm::vec3(0.0f);
	
	void CreateNewChunk(int x, int z);
	bool IsBlockInWorld(int world_x, int world_y, int world_z);
	// 청크 좌표 → (청크 내부 x, y, z → 최종 블록 종류)
	std::map<std::pair<int, int>, 
		std::map<std::tuple<int, int, int>, 
		BlockType>> pendingBlocks;
public:
	std::map<std::pair<int, int>, Chunk*> chunks;
	std::vector<Chunk*> activeChunks;
	siv::PerlinNoise perlin;
	Skybox skybox;
	std::pair<int, int> GetChunkPosFromWorldPos(glm::vec3 pos);
	std::pair<int, int> GetBlockPosFromWorldPos(glm::vec3 pos);

	World();
	World(int seed);


	void Init();
	void GenerateWorld();
	void Update();
	void SetPlayerPos(glm::vec3 pos);
	BlockType GetBlockType(int x, int y, int z);
	int GetTerrainHeight(int x, int z);
	glm::vec3 GetStartPos();
	int WorldPosIsSolid(glm::vec3 pos);
	// 월드 좌표의 블록 변경. 메인 스레드에서 호출한다.
	void SetBlock(int x, int y, int z, BlockType type);
};


#endif