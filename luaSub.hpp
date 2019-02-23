#ifndef G_LUASUB_H
#define G_LUASUB_H
#include "luaScript.hpp"
#include "resource.h"
int luaGetSMouseX(lua_State *L);
int luaGetSMouseY(lua_State *L);
int luaGetSMouseL(lua_State *L);
int luaGetSMouseR(lua_State *L);
int luaGetSMouseM(lua_State *L);
int luaGetMouseX(lua_State *L);
int luaGetMouseY(lua_State *L);
int luaGetMouseL(lua_State *L);
int luaGetMouseR(lua_State *L);
int luaGetMouseM(lua_State *L);
int luaGetTickCount(lua_State *L);
int luaGetSystemTickCount(lua_State *L);
int luaSetTicks(lua_State *L);
int luaSetCCDZoom(lua_State *L);
int luaGetCCD(lua_State *L);
int luaGetCCDRed(lua_State *L);
int luaGetCCDGreen(lua_State *L);
int luaGetCCDBlue(lua_State *L);
int luaKey(lua_State *L);
int luaKeyDown(lua_State *L);
int luaKeyUp(lua_State *L);
int luaKey2(lua_State *L);
int luaKeyDown2(lua_State *L);
int luaKeyUp2(lua_State *L);
int luaAnalog(lua_State *L);
int luaHat(lua_State *L);
int luaGetType(lua_State *L);
int luaGetOption(lua_State *L);
int luaGetEffect(lua_State *L);
int luaGetUserOption(lua_State *L);
int luaGetUserEffect(lua_State *L);
int luaGetDir(lua_State *L);
int luaGetAngle(lua_State *L);
int luaGetPower(lua_State *L);
int luaGetSpring(lua_State *L);
int luaGetDamper(lua_State *L);
int luaGetBrake(lua_State *L);
int luaGetColor(lua_State *L);
int luaGetTop(lua_State *L);
int luaGetParent(lua_State *L);
int luaGetEnergy(lua_State *L);
int luaGetM(lua_State *L);
int luaGetI(lua_State *L);
int luaGetObjM(lua_State *L);
int luaGetObjI(lua_State *L);
int luaPosx(lua_State *L);
int luaPosy(lua_State *L);
int luaPosz(lua_State *L);
int luaGetY(lua_State *L);
int luaUnLinkBye(lua_State *L);
int luaUnLink(lua_State *L);
int luaLx(lua_State *L);
int luaLy(lua_State *L);
int luaLz(lua_State *L);
int luaWx(lua_State *L);
int luaWy(lua_State *L);
int luaWz(lua_State *L);
int luaVx(lua_State *L);
int luaVy(lua_State *L);
int luaVz(lua_State *L);
int luaFx(lua_State *L);
int luaFy(lua_State *L);
int luaFz(lua_State *L);
int luaAx(lua_State *L);
int luaAy(lua_State *L);
int luaAz(lua_State *L);
int luaEx(lua_State *L);
int luaEy(lua_State *L);
int luaEz(lua_State *L);
int luaRx(lua_State *L);
int luaRy(lua_State *L);
int luaRz(lua_State *L);
int luaGx(lua_State *L);
int luaGy(lua_State *L);
int luaGz(lua_State *L);
int luaXx(lua_State *L);
int luaXy(lua_State *L);
int luaXz(lua_State *L);
int luaYx(lua_State *L);
int luaYy(lua_State *L);
int luaYz(lua_State *L);
int luaZx(lua_State *L);
int luaZy(lua_State *L);
int luaZz(lua_State *L);
int luaQx(lua_State *L);
int luaQy(lua_State *L);
int luaQz(lua_State *L);
int luaQw(lua_State *L);
int luaObjPosx(lua_State *L);
int luaObjPosy(lua_State *L);
int luaObjPosz(lua_State *L);
int luaVel(lua_State *L);
int luaChips(lua_State *L);
int luaWeight(lua_State *L);
int luaRnd(lua_State *L);
int luaGetDt(lua_State *L);
int luaGetWidth(lua_State *L);
int luaGetHeight(lua_State *L);
int luaGetFps(lua_State *L);
int luaGetBase(lua_State *L);
int luaGetFaces(lua_State *L);
int luaGetTolerant(lua_State *L);
int luaMove3D(lua_State *L);
int luaLine3D(lua_State *L);
int luaMove2D(lua_State *L);
int luaLine2D(lua_State *L);
int luaSetColor(lua_State *L);
int luaRound(lua_State *L);
int luaLen3(lua_State *L);
int luaLen2(lua_State *L);
int luaPI(lua_State *L);
int luaToDeg(lua_State *L);
int luaToRad(lua_State *L);
int luaGetPlayers(lua_State *L);
int luaGetPlayerHostID(lua_State *L);
int luaGetPlayerMyID(lua_State *L);
int luaGetPlayerID(lua_State *L);
int luaGetPlayerCrushes(lua_State *L);
int luaGetPlayerResets(lua_State *L);
int luaGetPlayerInits(lua_State *L);
int luaGetPlayerColor(lua_State *L);
int luaGetPlayerChips(lua_State *L);
int luaGetPlayerX(lua_State *L);
int luaGetPlayerY(lua_State *L);
int luaGetPlayerZ(lua_State *L);
int luaGetPlayerArms(lua_State *L);
int luaGetPlayerYForces(lua_State *L);
int luaGetPlayerName(lua_State *L);
int luaGetFuel(lua_State *L);
int luaGetFuelMax(lua_State *L);
void luaUpdateVal();

#endif
