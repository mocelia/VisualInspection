// Packaging_Inspection.cpp : 定义 DLL 应用程序的导出函数。
//
#include "Packaging_Inspection.h"



CMyMatchClass::CMyMatchClass()
{

}


CMyMatchClass::~CMyMatchClass()
{

}

//
////Qt彩色转Halcon彩色
HObject CMyMatchClass::QImageToHObject(QImage image)
{


    unsigned char *data = image.bits();//获取图像像素字节数据的首地址
    int width = image.width();//图像宽
    int height = image.height();//图像高
    unsigned char* dataRed = (unsigned char*)malloc(width*height);//存储处理后的数据
    unsigned char* dataGreen = (unsigned char*)malloc(width*height);//存储处理后的数据
    unsigned char* dataBlue = (unsigned char*)malloc(width*height);//存储处理后的数据
    try
    {
        if (dataRed == NULL)
        {
            HObject Image;
            free(dataRed);
            free(dataGreen);
            free(dataBlue);
            return Image;
        }
        int img_format = image.format();

        if (img_format == QImage::Format_RGB888)
        {
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++) {
                    dataRed[i*width + j] = *(data + 2);
                    dataGreen[i*width + j] = *(data + 1);
                    dataBlue[i*width + j] = *data;
                    data += 4;
                }
            }
            HObject Image;
            GenImage3(&Image, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
            free(dataRed);
            free(dataGreen);
            free(dataBlue);
            //RotateImage(Image, &Image, 90, "constant");
            return Image;
        }
        if (img_format == QImage::Format_Mono || img_format == QImage::Format_Indexed8)
        {
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++) {

                    dataRed[i*width + j] = *(data + 2);
                    data += 1;
                }
            }
            HObject Image;
            GenImage1(&Image, "byte", width, height, (Hlong)(dataRed));
            free(dataRed);
            free(dataGreen);
            free(dataBlue);
            //RotateImage(Image, &Image, 90, "constant");
            return Image;
        }

        if (img_format == QImage::Format_RGB888)
        {
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++) {
                    dataRed[i*width + j] = *(data + 2);
                    dataGreen[i*width + j] = *(data + 1);
                    dataBlue[i*width + j] = *data;
                    data += 3;
                }
            }
            HObject Image;
            GenImage3(&Image, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
            free(dataRed);
            free(dataGreen);
            free(dataBlue);
            //RotateImage(Image, &Image, 90, "constant");
            return Image;
        }
    }
    catch (HalconCpp::HException e)
    {
        HObject Image;
        free(dataRed);
        free(dataGreen);
        free(dataBlue);
        return Image;
    }
}


void CMyMatchClass::Create_Model(HObject ho_TemplateImage_roi)
{

	CreateShapeModel(ho_TemplateImage_roi, 4, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
		HTuple(1.5289).TupleRad(), (HTuple("none").Append("no_pregeneration")), "use_polarity",
		((HTuple(30).Append(31)).Append(8)), 4, &hv_ModelID);
}

BOOL CMyMatchClass::Find_Model(HObject ho_Image, HTuple hv_ModelID, double Min_Score)
{
	if (hv_ModelID.Length() == 0)
	{
		return 0;
	}
	HTuple  hv_Row, hv_Column, hv_Angle, hv_Score;
	FindShapeModel(ho_Image, hv_ModelID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
		Min_Score, 0, 0.5, "least_squares", (HTuple(4).Append(1)), 1, &hv_Row, &hv_Column,
		&hv_Angle, &hv_Score);
	if (hv_Score.Length() > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CMyMatchClass::Clear_Model(HTuple hv_ModelID)
{
	ClearShapeModel(hv_ModelID);
}
