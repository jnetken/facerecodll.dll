facerecodll.dll
===

****************************************************
facerecodll.dll<br>
作者：jnetken@126.com<br>
时间：2018-7-1<br>
****************************************************
<br>
### 目的和用途：<br>
facerecodll.dll 基于虹软公司（ http://www.arcsoft.com.cn ） 的人脸识别引擎封装，以便在不同语言的应用中非常简单地调用<br>
<br>
### 提供两个主要函数：<br>
GetFeature - 获取含有人脸图片的特征<br>
FaceMatch - 对两个人脸特征进行比对，返回相似度<br>
<br>
### 编译方式：<br>
本项目只使用了Windows 32位下的虹软SDK，源代码在VS2013的Release Win32模式下编译通过。<br>
<br>
### 在其他应用直接调用本项目：<br>
#### 1、将以下三个DLL文件复制到应用中<br>
facerecodll.dll<br>
libarcsoft_fsdk_face_detection.dll<br>
libarcsoft_fsdk_face_recognition.dll<br>
#### 2、函数调用形式<br>
```C++
int __stdcall GetFeature(const char * path, char * feature);<br>
参数说明：<br>
path	图片文件路径<br>
feature	返回的人脸特征数据（调用时请先申请32000字节空间）\<br>
返回值：<br>
大于0	人脸特征数据的字节数<br>
-1	内存初始化失败<br>
-2	初始化detection引擎失败<br>
-3	图片打开错误<br>
-4	未检测到人脸<br>
-5	初始化recognition引擎错误<br>
-6	获取人脸特征失败<br>
```
<br>
```C++
float __stdcall FaceMatch(const char * feature1, const int size1, const char * feature2, const int size2);<br>
参数说明：<br>
feature1	第一个人脸特征数据<br>
size1	第一个人脸特征数据字节数<br>
feature2	第二个人脸特征数据<br>
size2	第二个人脸特征数据字节数<br>
返回值：<br>
大于0	两个人脸特征的相似度（如果超过0.6，一般可认定是同一个人脸）<br>
0	两个人脸特征完全不相似<br>
-5	初始化recognition引擎错误<br>
-7	比对失败<br>
```
<br>
### 关于APPID和SDKKey的说明：<br>
根据虹软公司的协议，每个使用者必须单独向虹软公司申请APPID和SDKKey，不得未经授权向第三方应用提供。<br>
所以，请使用者自行向虹软公司申请APPID和SDKKey。<br>
<br>
### 版权说明：<br>
其中所使用的虹软公司的人脸识别引擎sdk，版权属于虹软公司，使用者必须遵循虹软公司关于该产品的版权说明。<br>
所有源码基于BSD开源协议，允许任意修改使用。<br>


