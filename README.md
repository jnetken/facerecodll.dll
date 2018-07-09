# facerecodll.dll
========================================================================
facerecodll.dll
作者：jnetken@126.com
时间：2018-7-1
========================================================================

目的和用途：
facerecodll.dll 基于虹软公司（http://www.arcsoft.com.cn）的人脸识别引擎封装，以便在不同语言的应用中非常简单地调用

提供两个主要函数：
GetFeature - 获取含有人脸图片的特征
FaceMatch - 对两个人脸特征进行比对，返回相似度

编译方式：
本项目只使用了Windows 32位下的虹软SDK，源代码在VS2013的Release Win32模式下编译通过。

在其他应用直接调用本项目：
1、将以下三个DLL文件复制到应用中
facerecodll.dll
libarcsoft_fsdk_face_detection.dll
libarcsoft_fsdk_face_recognition.dll
2、函数调用形式
int __stdcall GetFeature(const char * path, char * feature);
参数说明：
path	图片文件路径
feature	返回的人脸特征数据（调用时请先申请32000字节空间）
返回值：
大于0	人脸特征数据的字节数
-1	内存初始化失败
-2	初始化detection引擎失败
-3	图片打开错误
-4	未检测到人脸
-5	初始化recognition引擎错误
-6	获取人脸特征失败
	
float __stdcall FaceMatch(const char * feature1, const int size1, const char * feature2, const int size2);
参数说明：
feature1	第一个人脸特征数据
size1	第一个人脸特征数据字节数
feature2	第二个人脸特征数据a
size2	第二个人脸特征数据字节数
返回值：
大于0	两个人脸特征的相似度（如果超过0.6，一般可认定是同一个人脸）
0	两个人脸特征完全不相似
-5	初始化recognition引擎错误
-7	比对失败

关于APPID和SDKKey的说明：
根据虹软公司的协议，每个使用者必须单独向虹软公司申请APPID和SDKKey，不得未经授权向第三方应用提供。
所以，请使用者自行向虹软公司申请APPID和SDKKey。

版权说明：
其中所使用的虹软公司的人脸识别引擎sdk，版权属于虹软公司，使用者必须遵循虹软公司关于该产品的版权说明。
所有源码基于BSD开源协议，允许任意修改使用。


