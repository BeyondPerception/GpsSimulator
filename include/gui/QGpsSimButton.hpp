#ifndef QGPSSIMBUTTON_HPP
#define QGPSSIMBUTTON_HPP

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class QGpsSimButton : public QWidget
{
Q_OBJECT

public:
    explicit QGpsSimButton (QWidget* parent = nullptr);

    ~QGpsSimButton () override;

    uint16_t getDuration () const;

    double getLatitude () const;

    double getLongitude () const;

    uint16_t getHeight () const;

    void showLoading (bool enable);

public slots:

    void setDuration (uint16_t simFileDuration);

signals:

    void generatePressed ();

protected:
    void paintEvent (QPaintEvent* event) override;

private slots:

    void emitGeneratePressed ();

    void handleNextPressed ();

    void handlePreviousPressed ();

private:

    QLabel* locationNameLabel;

    QLabel* coordinateLabel;

    QPushButton* nextLocationButton;

    QPushButton* previousLocationButton;

    QPushButton* generateButton;

    uint16_t duration;

    class Location
    {
    public:
        Location (const std::string& locationName, double latCoord, double longCoord, uint16_t height,
                  const std::string& imgPath);

        double getLatitude () const;

        double getLongitude () const;

        uint16_t getHeight () const;

        std::string getName () const;

        static std::vector<Location> parseLocationList (std::string xmlLocationListPath);

    private:

        double latitude;

        double longitude;

        uint16_t height;

        std::string name;
    };

    size_t currentLocationIndex;

    std::vector<QGpsSimButton::Location> locationList;

    void updateLocation (const Location& newLocation);
};

#endif //QGPSSIMBUTTON_HPP
