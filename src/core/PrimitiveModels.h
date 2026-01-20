#pragma once

#include "../enums/PartType.h"
#include "src/graphics/Texture2D.h"
#include "src/graphics/Mesh.h"
#include "src/graphics/Model.h"
#include "src/datatypes/Vector3.h"
#include "src/core/Constants.h"

#include <cstring>
#include <unordered_map>

void PreparePrimitiveModels();
void UnloadPrimitiveModels();
Engine::Graphics::Model *GetPrimitiveModel(PartType shape);
