#pragma once

#define CLOUD_IMG_MAX 6
#define MOUNTAIN_IMG_MAX 3

void BackgroundSysInit(void);
void BackgroundInit(void);
void BackgroundMain(float castleVelocityIncr, float cloudPositionDesc1, float cloudPositionDesc2);
void BackgroundDraw(bool cloudFlag, bool castleFlag,bool endFlag);