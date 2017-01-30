#pragma once
#include "EDSDK.h"

static bool bContinue;

static EdsError EDSCALLBACK HandleObjectEvent(EdsUInt32 InEvent, EdsBaseRef InRef, EdsVoid* InContext);

static EdsError EDSCALLBACK HandlePropertyEvent(EdsUInt32 InEvent, EdsUInt32 InPropertyID, EdsUInt32 InParam, EdsVoid* InContext);

static EdsError EDSCALLBACK HandleStateEvent(EdsUInt32 InEvent, EdsUInt32 InParam, EdsVoid* InContext);

static EdsError DownloadImage(EdsDirectoryItemRef directoryItem);

static EdsError TakePicture(EdsCameraRef camera);

static EdsError GetVolume(EdsCameraRef camera, EdsVolumeRef * Volume);

static EdsError GetFirstCamera(EdsCameraRef *Camera);

static EdsError GetTv(EdsCameraRef Camera, EdsUInt32 *Tv);

static EdsError GetTvDesc(EdsCameraRef camera, EdsPropertyDesc *TvDesc);

static EdsError SetTv(EdsCameraRef camera, EdsUInt32 TvValue);

void OldCode();

void ApplicationRun();