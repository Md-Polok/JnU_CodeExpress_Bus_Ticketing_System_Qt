#ifndef ROUTE_H
#define ROUTE_H

#include <QString>

class Route {
private:
    QString routeID;
    QString departureCity;
    QString arrivalCity;
    int distanceKM;

public:
    // Constructors
    Route();
    Route(QString id, QString from, QString to, int distance);

    // Getters
    QString getRouteID() const;
    QString getDepartureCity() const;
    QString getArrivalCity() const;
    int getDistanceKM() const;

    // Data Serialization Helpers
    QString toPipeString() const;                      // Converts object to "R-101|Dhaka|Sylhet|240"
    static Route fromPipeString(const QString &line);  // Parses "R-101|Dhaka|Sylhet|240" back into an object
};

#endif // ROUTE_H
