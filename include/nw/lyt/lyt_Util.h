#pragma once

#include <nw/lyt/lyt_Types.h>

namespace nw {
namespace lyt {

class Pane;
class Layout;
class DrawInfo;
class Group;
class AnimTransform;
class TexMap;

void BindAnimation(Group* pGroup, AnimTransform* pAnimTrans, bool bRecursive = true, bool bDisable = false);
void UnbindAnimation(Group* pGroup, AnimTransform* pAnimTrans, bool bRecursive = true);
void SetAnimationEnable(Group* pGroup, AnimTransform* pAnimTrans, bool bEnable, bool bRecursive = true);

bool IsContain(Pane* pPane, const nn::math::VEC2& pos);
Pane* FindHitPane(Pane* pPane, const nn::math::VEC2& pos);
Pane* FindHitPane(Layout* pLayout, const nn::math::VEC2& pos);
Pane* GetNextPane(Pane* pPane);

const TextureInfo LoadTexture(const void* pImgRes, u32 size, int texLoadFlag = 0);
void DeleteTexture(const TextureInfo& texInfo);
void CalcTextureMtx(nw::math::MTX23* pTexMtx, const TexSRT& texSRT, const TexMap& texMap);

}
}