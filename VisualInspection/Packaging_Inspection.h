#pragma once
/// CMyMatchClass.h头文件中
/************************************************************************
*  Copyright(c) 2000-2013 Company Name
*  All rights reserved.
*
*  文件名称: CMyMatchClass
*
*   简要描述:             图像匹配类
*
*  修改日期: 2019-11-20
*  作者: Jesse
*  说明:
***********************************************************************/
#pragma once      

#include<stdio.h>
#include "halconcpp/HalconCpp.h"
#include <QImage>


using namespace HalconCpp;

class CMyMatchClass
{
public:
	CMyMatchClass();
	~CMyMatchClass();

public:

	void Create_Model(HObject ho_TemplateImage_roi);
	BOOL Find_Model(HObject ho_Image, HTuple hv_ModelID,double Min_Score);
	void Clear_Model(HTuple hv_ModelID);
	HObject QImageToHObject(QImage image);
//	void ReadFile(QString strPath);
	HTuple hv_ModelID;

public:
		int m_nFileCount;
};
