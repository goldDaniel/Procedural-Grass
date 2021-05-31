#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_sdl.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"
#include "Vendor/imgui/metrics_gui/include/metrics_gui/metrics_gui.h"


#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>

#include <cstdlib>



#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

#include "Camera.h"
#include "Input.h"


#include "Renderer/Texture.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Frustum.h"

#endif