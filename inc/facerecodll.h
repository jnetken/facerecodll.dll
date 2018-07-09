#pragma once

extern "C" _declspec(dllexport) int __stdcall GetFeature(const char * path, char * feature);
extern "C" _declspec(dllexport) float __stdcall FaceMatch(const char * feature1, const int size1, const char * feature2, const int size2);