#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QString>

class Schedule {
private:
    QString scheduleID;     // e.g., "SCH-999"
    QString routeID;        // References Route::getRouteID() -> e.g., "R-101"
    QString busNumber;      // References Bus::getBusNumber() -> e.g., "BD-1001"
    QString date;           // e.g., "17/07/2026"
    QString time;           // e.g., "8.30AM"
    int fare;               // e.g., 650
    int availableSeats;     // Starts at bus capacity, decreases as people book

public:
    // Constructors
    Schedule();
    Schedule(QString schID, QString rID, QString bNum, QString travelDate, QString travelTime, int ticketFare, int seatsLeft);

    // Getters and Setters
    QString getScheduleID() const;
    QString getRouteID() const;
    QString getBusNumber() const;
    QString getDate() const;
    QString getTime() const;
    int getFare() const;
    int getAvailableSeats() const;

    void setAvailableSeats(int count);

    // Data Serialization Helpers
    QString toPipeString() const;                         // Converts to "SCH-999|R-101|BD-1001|17/07/2026|8.30AM|650|11"
    static Schedule fromPipeString(const QString &line);  // Parses back into a Schedule object
};

#endif // SCHEDULE_H
