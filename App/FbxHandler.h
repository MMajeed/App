#ifndef FBX_HANDLER_H
#define FBX_HANDLER_H

#include "cFBXBuffer.h"

class Mesh;
class SkeletalAnimation;

Mesh LoadMeshFromFbx(std::string path);
SkeletalAnimation LoadBakedAnimationFromFBX(std::string path);\

#endif