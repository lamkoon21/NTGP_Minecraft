#include "character.h"
#include "../transform/tranform.h"
#include "../world/world.h"
#include "../chunk/chunk.h"


void Character::PosUpdate()
{
    float ds = glm::sin(glm::radians(BodyYaw));
    float dc = glm::cos(glm::radians(BodyYaw));

    HeadPos = glm::vec3(Position.x, Position.y + 1.35f, Position.z);
    BodyPos = glm::vec3(Position.x, Position.y + 0.675f, Position.z);
    LeftArmPos = glm::vec3(Position.x - 0.33751f * ds, Position.y + 1.35f, Position.z - 0.33751f * dc);
    RightArmPos = glm::vec3(Position.x + 0.33751f * ds, Position.y + 1.35f, Position.z + 0.33751f * dc);
    LeftLegPos = glm::vec3(Position.x - 0.11251f * ds, Position.y + 0.675f, Position.z - 0.11251f * dc);
    RightLegPos = glm::vec3(Position.x + 0.11251f * ds, Position.y + 0.675f, Position.z + 0.11251f * dc);
    camera.Position = glm::vec3(Position.x + 0.22501f * glm::cos(glm::radians(HeadYaw)), Position.y + 1.575f, Position.z - 0.22501f * glm::sin(glm::radians(HeadYaw)));
}

Character::Character() 
{ 
    Position = glm::vec3(0.0f);
    PosUpdate();
}

Character::Character(World* world)
{
    this->world = world;
    Position = world->GetStartPos();
    PosUpdate();
}

Character::Character(World* world, glm::vec3 position)
{
    this->world = world;
    Position = position;
    PosUpdate();
}

void Character::Init()
{
    characterReader[0].initBuffer(VAO_head);
    characterReader[1].initBuffer(VAO_body);
    characterReader[2].initBuffer(VAO_left_arm);
    characterReader[3].initBuffer(VAO_right_arm);
    characterReader[4].initBuffer(VAO_left_leg);
    characterReader[5].initBuffer(VAO_right_leg);

    glGenVertexArrays(1, &VAO_block);
    glGenBuffers(1, &VBO);
}

void Character::RenderCollideBox(Shader* shader)
{
    SRT(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f), shader);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(Position.x - box_interval, Position.y + 0.01f, Position.z - box_interval);
    glVertex3f(Position.x - box_interval, Position.y + 0.01f, Position.z + box_interval);
    glVertex3f(Position.x + box_interval, Position.y + 0.01f, Position.z + box_interval);
    glVertex3f(Position.x + box_interval, Position.y + 0.01f, Position.z - box_interval);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(Position.x, Position.y + 0.01f, Position.z);
    glVertex3f(Position.x + 0.5f, Position.y + 0.01f, Position.z);
    glEnd();

    int x = static_cast<int>(Position.x);
    int y = static_cast<int>(Position.y);
    int z = static_cast<int>(Position.z);
    glLineWidth(6.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x, y + 0.01f, z);
    glVertex3f(x + 1, y + 0.01f, z);
    glVertex3f(x + 1, y + 0.01f, z + 1);
    glVertex3f(x, y + 0.01f, z + 1);
    glEnd();
}

float Character::GetSwingProgress() const
{
    return hand_swing_ticks > 0 ? 1.0f - float(hand_swing_ticks) / HAND_SWING_TICKS : 0.0f;
}

glm::mat4 Character::GetRightArmTransform() const
{
    const float t = GetSwingProgress();
    const float stroke = glm::sin(glm::pi<float>() * glm::sqrt(t));
    // Lead outward while lifting, cross inward on the return stroke.
    const float sweep = glm::sin(2.0f * glm::pi<float>() * glm::sqrt(t));
    const float sideways = sweep * (sweep >= 0.0f ? 30.0f : 20.0f);
    const float resting = blocks[block_kind] == BlockType::AIR ? -shake_deg : 18.0f - shake_deg * 0.5f;
    // Yaw remains independent of aim pitch; only the arm elevation follows aim.
    const float elevation = resting * (1.0f - stroke) + (90.0f + HeadPitch) * stroke;
    const float yaw = glm::radians(BodyYaw);
    const glm::vec3 right(glm::sin(yaw), 0.0f, glm::cos(yaw));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), RightArmPos - right * 0.1125f);
    model = glm::rotate(model, yaw, glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(-sideways), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(elevation), glm::vec3(0, 0, 1));
    return glm::translate(model, glm::vec3(0, 0, 0.1125f));
}

glm::mat4 Character::GetFirstPersonTransform(bool block) const
{
    const float t = GetSwingProgress();
    const float stroke = glm::sin(glm::pi<float>() * glm::sqrt(t));
    const float dip = glm::sin(glm::pi<float>() * t);
    // Camera-local coordinates: looking around cannot move the view model.
    glm::vec3 pos = block ? glm::vec3(0.70f, -0.77f, -1.05f) : glm::vec3(0.52f, -0.40f, -0.85f);
    pos += glm::vec3(-0.50f * stroke, -0.30f * dip, -0.12f * stroke);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    // Camera-space attack only: turn the right hand inward from its resting pose.
    // The quick inward stroke and slower recovery follow the reference footage.
    // Positive roll brings the fingertips toward screen-left, not outward.
    model = glm::rotate(model, glm::radians(55.0f * stroke), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(35.0f * stroke), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(-35.0f * stroke), glm::vec3(1, 0, 0));
    if (block) {
        model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.6f));
        return glm::translate(model, glm::vec3(-0.5f));
    }
    model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(50.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0, 1, 0));
    // Preserve the tuned screen-space hand despite the smaller world mesh.
    model = glm::scale(model, glm::vec3(0.75f / 0.9f));
    return glm::translate(model, glm::vec3(0, 0.648f, 0.135f));
}

void Character::RenderFirstPersonArm(Shader* shader)
{
    if (blocks[block_kind] != BlockType::AIR) return;
    shader->setMat4("model", GetFirstPersonTransform(false));
    glBindVertexArray(VAO_right_arm);
    glDrawArrays(GL_TRIANGLES, 0, RightArmObj);
}

void Character::Render(Shader* shader)
{
    SRT(glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(HeadYaw), glm::radians(HeadPitch)), HeadPos, shader);
    glBindVertexArray(VAO_head);
    glDrawArrays(GL_TRIANGLES, 0, HeadObj);

    SRT(glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(BodyYaw), 0.0f), BodyPos, shader);
    glBindVertexArray(VAO_body);
    glDrawArrays(GL_TRIANGLES, 0, BodyObj);

    {
        SRT(glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(BodyYaw), glm::radians(shake_deg)), LeftArmPos, shader);
        glBindVertexArray(VAO_left_arm);
        glDrawArrays(GL_TRIANGLES, 0, LeftArmObj);

        shader->setMat4("model", GetRightArmTransform());
        glBindVertexArray(VAO_right_arm);
        glDrawArrays(GL_TRIANGLES, 0, RightArmObj);
    }
    SRT(glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(BodyYaw), -glm::radians(shake_deg)), LeftLegPos, shader);
    glBindVertexArray(VAO_left_leg);
    glDrawArrays(GL_TRIANGLES, 0, LeftLegObj);

    SRT(glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(BodyYaw), glm::radians(shake_deg)), RightLegPos, shader);
    glBindVertexArray(VAO_right_leg);
    glDrawArrays(GL_TRIANGLES, 0, RightLegObj);

    if (!god_mode)
        RenderUI(shader);
}

void Character::SetBlockVertices()
{
    block_vertices.clear();

    for (int f = 0; f < 6; f++) // Back -Z, Front +Z, Top Face +Y, Bottom -Y, Left -X, RIght +X
    {
        for (int i = 0; i < 6; i++) {
            block_vertices.push_back(voxelVertex[voxelTris[f][i]].x);
            block_vertices.push_back(voxelVertex[voxelTris[f][i]].y);
            block_vertices.push_back(voxelVertex[voxelTris[f][i]].z);

            block_vertices.push_back(faceChecks[f].x);
            block_vertices.push_back(faceChecks[f].y);
            block_vertices.push_back(faceChecks[f].z);

            block_vertices.push_back(GetTextureUV(TextureIndex[blocks[block_kind]][f]).first + voxelUvs[i][0]);
            block_vertices.push_back(GetTextureUV(TextureIndex[blocks[block_kind]][f]).second + voxelUvs[i][1]);
        }
    }
}

void Character::RenderBlock(Shader* shader, bool firstPerson)
{
    if (block_vertices.size() == 0 || blocks[block_kind] == BlockType::AIR)
        return;

    glBindVertexArray(VAO_block);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBufferData(GL_ARRAY_BUFFER, block_vertices.size() * sizeof(float), block_vertices.data(), GL_STATIC_DRAW);

    glm::mat4 model;
    if (firstPerson) {
        model = GetFirstPersonTransform(true);
    }
    else {
        // Constant grip in arm space, attached to the end of the hand.
        glm::mat4 grip = glm::rotate(glm::mat4(1), glm::radians(-18.0f), glm::vec3(0, 0, 1));
        grip = glm::rotate(grip, glm::radians(30.0f), glm::vec3(0, 1, 0));
        model = glm::translate(GetRightArmTransform(), glm::vec3(0.18f, -0.657f, 0)) * grip;
        model = glm::scale(model, glm::vec3(0.36f));
        model = glm::translate(model, glm::vec3(-0.5f));
    }
    shader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //glDeleteVertexArrays(1, &VBO);
    //glDeleteVertexArrays(1, &VAO_block);
}

void Character::RenderUI(Shader* shader)
{
    SRT(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f), shader);

    if (selectblock) {
        float itv = 0.001f;
        int x, y, z; 
        (selectBlockPos.x >= 0) ? x = static_cast<int>(selectBlockPos.x) : x = static_cast<int>(selectBlockPos.x + 0.0001f) - 1;
        y = static_cast<int>(selectBlockPos.y);
        (selectBlockPos.z >= 0) ? z = static_cast<int>(selectBlockPos.z) : z = static_cast<int>(selectBlockPos.z + 0.0001f) - 1;

        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(x - itv, y - itv, z - itv);
        glVertex3f(x + 1.0f + itv, y - itv, z - itv);
        glVertex3f(x + 1.0f + itv, y - itv, z + 1.0f + itv);
        glVertex3f(x - itv, y - itv, z + 1.0f + itv);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(x - itv, y + 1.0f + itv, z - itv);
        glVertex3f(x + 1.0f + itv, y + 1.0f + itv, z - itv);
        glVertex3f(x + 1.0f + itv, y + 1.0f + itv, z + 1.0f + itv);
        glVertex3f(x - itv, y + 1.0f + itv, z + 1.0f + itv);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(x - itv, y - itv, z - itv);
        glVertex3f(x - itv, y + 1.0f + itv, z - itv);
        glVertex3f(x + 1.0f + itv, y - itv, z - itv);
        glVertex3f(x + 1.0f + itv, y + 1.0f + itv, z - itv);
        glVertex3f(x + 1.0f + itv, y - itv, z + 1.0f + itv);
        glVertex3f(x + 1.0f + itv, y + 1.0f + itv, z + 1.0f + itv);
        glVertex3f(x - itv, y - itv, z + 1.0f + itv);
        glVertex3f(x - itv, y + 1.0f + itv, z + 1.0f + itv);
        glEnd();
    }
    
}

void Character::Input()
{
    if (!god_mode) {
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) // shift
            move_state = 2;
        else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) // ctrl
            move_state = 1;
        else
            move_state = 0;

        if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && jump) { // space
            speedY = 0.16f;
            jump = false;
        }
    }
    else {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) // ctrl
            Position.y -= 0.3f;

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) // space
            Position.y += 0.3f;
    }

    dir_front = 0;
    dir_left = 0;
    if (GetAsyncKeyState(0x57) & 0x8000) // up
        dir_front += 1;
    if (GetAsyncKeyState(0x53) & 0x8000) // down
        dir_front -= 1;
    if (GetAsyncKeyState(0x41) & 0x8000) // left
        dir_left += 1;
    if (GetAsyncKeyState(0x44) & 0x8000) // right
        dir_left -= 1;
    
    /*
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) // space
        Position.y -= 0.1f;
    */
}

void Character::KeyboardInput(unsigned char key)
{
    if (key >= '1' && key <= '9') {
        if (block_kind != key - '1' && key - '1' != 0) {
            block_kind = key - '1';
            SetBlockVertices();
        }
        else
            block_kind = key - '1';
    }
}

void Character::SpecialKeyboardInput(unsigned char key)
{
    if (key == GLUT_KEY_F5) {
        camera_mode = (camera_mode + 1) % 3;
    }

    if (key == GLUT_KEY_F1) {
        god_mode = !god_mode;
    }
}

void Character::MouseInput(bool left_state, bool right_state)
{
    // Every fresh left click swings, even in mid-swing or when aiming at air.
    if (left_state && !mouse_left_state)
        hand_swing_ticks = HAND_SWING_TICKS;

    mouse_left_state = left_state;
    mouse_right_state = right_state;

    if (!mouse_right_state)
        add_block_time = 0.0f;
    if (!mouse_left_state)
        del_block_time = 0.0f;
}

void Character::MouseWheelInput(int dir)
{
    if (dir > 0)
    {
        if (block_kind > 0) {
            block_kind -= 1;
            SetBlockVertices();
        }
        else
            block_kind = 8;
    }
    else
    {
        if (block_kind < 8) {
            block_kind += 1;
            SetBlockVertices();
        }
        else
            block_kind = 0;
    }
}

void Character::Update()
{
    if (hand_swing_ticks > 0)
        --hand_swing_ticks;
    // Holding left click repeats complete swings regardless of raycast results.
    if (mouse_left_state && hand_swing_ticks == 0)
        hand_swing_ticks = HAND_SWING_TICKS;

    // set angle
    HeadPitch = camera.Pitch;
    HeadYaw = -camera.Yaw;
    BodyYaw = HeadYaw;

    // set shake_deg
    if (dir_front == 0 && dir_left == 0) {
        shake_deg /= 1.1f;
        if (glm::abs(shake_deg) < 0.1f)
            shake_deg = 0.0f;
    }
    else {
        if (shake_state) {
            shake_deg += ADD_SHAKE_DEG[move_state];
            if (shake_deg > MAX_SHAKE_DEG[move_state]) {
                shake_deg = MAX_SHAKE_DEG[move_state];
                shake_state = false;
            }
        }
        else {
            shake_deg -= ADD_SHAKE_DEG[move_state];
            if (shake_deg < -MAX_SHAKE_DEG[move_state]) {
                shake_deg = -MAX_SHAKE_DEG[move_state];
                shake_state = true;
            }
        }
    }

    // set Pos
    float speed = 0.0f;
    if (dir_front != 0 && dir_left != 0)
        speed = CHARACTER_SPEED[move_state] / 1.4f;
    else if ((dir_front == 0) ^ (dir_left == 0))
        speed = CHARACTER_SPEED[move_state];

    if (god_mode)
        speed *= 5.0f;

    speedX = (static_cast<float>(dir_front) * glm::cos(glm::radians(HeadYaw)) + static_cast<float>(dir_left) * -glm::sin(glm::radians(HeadYaw))) * speed;
    speedZ = (static_cast<float>(dir_front) * -glm::sin(glm::radians(HeadYaw)) + static_cast<float>(dir_left) * -glm::cos(glm::radians(HeadYaw))) * speed;
    if (god_mode) {
        Position.x += speedX;
        Position.z += speedZ;
    }

    add_block_time -= 1.0f;
    del_block_time -= 1.0f;

    if (!god_mode)
        PhysicsUpdate();
    else
        speedY = 0.0f;
    BlockUpdate();
    PosUpdate();
}

void Character::PhysicsUpdate()
{
    speedY -= GRAVITY_ACC * 0.001f;
    if (speedY < -MAX_SPEED)
        speedY = -MAX_SPEED;

    jump = false;
    const float height = CHARACTER_HEIGHT;
    const float itv = 0.001f;

    // Resolve one axis at a time, so a wall cannot be mistaken for the floor.
    auto moveAxis = [&](int axis, float distance) {
        const int steps = static_cast<int>(glm::ceil(glm::abs(distance) / 0.25f));
        if (steps == 0) return;
        const float step = distance / steps;
        for (int i = 0; i < steps; ++i) {
            Position[axis] += step;
            glm::vec3 lower = Position - glm::vec3(box_interval, 0.0f, box_interval);
            glm::vec3 upper = Position + glm::vec3(box_interval, height, box_interval);
            glm::ivec3 first(glm::floor(lower + glm::vec3(itv)));
            glm::ivec3 last(glm::floor(upper - glm::vec3(itv)));
            bool collided = false;
            float corrected = Position[axis];
            for (int x = first.x; x <= last.x; ++x)
                for (int y = first.y; y <= last.y; ++y)
                    for (int z = first.z; z <= last.z; ++z) {
                        // Query the cell center to avoid ambiguous negative boundaries.
                        if (world->WorldPosIsSolid(glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f)) != 2)
                            continue;
                        const int cell = (axis == 0) ? x : ((axis == 1) ? y : z);
                        const float extent = (axis == 1) ? height : box_interval;
                        if (step > 0.0f)
                            corrected = glm::min(corrected, cell - extent);
                        else
                            corrected = glm::max(corrected, cell + 1.0f + ((axis == 1) ? 0.0f : box_interval));
                        collided = true;
                    }
            if (collided) {
                Position[axis] = corrected;
                if (axis == 1) {
                    jump = step < 0.0f;
                    speedY = 0.0f;
                }
                break;
            }
        }
    };

    moveAxis(0, speedX);
    moveAxis(2, speedZ);
    moveAxis(1, speedY);
}

void Character::BlockUpdate()
{
    selectblock = false;
    glm::vec3 headPos = camera.Position;
    glm::vec3 headDir = glm::normalize(camera.Front);
    glm::vec3 delta_headDir = headDir * 0.1f;

    for (int i = 1; i <= 50; i++)
    {
        if (world->WorldPosIsSolid(headPos + (delta_headDir * static_cast<float>(i))) == 2) {
            selectblock = true;
            selectBlockPos = headPos + (delta_headDir * static_cast<float>(i));
            glm::vec3 bp = headPos + (delta_headDir * static_cast<float>(i - 1));
            
            int sx, sy, sz;
            (selectBlockPos.x >= 0) ? sx = static_cast<int>(selectBlockPos.x) : sx = static_cast<int>(selectBlockPos.x + 0.0001f) - 1;
            (selectBlockPos.y >= 0) ? sy = static_cast<int>(selectBlockPos.y) : sy = static_cast<int>(selectBlockPos.y + 0.0001f) - 1;
            (selectBlockPos.z >= 0) ? sz = static_cast<int>(selectBlockPos.z) : sz = static_cast<int>(selectBlockPos.z + 0.0001f) - 1;

            int bx, by, bz;
            (bp.x >= 0) ? bx = static_cast<int>(bp.x) : bx = static_cast<int>(bp.x + 0.0001f) - 1;
            (bp.y >= 0) ? by = static_cast<int>(bp.y) : by = static_cast<int>(bp.y + 0.0001f) - 1;
            (bp.z >= 0) ? bz = static_cast<int>(bp.z) : bz = static_cast<int>(bp.z + 0.0001f) - 1;

            if (sx > bx) {
                nextBlockPos = selectBlockPos + glm::vec3(-1.0f, 0.0f, 0.0f);
            }
            else if (sx < bx) {
                nextBlockPos = selectBlockPos + glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else if (sy > by) {
                nextBlockPos = selectBlockPos + glm::vec3(0.0f, -1.0f, 0.0f);
            }
            else if (sy < by) {
                nextBlockPos = selectBlockPos + glm::vec3(0.0f, 1.0f, 0.0f);
            }
            else if (sz > bz) {
                nextBlockPos = selectBlockPos + glm::vec3(0.0f, 0.0f, -1.0f);
            }
            else if (sz < bz) {
                nextBlockPos = selectBlockPos + glm::vec3(0.0f, 0.0f, 1.0f);
            }

            break;
        }
    }

    if (mouse_left_state && selectblock && del_block_time <= 0.0f) {
        std::pair<int, int> chunkPos = world->GetChunkPosFromWorldPos(selectBlockPos);
        // find chunk
        if (world->chunks.find({ chunkPos.first, chunkPos.second }) != world->chunks.end()) {
            Chunk* chunk = world->chunks.find({ chunkPos.first, chunkPos.second })->second;
            std::pair<int, int> blockPos = world->GetBlockPosFromWorldPos(selectBlockPos);
            int by = static_cast<int>(selectBlockPos.y);
            chunk->blocks[blockPos.first][by][blockPos.second] = BlockType::AIR;
            chunk->ChangeMeshData();
            del_block_time = 15.0f;

            if (blockPos.first == 0 && world->chunks.find({ chunkPos.first - 1, chunkPos.second }) != world->chunks.end()) {
                //world->chunks.find({ chunkPos.first - 1, chunkPos.second })->second->blocks[15][by][blockPos.second] = BlockType::AIR;
                world->chunks.find({ chunkPos.first - 1, chunkPos.second })->second->ChangeMeshData();
            }
            if (blockPos.first == 15 && world->chunks.find({ chunkPos.first + 1, chunkPos.second }) != world->chunks.end()) {
                world->chunks.find({ chunkPos.first + 1, chunkPos.second })->second->ChangeMeshData();
            }
            if (blockPos.second == 0 && world->chunks.find({ chunkPos.first, chunkPos.second - 1 }) != world->chunks.end()) {
                world->chunks.find({ chunkPos.first, chunkPos.second - 1 })->second->ChangeMeshData();
            }
            if (blockPos.second == 15 && world->chunks.find({ chunkPos.first, chunkPos.second + 1 }) != world->chunks.end()) {
                world->chunks.find({ chunkPos.first, chunkPos.second + 1 })->second->ChangeMeshData();
            }
        }
    }

    if (mouse_right_state && selectblock && add_block_time <= 0.0f) {
        if (blocks[block_kind] != BlockType::AIR) {
            const glm::vec3 blockMin = glm::floor(nextBlockPos);
            const glm::vec3 blockMax = blockMin + glm::vec3(1.0f);
            const glm::vec3 playerMin = Position - glm::vec3(box_interval, 0.0f, box_interval);
            const glm::vec3 playerMax = Position + glm::vec3(box_interval, CHARACTER_HEIGHT, box_interval);
            // Use the same standing bounds as PhysicsUpdate. Touching a face
            // is allowed; any volume overlap (including feet/head) is not.
            const bool overlapsPlayer =
                blockMin.x < playerMax.x && blockMax.x > playerMin.x &&
                blockMin.y < playerMax.y && blockMax.y > playerMin.y &&
                blockMin.z < playerMax.z && blockMax.z > playerMin.z;
            if (overlapsPlayer || blockMin.y < 0.0f || blockMin.y >= ChunkHeight)
                return;
            const glm::vec3 blockCenter = blockMin + glm::vec3(0.5f);
            std::pair<int, int> chunkPos = world->GetChunkPosFromWorldPos(blockCenter);
            // find chunk
            if (world->chunks.find({ chunkPos.first, chunkPos.second }) != world->chunks.end()) {
                Chunk* chunk = world->chunks.find({ chunkPos.first, chunkPos.second })->second;
                std::pair<int, int> blockPos = world->GetBlockPosFromWorldPos(blockCenter);
                const int by = static_cast<int>(blockMin.y);
                if (chunk->blocks[blockPos.first][by][blockPos.second] != BlockType::AIR)
                    return;
                chunk->blocks[blockPos.first][by][blockPos.second] = blocks[block_kind];
                chunk->ChangeMeshData();
                add_block_time = 15.0f;
                if (hand_swing_ticks == 0)
                    hand_swing_ticks = HAND_SWING_TICKS;
            }
        }
    }
}

glm::mat4 Character::GetViewMatrix()
{
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    if (camera_mode == 0) {
        cameraPos = camera.Position;
        cameraFront = camera.Front;
    }
    if (camera_mode == 1) {
        cameraPos = camera.Position - 5.0f * glm::normalize(camera.Front);
        cameraFront = camera.Front;
    }
    if (camera_mode == 2) {
        cameraPos = camera.Position + 5.0f * glm::normalize(camera.Front);
        cameraFront = -camera.Front;
    }

    if (camera_mode != 0) {
        // Sweep a small camera box from the eyes to the desired position.
        // Stop at the first obstruction so even a thin wall cannot be crossed.
        const glm::vec3 direction = glm::normalize(cameraPos - camera.Position);
        constexpr float radius = 0.10f;
        constexpr float step = 0.05f;
        float safeDistance = 0.0f;
        for (int i = 1; i <= 100; ++i) {
            const float distance = i * step;
            const glm::vec3 probe = camera.Position + direction * distance;
            const glm::ivec3 first(glm::floor(probe - glm::vec3(radius)));
            const glm::ivec3 last(glm::floor(probe + glm::vec3(radius)));
            bool blocked = false;
            for (int x = first.x; x <= last.x && !blocked; ++x)
                for (int y = first.y; y <= last.y && !blocked; ++y)
                    for (int z = first.z; z <= last.z && !blocked; ++z)
                        blocked = world->WorldPosIsSolid(
                            glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f)) == 2;
            if (blocked) break;
            safeDistance = distance;
        }
        cameraPos = camera.Position + direction * safeDistance;
    }

    return glm::lookAt(cameraPos, cameraPos + cameraFront, camera.Up);
}

void Character::ApplyRemoteState(const glm::vec3& pos,
    float yaw, float pitch, int block)
{
    // 받은 위치를 적용하기 전에 이동 여부를 확인한다.
    const glm::vec3 delta = pos - Position;
    const bool moved = glm::dot(delta, delta) > 0.000001f;

    Position = pos;

    // 패킷의 yaw는 카메라 기준, 모델의 yaw는 반대 부호다.
    camera.Yaw = yaw;
    camera.Pitch = pitch;
    HeadYaw = -yaw;
    HeadPitch = pitch;
    BodyYaw = HeadYaw;

    // 입력 없이 받은 슬롯 번호만 적용한다.
    // 최초 수신에는 같은 슬롯이어도 정점 생성이 필요할 수 있다.
    if (block >= 0 && block < 9 &&
        (block_kind != block || block_vertices.empty()))
    {
        block_kind = block;
        SetBlockVertices();
    }

    // 이동 모드는 패킷에 없으므로 기본 걷기 값을 사용한다.
    if (moved)
    {
        if (shake_state)
        {
            shake_deg += ADD_SHAKE_DEG[0];
            if (shake_deg >= MAX_SHAKE_DEG[0])
            {
                shake_deg = MAX_SHAKE_DEG[0];
                shake_state = false;
            }
        }
        else
        {
            shake_deg -= ADD_SHAKE_DEG[0];
            if (shake_deg <= -MAX_SHAKE_DEG[0])
            {
                shake_deg = -MAX_SHAKE_DEG[0];
                shake_state = true;
            }
        }
    }
    else
    {
        shake_deg /= 1.1f;
        if (glm::abs(shake_deg) < 0.1f)
            shake_deg = 0.0f;
    }

    // 변경된 위치와 몸 각도로 신체 위치를 다시 계산한다.
    PosUpdate();
}