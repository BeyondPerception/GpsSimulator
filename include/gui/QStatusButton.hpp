#ifndef QSTATUSBUTTON_HPP
#define QSTATUSBUTTON_HPP

#include <QObject>
#include <QPushButton>

enum Status
{
    OFF,
    OK,
    WARNING,
    ERROR
};

Q_DECLARE_METATYPE (Status)

class QStatusButton : public QPushButton
{
public:
    explicit QStatusButton (QWidget* parent = nullptr);

    ~QStatusButton () override;

    void setStatus (Status set);

    Status getStatus ();

    void doPress ();

protected:
    void paintEvent (QPaintEvent* e) override;

private:
    Status status;
};


#endif //QSTATUSBUTTON_HPP
