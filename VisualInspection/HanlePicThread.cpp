#include "HanlePicThread.h"
#include <QDebug>
#include <QMutex>
#include <vector>
#include <QDateTime>

QMutex ListLock;
HanlePicThread::HanlePicThread()
{

}

void HanlePicThread::run()
{
    int ListSize = 0;
    while (ThreadRunnning)
    {
        {
            ListLock.lock();
            ListSize = HalconObjlist.size();
            ListLock.unlock();
        }
        if(ListSize!=0)
        {
            CurObj = HalconObjlist.front();
            //msleep(1500);
            int Result = 0;
            Result = MatchObj.Find_Model(CurObj,MatchObj.hv_ModelID,0.5);
            qDebug()<<"compare result:";
            qDebug()<<Result;
            qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-DD HH:mm:ss:zzz");
            HalconObjlist.pop_front();
            emit SendCompareResult(Result);
        }
    }
}

void HanlePicThread::GetFrame(cv::Mat Frame)
{
    ListLock.lock();
    HObject Hobj;
    Mat2HObject(Frame,Hobj);
    HalconObjlist.push_back(Hobj);
    ListLock.unlock();
}

void HanlePicThread::GetTempaltePic(cv::Mat Frame)
{
    //MatchObj.Clear_Model(MatchObj.hv_ModelID);
    HObject TemplateHobj;
    Mat2HObject(Frame,TemplateHobj);
    MatchObj.Create_Model(TemplateHobj);
}

void HanlePicThread::Mat2HObject(const cv::Mat &image, HObject &outObj)
{
    HObject Hobj = HObject();
    int hgt = image.rows;
    int wid = image.cols;
    int i;
    //	CV_8UC3
    if (image.type() == CV_8UC3)
    {
        std::vector<cv::Mat> imgchannel;
        split(image, imgchannel);
        cv::Mat imgB = imgchannel[0];
        cv::Mat imgG = imgchannel[1];
        cv::Mat imgR = imgchannel[2];
        uchar *dataR = new uchar[hgt * wid];
        uchar *dataG = new uchar[hgt * wid];
        uchar *dataB = new uchar[hgt * wid];
        for (i = 0; i < hgt; i++)
        {
            memcpy(dataR + wid * i, imgR.data + imgR.step * i, wid);
            memcpy(dataG + wid * i, imgG.data + imgG.step * i, wid);
            memcpy(dataB + wid * i, imgB.data + imgB.step * i, wid);
        }
        GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
        delete[]dataR;
        delete[]dataG;
        delete[]dataB;
        dataR = NULL;
        dataG = NULL;
        dataB = NULL;
    }
    //	CV_8UCU1
    else if (image.type() == CV_8UC1)
    {
        uchar *data = new uchar[hgt * wid];
        for (i = 0; i < hgt; i++)
            memcpy(data + wid * i, image.data + image.step * i, wid);
        GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
        delete[] data;
        data = NULL;
    }
    outObj = Hobj;
}

