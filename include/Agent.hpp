#pragma once
#include <cmath>
#include <cstdlib>
#include <cstdint>

extern const int WIDTH;
extern const int HEIGHT;
extern const float PI;
extern const float REPULSION;

enum TerrainType : uint8_t
{
    AIR = 0,
    OBSTACLE = 1,
    FOOD = 2,
    HOME = 3
};

bool validCoords(int x, int y)
{
    return (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
}

// The Agent Struct
struct Agent
{
    float x, y;
    float angle;
    float speed;

    // State
    bool hasFood = false;

    // Reinforcement Hyperparameters
    float baseDeposit = 0.1f;
    float currentDeposit = 1.0f;
    float depositDecay = 0.99f;
    float pheromoneBoost = 100.0f;

    // Sensing Specifications. You need to modify these in order to change the simulation behaviour.
    float turnAngle = PI / 10.0f;
    float sensorAngleOffset = PI / 2.0f;
    float sensorDistance = 40.0f;

    void senseAndSteer(const float *readGrid, const uint8_t *terrainGrid)
    {
        float sensorAngleLeft = angle + sensorAngleOffset;
        float sensorAngleFwd = angle;
        float sensorAngleRight = angle - sensorAngleOffset;

        int leftSensorX = static_cast<int>(x + sensorDistance * cos(sensorAngleLeft));
        int leftSensorY = static_cast<int>(y + sensorDistance * sin(sensorAngleLeft));

        int fwdSensorX = static_cast<int>(x + sensorDistance * cos(sensorAngleFwd));
        int fwdSensorY = static_cast<int>(y + sensorDistance * sin(sensorAngleFwd));

        int rightSensorX = static_cast<int>(x + sensorDistance * cos(sensorAngleRight));
        int rightSensorY = static_cast<int>(y + sensorDistance * sin(sensorAngleRight));

        // Returns the appropriate weight based on terrain type at the sensor's location
        auto getWeight = [&](int x, int y) -> float
        {
            // Out of Simulation Bounds
            if (!validCoords(x, y))
                return REPULSION;

            int index = y * WIDTH + x;
            // Hit an obstacle
            if (terrainGrid[index] == OBSTACLE)
                return REPULSION;

            // Found food
            if (terrainGrid[index] == FOOD)
                return 1000.0f;

            // Found home
            if (terrainGrid[index] == HOME)
                return 1000.0f;

            int sniffChannel = hasFood ? 0 : 1;
            float sensedValue = readGrid[index * 3 + sniffChannel];

            return sensedValue;
        };

        float LeftSensorValue = getWeight(leftSensorX, leftSensorY);
        float FwdSensorValue = getWeight(fwdSensorX, fwdSensorY);
        float RightSensorValue = getWeight(rightSensorX, rightSensorY);

        // Discrete Steering Logic
        if (FwdSensorValue > LeftSensorValue && FwdSensorValue > RightSensorValue)
            angle += 0.0f;
        else if (FwdSensorValue == REPULSION || LeftSensorValue == REPULSION || RightSensorValue == REPULSION)
        {
            currentDeposit = baseDeposit;

            if (rand() % 2 == 0)
                angle += turnAngle;
            else
                angle -= turnAngle;
        }
        else if (FwdSensorValue < LeftSensorValue && FwdSensorValue < RightSensorValue)
        {
            if (rand() % 2 == 0)
                angle += turnAngle;
            else
                angle -= turnAngle;
        }
        else if (LeftSensorValue > RightSensorValue)
            angle += turnAngle;
        else if (RightSensorValue > LeftSensorValue)
            angle -= turnAngle;
    }

    // Deposit Pheromones at (x, y)
    void deposit(float *writeGrid)
    {
        int gridX = static_cast<int>(x);
        int gridY = static_cast<int>(y);

        if (validCoords(gridX, gridY))
        {
            int depositChannel = hasFood ? 1 : 0;
            writeGrid[(gridY * WIDTH + gridX) * 3 + depositChannel] += currentDeposit;
        }
    }

    // Detect Food at (x, y)
    void detectState(uint8_t *terrainGrid)
    {
        int gridX = static_cast<int>(x);
        int gridY = static_cast<int>(y);

        if (!validCoords(gridX, gridY))
            return;

        int terrain = terrainGrid[gridY * WIDTH + gridX];

        if (!hasFood && terrain == FOOD)
        {
            hasFood = true;
            angle += PI;
            currentDeposit = baseDeposit * pheromoneBoost;
        }
        else if (hasFood && terrain == HOME)
        {
            hasFood = false;
            angle += PI;
            currentDeposit = baseDeposit * pheromoneBoost;
        }
        else
        {
            currentDeposit = std::max(baseDeposit, currentDeposit * depositDecay);
        }
    }
};