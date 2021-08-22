
#include <QVBoxLayout>
#include <QPainter>
#include <filesystem>
#include <QGpsSimButton.hpp>
#include <loguru.hpp>
#include <iostream>

QGpsSimButton::QGpsSimButton (QWidget* parent) : QWidget (parent), duration (400), currentLocationIndex (0)
{
    auto* vboxLayOut = new QVBoxLayout (this);

    // Add vertical spacer to make space for duration display.
//    vboxLayOut->addSpacerItem (new QSpacerItem (0, 7));

    // Set title.
    auto* title = new QLabel ("Generate Simulation File");
    title->setFont (QFont ("Ubuntu Mono", 18, QFont::Bold));
    title->setAlignment (Qt::AlignCenter);
    vboxLayOut->addWidget (title);

    // Initialize and setup widgets in tile area.
    auto* hBoxContainer = new QWidget ();
    auto* hBoxLayout = new QHBoxLayout (hBoxContainer);

    previousLocationButton = new QPushButton ("Previous");
    hBoxLayout->addWidget (previousLocationButton);

    locationNameLabel = new QLabel ("");
    locationNameLabel->setAlignment (Qt::AlignCenter);
    hBoxLayout->addWidget (locationNameLabel);

    nextLocationButton = new QPushButton ("Next");
    hBoxLayout->addWidget (nextLocationButton);

    vboxLayOut->addWidget (hBoxContainer);

    coordinateLabel = new QLabel ("");
    coordinateLabel->setAlignment (Qt::AlignCenter);
    vboxLayOut->addWidget (coordinateLabel);

    generateButton = new QPushButton ("Generate");
    vboxLayOut->addWidget (generateButton);

    std::string homeDir = getenv ("HOME");
    locationList = Location::parseLocationList (homeDir + "/locations.conf");
    updateLocation (locationList[0]);

    // Register callbacks for the buttons.
    connect (generateButton, &QPushButton::released, this, &QGpsSimButton::emitGeneratePressed);
    connect (nextLocationButton, &QPushButton::released, this, &QGpsSimButton::handleNextPressed);
    connect (previousLocationButton, &QPushButton::released, this, &QGpsSimButton::handlePreviousPressed);
}

QGpsSimButton::~QGpsSimButton ()
{
    delete locationNameLabel;
    delete coordinateLabel;
    delete nextLocationButton;
    delete previousLocationButton;
    delete generateButton;
}

void QGpsSimButton::paintEvent (QPaintEvent* event)
{
    QWidget::paintEvent (event);
    QPainter painter (this);

    // Display duration.
    painter.drawText (7, 15, QString::fromStdString ("← Sim Duration: " + std::to_string (getDuration ())));
}

void QGpsSimButton::setDuration (uint16_t simFileDuration)
{
    duration = simFileDuration;
    update ();
}

uint16_t QGpsSimButton::getDuration () const
{
    return duration;
}

double QGpsSimButton::getLatitude () const
{
    return locationList[currentLocationIndex].getLatitude ();
}

double QGpsSimButton::getLongitude () const
{
    return locationList[currentLocationIndex].getLongitude ();
}

uint16_t QGpsSimButton::getHeight () const
{
    return locationList[currentLocationIndex].getHeight ();
}

void QGpsSimButton::updateLocation (const QGpsSimButton::Location& newLocation)
{
    locationNameLabel->setText (QString::fromStdString (newLocation.getName ()));
    coordinateLabel->setText (QString::fromStdString (
        std::to_string (newLocation.getLatitude ()) + "," +
        std::to_string (newLocation.getLongitude ()) + "," +
        std::to_string (newLocation.getHeight ())));
}

void QGpsSimButton::handleNextPressed ()
{
    if (currentLocationIndex == locationList.size () - 1)
    {
        currentLocationIndex = 0;
    } else
    {
        currentLocationIndex++;
    }
    updateLocation (locationList[currentLocationIndex]);
}

void QGpsSimButton::handlePreviousPressed ()
{
    if (currentLocationIndex == 0)
    {
        currentLocationIndex = locationList.size () - 1;
    } else
    {
        currentLocationIndex--;
    }
    updateLocation (locationList[currentLocationIndex]);
}

void QGpsSimButton::emitGeneratePressed ()
{
    emit generatePressed ();
}

QGpsSimButton::Location::Location (const std::string& locationName, double latCoord, double longCoord,
                                   uint16_t heightCoord, const std::string& imgPath)
{
    name = locationName;
    latitude = latCoord;
    longitude = longCoord;
    height = heightCoord;
}

double QGpsSimButton::Location::getLatitude () const
{
    return latitude;
}

double QGpsSimButton::Location::getLongitude () const
{
    return longitude;
}

uint16_t QGpsSimButton::Location::getHeight () const
{
    return height;
}

std::string QGpsSimButton::Location::getName () const
{
    return name;
}

std::vector<QGpsSimButton::Location> QGpsSimButton::Location::parseLocationList (std::string xmlLocationListPath)
{
    LOG_F (INFO, "Parsing location configuration file `%s`", xmlLocationListPath.c_str ());
    if (!std::filesystem::exists (xmlLocationListPath))
    {
        LOG_F (WARNING, "Location configuration file `%s` not found. Using default location list",
               xmlLocationListPath.c_str ());
        return { QGpsSimButton::Location ("Houston Radar", 29.638147, -95.599655, 50, ""),
                 QGpsSimButton::Location ("Moscow", 55.751760, 37.617579, 130, "") };
    }

    return std::vector<Location> ();
}
