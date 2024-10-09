#pragma once

#define glsl(name, src) const char* name = "#version 420 core\n" #src;