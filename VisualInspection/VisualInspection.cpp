#include "VisualInspection.h"
#include "ui_VisualInspection.h"
#include <QFileDialog>
#include <QFile>
#include <QList>
#include <QDebug>
#include "RedrawComboBox.h"
#include <QMessageBox>
#include <QThread>
#include <QMutex>
#include <QDateTime>
#include "halconcpp/HalconCpp.h"
using namespace HalconCpp;

QMutex o_mutex;

VisualInspection::VisualInspection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisualInspection)
{
    ui->setupUi(this);
    ui->label_2->setScaledContents(true);
    connect(ui->PortBox,SIGNAL(clicked()),this,SLOT(on_PortBox_Clicked()));
    //connect(ui->PortBox,SIGNAL(clicked()),this,SLOT(on_PortBox_Clicked()));
    ui->PortBox->clear();
    foreach(const QSerialPortInfo &portinfo,QSerialPortInfo::availablePorts())
    {
       ui->PortBox->addItem(portinfo.portName());
    }

    Serial = new QSerialPort;
    connect(Serial,SIGNAL(readyRead()),this,SLOT(RecvMessgae()));
    connect(&GetFrameTimer,SIGNAL(timeout()),this,SLOT(GetFrameFromCap()));
    connect(&SendCommandTimer,SIGNAL(timeout()),this,SLOT(SendCommand()));

    connect(&ThreadObj,SIGNAL(SendCompareResult(int)),this,SLOT(GetCompareResult(int)));
    connect(this,SIGNAL(SendFrame(cv::Mat)),&ThreadObj,SLOT(GetFrame(cv::Mat)));
    connect(this,SIGNAL(SendTemplate(cv::Mat)),&ThreadObj,SLOT(GetTempaltePic(cv::Mat)));
}

VisualInspection::~VisualInspection()
{
    delete ui;
}

void VisualInspection::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.jpeg *.jpg)"));
    if (!fileName.isEmpty())
    {
        QString PicPath;
        PicPath = fileName;
        PicPath.replace("/","\\");
        cv::Mat img = cv::imread(PicPath.toLocal8Bit().data());
        emit SendTemplate(img);
        ui->label_2->setPixmap(QPixmap::fromImage(cvMat_to_QImage(img)));
    }
}

void VisualInspection::on_PortBox_Clicked()
{
    ui->PortBox->clear();
    foreach(const QSerialPortInfo &portinfo,QSerialPortInfo::availablePorts())
    {
       ui->PortBox->addItem(portinfo.portName());
    }
}

QImage VisualInspection::cvMat_to_QImage(const cv::Mat &mat )
{
  switch ( mat.type() )
  {
     // 8-bit, 4 channel
     case CV_8UC4:
     {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
        return image;
     }

     // 8-bit, 3 channel
     case CV_8UC3:
     {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
     }

     // 8-bit, 1 channel
     case CV_8UC1:
     {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
           for ( int i = 0; i < 256; ++i )
              sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
     }

     default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
        break;
  }
  return QImage();
}


void VisualInspection::on_pushButton_2_clicked()
{
    if(ui->pushButton_2->text()==QString::fromLocal8Bit("开始"))
    {
        cap.open(1);
        if (!cap.isOpened())
        {
            QMessageBox::warning(this,"warning","open camera fail!");
            return;
        }
        GetFrameTimer.start(30);
        Serial->setPortName(ui->PortBox->currentText());
        if(Serial->isOpen())
        {
            Serial->clear();
            Serial->close();
        }
        if(!Serial->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,"warning","open port fail!");
            cap.release();
            return;
        }
        Serial->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);//设置波特率和读写方向
        Serial->setDataBits(QSerialPort::Data8);		//数据位为8位
        Serial->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        Serial->setParity(QSerialPort::NoParity);	//无校验位
        Serial->setStopBits(QSerialPort::OneStop); //一位停止位
        ThreadObj.ThreadRunnning = true;
        ThreadObj.start();
        SendCommandTimer.start(110);        
        ui->PortBox->setDisabled(true);
        ui->pushButton_2->setText(QString::fromLocal8Bit("停止"));
    }
    else
    {
        QThread::msleep(400);
        cap.release();
        if(Serial->isOpen())
        {
            Serial->clear();
            Serial->close();
        }
        ui->PortBox->setDisabled(false);
        SendCommandTimer.stop();
        ui->pushButton_2->setText(QString::fromLocal8Bit("开始"));
        ui->label_4->clear();
    }
}

void VisualInspection::RecvMessgae()
{
    //SendCommandTimer.stop();
    QString message;
    QString TempMsg;
    QByteArray Tempbuff;
    Tempbuff = Serial->readAll();
    buff.append(Tempbuff);
    if(buff.size()>=2)
    {
        if(buff.toHex().at(3)-'0'==2)  //查光耦的回复
        {
            if(buff.size()==6)
            {
                int bit[8];
                message = buff.toHex();
                for(int i =0; i<8; i++)
                {
                     bit[i] = ((buff.at(3) >> i) & 1);
                     if(bit[i] == 1) //传感器有返回值
                     {
                         if(i==0)  //拍照传感器检测到产品，DI1
                         {

                             if(IsAnotherOne1)
                             {
                                 qDebug()<<"DI1";
                                 IsAnotherOne1 = false;
                                 cap.read(Frame1);
                                 ui->label_4->setPixmap(QPixmap::fromImage(cvMat_to_QImage(Frame1)));
                                 emit SendFrame(Frame1);
                                 qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-DD HH:mm:ss:zzz");
                             }
                         }
                         else if (i == 1)  //不良传感器检测到产品，DI2
                         {                            
                             if(IsAnotherOne2)
                             {
                                 IsAnotherOne2 = false;
                                 if(CompareResult.size()!=0&&Index<CompareResult.size()&&!CompareResult.at(Index)&&!IsVerticalCyDropDown)
                                 {
                                     qDebug()<<"DI2";
                                     WritePort("FE050000FF009835");  //控制继电器1开，控制气缸下降
                                     IsVerticalCyDropDown = true;
                                     Index++;
                                 }
                             }
                         }
                         else if (i == 2)  //气缸下降完成，DI3
                         {
                             if(IsVerticalCyDropDown&&!IsHorizontalCy)
                             {
                                 qDebug()<<"DI3";
                                 WritePort("FE050001FF00C9F5");  //控制继电器2开，控制气缸横推
                                 IsHorizontalCy = true;
                             }
                         }
                         else if (i == 3)  //气缸横推完成，DI4
                         {
                             if(IsHorizontalCy&&IsVerticalCyDropDown)
                             {
                                 qDebug()<<"DI4";
                                 WritePort("FE0500000000D9C5");  //控制继电器1断开，气缸上升
                                 IsVerticalCyDropDown = false;
                             }
                         }
                         else if (i == 4)  //气缸上升完成，DI5
                         {
                             if(IsHorizontalCy&&!IsVerticalCyDropDown)
                             {
                                 qDebug()<<"DI5";
                                 WritePort("FE05000100008805");  //控制继电器2断关，横向气缸收回
                                 IsHorizontalCy = false;
                                 qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-DD HH:mm:ss:zzz");
                             }
                         }
                     }
                     else
                     {
                         if(i==0)
                         {
                             IsAnotherOne1 = true;
                             ui->label_4->clear();
                         }
                         else if (i == 1)
                         {
                             IsAnotherOne2 = true;
                         }
                     }
                }
                buff.clear();
            }
        }
        else if (buff.toHex().at(3)-'0'==5)  //控制继电器的回复
        {
            if(buff.size()==8)
            {
               buff.clear();
            }
        }
    }
}

void VisualInspection::SendCommand()
{
    WritePort("FE02000000086DC3");
}

void VisualInspection::GetCompareResult(int ok)
{
    CompareResult.push_back(ok);
}

qint64 VisualInspection::WritePort(QString str)
{
    o_mutex.lock();
    QByteArray b;
    b = QByteArray::fromHex(str.toLatin1());
    qint64 Bytes = Serial->write(b);
    o_mutex.unlock();
    return  Bytes;
}

void VisualInspection::GetFrameFromCap()
{
    cap.read(Frame2);
}

void VisualInspection::closeEvent(QCloseEvent *event)
{
    if(ThreadObj.ThreadRunnning==true)
    {
        ThreadObj.ThreadRunnning = false;
        QThread::msleep(1000);
    }
}
