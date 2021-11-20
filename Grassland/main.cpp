#include <Grassland.h>

#include <Windows.h>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>


using namespace Grassland;

int main()
{
    SetConsoleOutputCP(936);
    
    GRLDirectXInit(800, 600, "Grassland D3D12", false);
    while (!GRLDirectXPollEvent());
}