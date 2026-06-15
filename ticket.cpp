#include "ticket.h"
#include <QStringList>

Ticket::Ticket() {
    ticketID = ""; passengerID = ""; scheduleID = ""; seatNumber = ""; finalPrice = 0; status = "Confirmed";
}

Ticket::Ticket(QString tID, QString pID, QString sID, QString seat, int price, QString stat) {
    ticketID = tID; passengerID = pID; scheduleID = sID; seatNumber = seat; finalPrice = price; status = stat;
}

QString Ticket::getTicketID() const { return ticketID; }
QString Ticket::getPassengerID() const { return passengerID; }
QString Ticket::getScheduleID() const { return scheduleID; }
QString Ticket::getSeatNumber() const { return seatNumber; }
int Ticket::getFinalPrice() const { return finalPrice; }
QString Ticket::getStatus() const { return status; }

void Ticket::cancelTicket() { status = "Cancelled"; }

QString Ticket::toPipeString() const {
    return ticketID + "|" + passengerID + "|" + scheduleID + "|" + seatNumber + "|" + QString::number(finalPrice) + "|" + status;
}

Ticket Ticket::fromPipeString(const QString &line) {
    QStringList tokens = line.split('|');
    if (tokens.size() < 6) return Ticket();
    return Ticket(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4].toInt(), tokens[5]);
}
