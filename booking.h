#ifndef BOOKING_H
#define BOOKING_H

#include <QString>

class Booking {
private:
    QString bookingRef;   // e.g., "BK9999999"
    QString passengerID;  // References Passenger::getPassengerID()
    QString scheduleID;   // References Schedule::getScheduleID()
    QString seatNumber;   // e.g., "B2"
    QString bookingDate;  // e.g., "17/06/2026"
    QString status;       // "Confirmed" or "Cancelled"

public:
    // Constructors
    Booking();
    Booking(QString ref, QString pID, QString sID, QString seat, QString date, QString stat = "Confirmed");

    // Getters
    QString getBookingRef() const;
    QString getPassengerID() const;
    QString getScheduleID() const;
    QString getSeatNumber() const;
    QString getBookingDate() const;
    QString getStatus() const;

    // Actions
    void cancel();        // Changes status to "Cancelled"

    // Data Serialization Helpers
    QString toPipeString() const;                       // Converts to "BK9999999|P-5001|SCH-999|B2|17/06/2026|Confirmed"
    static Booking fromPipeString(const QString &line); // Parses back into a Booking object

};

#endif // BOOKING_H
