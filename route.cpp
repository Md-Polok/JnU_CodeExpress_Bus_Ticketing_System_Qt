#include "route.h"
#include <QStringList>

// Default Constructor
Route::Route() {
    routeID = "";
    departureCity = "";
    arrivalCity = "";
    distanceKM = 0;
}

// Parameterized Constructor
Route::Route(QString id, QString from, QString to, int distance) {
    routeID = id;
    departureCity = from;
    arrivalCity = to;
    distanceKM = distance;
}

// Getters
QString Route::getRouteID() const { return routeID; }
QString Route::getDepartureCity() const { return departureCity; }
QString Route::getArrivalCity() const { return arrivalCity; }
int Route::getDistanceKM() const { return distanceKM; }

// Convert object data to a single pipe-delimited text line
QString Route::toPipeString() const {
    return routeID + "|" + departureCity + "|" + arrivalCity + "|" + QString::number(distanceKM);
}

// Parse a pipe-delimited text line and reconstruct a Route object
Route Route::fromPipeString(const QString &line) {
    // split() chops the string everywhere there is a '|' and puts it into a list
    QStringList tokens = line.split('|');

    // Safety check: ensure we have all 4 required fields
    if (tokens.size() < 4) {
        return Route();
    }

    QString id = tokens[0];
    QString from = tokens[1];
    QString to = tokens[2];
    int distance = tokens[3].toInt(); // Convert string to integer

    return Route(id, from, to, distance);
}
