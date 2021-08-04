#include <QStatusButton.hpp>
#include <QPainter>

QStatusButton::QStatusButton (QWidget* parent) : QPushButton (parent), status (Status::OFF)
{
    this->setAutoFillBackground(true);
}

QStatusButton::~QStatusButton () = default;

void QStatusButton::setStatus (Status set)
{
    status = set;
}

Status QStatusButton::getStatus ()
{
    return status;
}

void QStatusButton::doPress ()
{

}

void QStatusButton::paintEvent (QPaintEvent* e)
{
    QPushButton::paintEvent (e);

    QPalette pal = palette();
    switch (status)
    {
        case OFF:
            pal.setBrush (QPalette::Button, Qt::gray);
            break;
        case OK:
            pal.setBrush (QPalette::Button, Qt::green);
            break;
        case WARNING:
            pal.setBrush (QPalette::Button, Qt::yellow);
            break;
        case ERROR:
            pal.setBrush (QPalette::Button, Qt::red);
            break;
    }
    this->setPalette (pal);
}
