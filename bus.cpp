#include "bus.h"
#include <QStringList>

// Default Constructor
Bus::Bus() {
    busNumber = "";
    type = "";
    capacity = 0;
    status = "Inactive";
}

// Parameterized Constructor
Bus::Bus(QString busNum, QString busType, int cap, QString stat) {
    busNumber = busNum;
    type = busType;
    capacity = cap;
    status = stat;
}

// Getters and Setters
QString Bus::getBusNumber() const { return busNumber; }
QString Bus::getType() const { return type; }
int Bus::getCapacity() const { return capacity; }
QString Bus::getStatus() const { return status; }

void Bus::setStatus(const QString &stat) { status = stat; }

// Convert object data to a single pipe-delimited text line
QString Bus::toPipeString() const {
    return busNumber + "|" + type + "|" + QString::number(capacity) + "|" + status;
}

// Parse a pipe-delimited text line and reconstruct a Bus object
Bus Bus::fromPipeString(const QString &line) {
    QStringList tokens = line.split('|');

    if (tokens.size() < 4) {
        return Bus();
    }

    QString busNum = tokens[0];
    QString busType = tokens[1];
    int cap = tokens[2].toInt();
    QString stat = tokens[3];

    return Bus(busNum, busType, cap, stat);
}
