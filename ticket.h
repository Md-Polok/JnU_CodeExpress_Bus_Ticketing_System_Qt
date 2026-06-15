#ifndef TICKET_H
#define TICKET_H

#include <QString>

class Ticket {
private:
    QString ticketID;     // e.g., "TKT10001"
    QString passengerID;  // Links to Passenger
    QString scheduleID;   // Links to Schedule
    QString seatNumber;   // e.g., "A1"
    int finalPrice;       // Ticket price
    QString status;       // "Confirmed" or "Cancelled"

public:
    Ticket();
    Ticket(QString tID, QString pID, QString sID, QString seat, int price, QString stat = "Confirmed");

    // Getters
    QString getTicketID() const;
    QString getPassengerID() const;
    QString getScheduleID() const;
    QString getSeatNumber() const;
    int getFinalPrice() const;
    QString getStatus() const;

    void cancelTicket();

    // Pipe serialization matching your format
    QString toPipeString() const;
    static Ticket fromPipeString(const QString &line);
};

#endif // TICKET_H
