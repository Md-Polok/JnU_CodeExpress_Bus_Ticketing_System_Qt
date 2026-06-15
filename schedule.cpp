#include "schedule.h"
#include <QStringList>

// Default Constructor
Schedule::Schedule() {
    scheduleID = "";
    routeID = "";
    busNumber = "";
    date = "";
    time = "";
    fare = 0;
    availableSeats = 0;
}

// Parameterized Constructor
Schedule::Schedule(QString schID, QString rID, QString bNum, QString travelDate, QString travelTime, int ticketFare, int seatsLeft) {
    scheduleID = schID;
    routeID = rID;
    busNumber = bNum;
    date = travelDate;
    time = travelTime;
    fare = ticketFare;
    availableSeats = seatsLeft;
}

// Getters and Setters
QString Schedule::getScheduleID() const { return scheduleID; }
QString Schedule::getRouteID() const { return routeID; }
QString Schedule::getBusNumber() const { return busNumber; }
QString Schedule::getDate() const { return date; }
QString Schedule::getTime() const { return time; }
int Schedule::getFare() const { return fare; }
int Schedule::getAvailableSeats() const { return availableSeats; }

void Schedule::setAvailableSeats(int count) { availableSeats = count; }

// Convert object data to a single pipe-delimited text line
QString Schedule::toPipeString() const {
    return scheduleID + "|" +
           routeID + "|" +
           busNumber + "|" +
           date + "|" +
           time + "|" +
           QString::number(fare) + "|" +
           QString::number(availableSeats);
}

// Parse a pipe-delimited text line and reconstruct a Schedule object
Schedule Schedule::fromPipeString(const QString &line) {
    QStringList tokens = line.split('|');

    // Safety check: ensure all 7 components exist
    if (tokens.size() < 7) {
        return Schedule();
    }

    QString schID = tokens[0];
    QString rID = tokens[1];
    QString bNum = tokens[2];
    QString travelDate = tokens[3];
    QString travelTime = tokens[4];
    int ticketFare = tokens[5].toInt();
    int seatsLeft = tokens[6].toInt();

    return Schedule(schID, rID, bNum, travelDate, travelTime, ticketFare, seatsLeft);
}
