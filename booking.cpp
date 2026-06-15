#include "booking.h"
#include <QStringList>

// Default Constructor
Booking::Booking() {
    bookingRef = "";
    passengerID = "";
    scheduleID = "";
    seatNumber = "";
    bookingDate = "";
    status = "Confirmed";
}

// Parameterized Constructor
Booking::Booking(QString ref, QString pID, QString sID, QString seat, QString date, QString stat) {
    bookingRef = ref;
    passengerID = pID;
    scheduleID = sID;
    seatNumber = seat;
    bookingDate = date;
    status = stat;
}

// Getters
QString Booking::getBookingRef() const { return bookingRef; }
QString Booking::getPassengerID() const { return passengerID; }
QString Booking::getScheduleID() const { return scheduleID; }
QString Booking::getSeatNumber() const { return seatNumber; }
QString Booking::getBookingDate() const { return bookingDate; }
QString Booking::getStatus() const { return status; }

// Cancel the booking
void Booking::cancel() {
    status = "Cancelled";
}

// Convert object data to a single pipe-delimited text line
QString Booking::toPipeString() const {
    return bookingRef + "|" +
           passengerID + "|" +
           scheduleID + "|" +
           seatNumber + "|" +
           bookingDate + "|" +
           status;
}

// Parse a pipe-delimited text line and reconstruct a Booking object
Booking Booking::fromPipeString(const QString &line) {
    QStringList tokens = line.split('|');

    if (tokens.size() < 6) {
        return Booking();
    }

    QString ref = tokens[0];
    QString pID = tokens[1];
    QString sID = tokens[2];
    QString seat = tokens[3];
    QString date = tokens[4];
    QString stat = tokens[5];

    return Booking(ref, pID, sID, seat, date, stat);
}
