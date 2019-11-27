#include "RedrawComboBox.h"

RedrawComboBox::RedrawComboBox(QWidget *parent):QComboBox(parent)
{

}

void RedrawComboBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked();  //触发clicked信号
    }

    QComboBox::mousePressEvent(event);
}
