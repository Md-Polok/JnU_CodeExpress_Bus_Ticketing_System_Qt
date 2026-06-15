#include "bookingmanager.h"
#include <QFile>
#include <QTextStream>

BookingManager::BookingManager() {
    loadBookingData();
}

void BookingManager::registerPassenger(const Passenger &p) {
    passengers.append(p);
    saveBookingData();
}

void BookingManager::issueTicket(const Ticket &t) {
    tickets.append(t);
    saveBookingData();
}

bool BookingManager::cancelTicketByID(const QString &ticketID) {
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].getTicketID() == ticketID) {
            tickets[i].cancelTicket();
            saveBookingData();
            return true;
        }
    }
    return false;
}

QVector<Passenger> BookingManager::getPassengers() const {
    return passengers;
}

QVector<Ticket> BookingManager::getTickets() const {
    return tickets;
}

Passenger* BookingManager::findPassengerByID(const QString &pID) {
    for (int i = 0; i < passengers.size(); ++i) {
        if (passengers[i].getPassengerID() == pID) return &passengers[i];
    }
    return nullptr;
}

Ticket* BookingManager::findTicketByID(const QString &tID) {
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].getTicketID() == tID) return &tickets[i];
    }
    return nullptr;
}

// Find all tickets booked for a specific bus trip (critical for updating seat maps)
QVector<Ticket> BookingManager::getTicketsBySchedule(const QString &scheduleID) const {
    QVector<Ticket> match;
    for (const auto &t : tickets) {
        if (t.getScheduleID() == scheduleID && t.getStatus() == "Confirmed") {
            match.append(t);
        }
    }
    return match;
}

void BookingManager::saveBookingData() {
    // Save Passengers
    QFile filePassengers("passengers.txt");
    if (filePassengers.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&filePassengers);
        for (const auto &p : passengers) out << p.toPipeString() << "\n";
        filePassengers.close();
    }

    // Save Tickets
    QFile fileTickets("tickets.txt");
    if (fileTickets.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileTickets);
        for (const auto &t : tickets) out << t.toPipeString() << "\n";
        fileTickets.close();
    }
}

void BookingManager::loadBookingData() {
    passengers.clear();
    tickets.clear();
    QString line;

    // Load Passengers
    QFile filePassengers("passengers.txt");
    if (filePassengers.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&filePassengers);
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.isEmpty()) passengers.append(Passenger::fromPipeString(line));
        }
        filePassengers.close();
    }

    // Load Tickets
    QFile fileTickets("tickets.txt");
    if (fileTickets.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileTickets);
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.isEmpty()) tickets.append(Ticket::fromPipeString(line));
        }
        fileTickets.close();
    }
}

bool BookingManager::cancelTicket(const QString& tktID) {
    for (auto it = tickets.begin(); it != tickets.end(); ++it) {
        if (it->getTicketID() == tktID) {
            tickets.erase(it); // Remove the ticket from memory
            saveBookingData(); // Save the updated list to your text file
            return true;
        }
    }
    return false; // Ticket not found
}

bool BookingManager::removePassengerByID(const QString &passID) {
    for (int i = 0; i < passengers.size(); ++i) {
        if (passengers[i].getPassengerID() == passID) {
            // 1. Remove from active memory
            passengers.removeAt(i);

            // 2. Overwrite the physical text file to make it permanent!
            // IMPORTANT: Change "passengers.txt" to whatever your actual text file is named!
            QFile file("passengers.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (const Passenger &p : passengers) {
                    out << p.toPipeString() << "\n";
                }
                file.close();
            }

            return true;
        }
    }
    return false; // Passenger not found
}
