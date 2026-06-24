#include <iostream>
#include <cmath>
#include <random>
#include "Engine.hpp" // Include our new graphics engine!
#include "Agent.hpp"
#include "ImageMapper.hpp"

const int WIDTH = 900;
const int HEIGHT = 900;
const int NUM_AGENTS = 10000;
const float PI = 3.14159;
const float MAX_SPEED = 5.0f;
const float DECAY_FACTOR = 0.99f;
const float REPULSION = -1000.0f;

float getRandomFloat(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

void addZone(uint8_t *terrainGrid, int centerX, int centerY, int radius, TerrainType type)
{
    for (int i = centerX - radius; i <= centerX + radius; i++)
    {
        for (int j = centerY - radius; j <= centerY + radius; j++)
        {
            if (validCoords(i, j))
                terrainGrid[j * WIDTH + i] = type;
        }
    }
}

int main(int argc, char **argv)
{
    uint8_t *terrainGrid = new uint8_t[WIDTH * HEIGHT]();

    // Only initialize terrain grid with something if an image file was added as a command line argument
    if (argc >= 2)
    {
        if (!loadTerrainMap(argv[1], terrainGrid, WIDTH, HEIGHT))
            return -1;
    }
    else // otherwise fill it with AIR
        std::fill(terrainGrid, terrainGrid + (WIDTH * HEIGHT), AIR);

    // Boot up the Graphics Engine
    RenderEngine engine(WIDTH, HEIGHT);
    if (!engine.init())
    {
        std::cerr << "Failed to initialize graphics engine!" << std::endl;
        return -1;
    }

    float *readGrid = new float[WIDTH * HEIGHT * 3]();  // First grid which we read to form the next grid
    float *writeGrid = new float[WIDTH * HEIGHT * 3](); // Second grid which we write to
    Agent *agents = new Agent[NUM_AGENTS]();            // We can use an array of structs for the agents

    int HomeX = 100;
    int HomeY = 100;
    int HomeRadius = 15;

    // addZone(terrainGrid, 100, 100, 15, FOOD);
    // addZone(terrainGrid, 100, 799, 15, FOOD);
    // addZone(terrainGrid, 799, 100, 15, FOOD);
    addZone(terrainGrid, 799, 799, 15, FOOD);
    addZone(terrainGrid, HomeX, HomeY, HomeRadius, HOME);

    for (int i = 0; i < NUM_AGENTS; i++)
    {
        agents[i].x = getRandomFloat(HomeX - HomeRadius, HomeX + HomeRadius);
        agents[i].y = getRandomFloat(HomeY - HomeRadius, HomeY + HomeRadius);
        agents[i].angle = getRandomFloat(0.0, 2.0 * PI);
        agents[i].speed = getRandomFloat(0.8f * MAX_SPEED, MAX_SPEED);
        agents[i].hasFood = false;
    }

    while (!engine.shouldClose())
    {
        #pragma omp parallel for collapse(2)
        for (int y = 1; y < HEIGHT - 1; y++)
        {
            for (int x = 1; x < WIDTH - 1; x++)
            {

                for (int c = 0; c < 3; c++)
                {
                    float sum = 0.0f;
                    for (int offsetX = -1; offsetX <= 1; offsetX++)
                    {
                        for (int offsetY = -1; offsetY <= 1; offsetY++)
                        {
                            int X = x + offsetX;
                            int Y = y + offsetY;
                            sum += readGrid[(Y * WIDTH + X) * 3 + c];
                        }
                    }

                    float averageBlur = sum / 9.0f;
                    float finalValue = averageBlur * DECAY_FACTOR;
                    finalValue = std::min(finalValue, 5.0f);
                    writeGrid[(y * WIDTH + x) * 3 + c] = finalValue;
                }
            }
        }

        #pragma omp parallel for collapse(2)
        for (int x = 0; x < WIDTH; x++) {
            for (int c = 0; c < 3; c++) {
                writeGrid[x * 3 + c] = readGrid[x * 3 + c] * DECAY_FACTOR; 
                writeGrid[((HEIGHT - 1) * WIDTH + x) * 3 + c] = readGrid[((HEIGHT - 1) * WIDTH + x) * 3 + c] * DECAY_FACTOR; 
            }
        }

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < HEIGHT; y++) {
            for (int c = 0; c < 3; c++) {
                writeGrid[(y * WIDTH) * 3 + c] = readGrid[(y * WIDTH) * 3 + c] * DECAY_FACTOR; 
                writeGrid[(y * WIDTH + WIDTH - 1) * 3 + c] = readGrid[(y * WIDTH + WIDTH - 1) * 3 + c] * DECAY_FACTOR; 
            }
        }

        for (int i = 0; i < NUM_AGENTS; ++i)
        {
            agents[i].x += cos(agents[i].angle) * agents[i].speed;
            agents[i].y += sin(agents[i].angle) * agents[i].speed;

            agents[i].senseAndSteer(readGrid, terrainGrid);
            agents[i].detectState(terrainGrid);
            agents[i].deposit(writeGrid);
        }

        engine.renderFrame(writeGrid);

        float *temp = readGrid;
        readGrid = writeGrid;
        writeGrid = temp;
    }

    delete[] writeGrid;
    delete[] readGrid;
    delete[] agents;
    engine.shutdown();

    return 0;
}