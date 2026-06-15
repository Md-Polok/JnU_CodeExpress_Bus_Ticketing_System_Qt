#ifndef BOOKINGMANAGER_H
#define BOOKINGMANAGER_H

#include <QVector>
#include "passenger.h"
#include "ticket.h"

class BookingManager {
private:
    QVector<Passenger> passengers;
    QVector<Ticket> tickets;

public:
    BookingManager();

    // File Input/Output Systems
    void loadBookingData();
    void saveBookingData();

    // Transaction Management Engines
    void registerPassenger(const Passenger &p);
    void issueTicket(const Ticket &t);
    bool cancelTicketByID(const QString &ticketID);
    bool removePassengerByID(const QString &passID);

    QVector<Passenger> getPassengers() const;
    QVector<Ticket> getTickets() const;

    // Search and verification tools
    Passenger* findPassengerByID(const QString &pID);
    Ticket* findTicketByID(const QString &tID);
    QVector<Ticket> getTicketsBySchedule(const QString &scheduleID) const;

    bool cancelTicket(const QString& tktID);
};

#endif // BOOKINGMANAGER_H
