#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include "arcsoft_fsdk_face_detection.h"
#include "arcsoft_fsdk_face_recognition.h"
#include "merror.h"
#include "facerecodll.h"

#pragma comment(lib,"libarcsoft_fsdk_face_detection.lib")
#pragma comment(lib,"libarcsoft_fsdk_face_recognition.lib")

#define WORKBUF_SIZE        (40*1024*1024)
#define APPID			"填写你的APPID"
#define SDKKeyReco		"填写你的人脸识别SDKKey"
#define SDKKeyDetect	"填写你的人脸检测SDKKey"

/*读入图片文件*/
bool readBmp24(const char* path, uint8_t **imageData, int *pWidth, int *pHeight)
{
	if (path == NULL || imageData == NULL || pWidth == NULL || pHeight == NULL)
	{
		return false;
	}
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
	{
		return false;
	}
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	*pWidth = head.biWidth;
	*pHeight = head.biHeight;
	int biBitCount = head.biBitCount;
	if (24 == biBitCount)
	{
		int lineByte = ((*pWidth) * biBitCount / 8 + 3) / 4 * 4;
		*imageData = (uint8_t *)malloc(lineByte * (*pHeight));
		uint8_t * data = (uint8_t *)malloc(lineByte * (*pHeight));
		fseek(fp, 54, SEEK_SET);
		fread(data, 1, lineByte * (*pHeight), fp);
		for (int i = 0; i < *pHeight; i++)
		{
			for (int j = 0; j < *pWidth; j++)
			{
				memcpy((*imageData) + i * (*pWidth) * 3 + j * 3, data + (((*pHeight) - 1) - i) * lineByte + j * 3, 3);
			}
		}
		free(data);
	}
	else
	{
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}

int __stdcall GetFeature(const char * path, char * feature)
{
	char buf[32000];
	MRESULT nRet = MERR_UNKNOWN;
	MHandle hEngine = nullptr;
	MInt32 nScale = 16;
	MInt32 nMaxFace = 10;
	MByte *pWorkMem = (MByte *)malloc(WORKBUF_SIZE);
	MHandle hEngine1 = nullptr;
	MByte *pWorkMem1 = (MByte *)malloc(WORKBUF_SIZE);

	if (pWorkMem == nullptr)
	{
		return -1;
	}
	if (pWorkMem1 == nullptr)
	{
		return -1;
	}
	/* 初始化detection引擎 */
	nRet = AFD_FSDK_InitialFaceEngine(APPID, SDKKeyDetect, pWorkMem, WORKBUF_SIZE, &hEngine, AFD_FSDK_OPF_0_HIGHER_EXT, nScale, nMaxFace);
	if (nRet != MOK)
	{
		free(pWorkMem);
		return -2;
	}

	/* 版本信息 */
	/*
	const AFR_FSDK_Version * pVersionInfo = nullptr;
	pVersionInfo = AFR_FSDK_GetVersion(hEngine);
	sprintf(buf, "%d %d %d %d %d\n%s\n%s\n%s\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild, pVersionInfo->lFeatureLevel, pVersionInfo->Version, pVersionInfo->BuildDate, pVersionInfo->CopyRight);
	*/

	/* 读取第一张静态图片信息，并保存到ASVLOFFSCREEN结构体 （以ASVL_PAF_RGB24_B8G8R8格式为例） */
	ASVLOFFSCREEN offInput1 = { 0 };
	offInput1.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	offInput1.ppu8Plane[0] = nullptr;
	readBmp24(path, (uint8_t**)&offInput1.ppu8Plane[0], &offInput1.i32Width, &offInput1.i32Height);
	if (!offInput1.ppu8Plane[0])
	{
		sprintf(buf, "fail to ReadBmp(%s)\n", path);
		AFD_FSDK_UninitialFaceEngine(hEngine);
		free(pWorkMem);
		return -3;
	}
	else
	{
		sprintf(buf, "Picture width : %d , height : %d \n", offInput1.i32Width, offInput1.i32Height);
	}
	offInput1.pi32Pitch[0] = offInput1.i32Width * 3;

	/* 人脸检测 */
	LPAFD_FSDK_FACERES	FaceRes1 = nullptr;
	nRet = AFD_FSDK_StillImageFaceDetection(hEngine, &offInput1, &FaceRes1);
	if (nRet != MOK)
	{
		sprintf(buf, "Face Detection failed, error code: %d\n", nRet);
		AFD_FSDK_UninitialFaceEngine(hEngine);
		free(pWorkMem);
		return -4;
	}
	else
	{
		sprintf(buf, "The number of face: %d\n", FaceRes1->nFace);
		for (int i = 0; i < FaceRes1->nFace; ++i)
		{
			sprintf(buf, "Face[%d]: rect[%d,%d,%d,%d], Face orient: %d\n", i, FaceRes1->rcFace[i].left, FaceRes1->rcFace[i].top, FaceRes1->rcFace[i].right, FaceRes1->rcFace[i].bottom, FaceRes1->lfaceOrient[i]);
		}
	}

	/* 初始化recognition引擎 */
	nRet = AFR_FSDK_InitialEngine(APPID, SDKKeyReco, pWorkMem1, WORKBUF_SIZE, &hEngine1);
	if (nRet != MOK)
	{
		return -5;
	}

	AFR_FSDK_FACEINPUT faceInput;
	//第一张人脸信息通过face detection\face tracking获得
	faceInput.lOrient = FaceRes1->lfaceOrient[0];//人脸方向
	//人脸框位置
	faceInput.rcFace.left = FaceRes1->rcFace[0].left; 
	faceInput.rcFace.top = FaceRes1->rcFace[0].top; 
	faceInput.rcFace.right = FaceRes1->rcFace[0].right;  
	faceInput.rcFace.bottom = FaceRes1->rcFace[0].bottom; 

	//提取人脸特征
	AFR_FSDK_FACEMODEL LocalFaceModels = { 0 };
	nRet = AFR_FSDK_ExtractFRFeature(hEngine1, &offInput1, &faceInput, &LocalFaceModels);
	if (nRet != MOK)
	{
		sprintf(buf, "fail to Extract 1st FR Feature, error code: %d\n", nRet);
		AFD_FSDK_UninitialFaceEngine(hEngine);
		AFR_FSDK_UninitialEngine(hEngine1);
		free(pWorkMem);
		return -6;
	}

	/* 拷贝人脸特征结果 */
	memcpy(feature, LocalFaceModels.pbFeature, LocalFaceModels.lFeatureSize);

	/* 释放引擎和内存 */
	nRet = AFD_FSDK_UninitialFaceEngine(hEngine);
	if (nRet != MOK)
	{
		sprintf(buf, "UninitialFaceEngine failed , errorcode is %d \n", nRet);
	}
	nRet = AFR_FSDK_UninitialEngine(hEngine1);
	if (nRet != MOK)
	{
		sprintf(buf, "UninitialFaceEngine failed , errorcode is %d \n", nRet);
	}
	free(pWorkMem1);
	free(pWorkMem);

	return LocalFaceModels.lFeatureSize;
}

float __stdcall FaceMatch(const char * feature1, const int size1, const char * feature2, const int size2)
{
	float r;
	MRESULT nRet = MERR_UNKNOWN;
	MHandle hEngine1 = nullptr;
	MByte *pWorkMem1 = (MByte *)malloc(WORKBUF_SIZE);
	if (pWorkMem1 == nullptr)
	{
		return -1;
	}
	/* 初始化recognition引擎 */
	nRet = AFR_FSDK_InitialEngine(APPID, SDKKeyReco, pWorkMem1, WORKBUF_SIZE, &hEngine1);
	if (nRet != MOK)
	{
		return -5;
	}

	/* 对比两张人脸特征，获得比对结果 */
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	faceModels1.lFeatureSize = size1;
	faceModels1.pbFeature = (MByte*)malloc(faceModels1.lFeatureSize);
	memcpy(faceModels1.pbFeature, feature1, faceModels1.lFeatureSize);
	faceModels2.lFeatureSize = size2;
	faceModels2.pbFeature = (MByte*)malloc(faceModels2.lFeatureSize);
	memcpy(faceModels2.pbFeature, feature2, faceModels2.lFeatureSize);

	MFloat  fSimilScore = 0.0f;
	nRet = AFR_FSDK_FacePairMatching(hEngine1, &faceModels1, &faceModels2, &fSimilScore);
	if (nRet == MOK)
	{
		r = fSimilScore;
	}
	else
	{
		r = -7;
	}

	/* 释放引擎和内存 */
	AFR_FSDK_UninitialEngine(hEngine1);
	free(pWorkMem1);

	return r;
}