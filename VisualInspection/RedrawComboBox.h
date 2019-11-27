#ifndef REDRAWCOMBOBOX_H
#define REDRAWCOMBOBOX_H
#include <QComboBox>
#include <QMouseEvent>

class RedrawComboBox : public QComboBox
{
    Q_OBJECT
public:
    RedrawComboBox(QWidget *parent);
signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
};

#endif // REDRAWCOMBOBOX_H
