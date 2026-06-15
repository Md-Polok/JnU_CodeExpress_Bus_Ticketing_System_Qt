#include "passenger.h"
#include <QStringList>

// Default Constructor
Passenger::Passenger() {
    passengerID = "";
    name = "";
    idNumber = "";
    contact = "";
    email = "";
}

// Parameterized Constructor
Passenger::Passenger(QString pID, QString pName, QString pIDNum, QString pContact, QString pEmail, QString pSubType) {
    passengerID = pID;
    name = pName;
    idNumber = pIDNum;
    contact = pContact;
    email = pEmail;
    subType = pSubType; // Save the VIP/Standard status!
}

// Getters
QString Passenger::getPassengerID() const { return passengerID; }
QString Passenger::getName() const { return name; }
QString Passenger::getIdNumber() const { return idNumber; }
QString Passenger::getContact() const { return contact; }
QString Passenger::getEmail() const { return email; }

// Convert object data to a single pipe-delimited text line
QString Passenger::toPipeString() const {
    // Simply added the subType to the end of the string chain
    return passengerID + "|" + name + "|" + idNumber + "|" + contact + "|" + email + "|" + subType;
}

// Parse a pipe-delimited text line and reconstruct a Passenger object
Passenger Passenger::fromPipeString(const QString &line) {
    QStringList tokens = line.split('|');

    // We still check for at least 5 to handle your older saved files
    if (tokens.size() < 5) {
        return Passenger();
    }

    QString pID = tokens[0];
    QString pName = tokens[1];
    QString pIDNum = tokens[2];
    QString pContact = tokens[3];
    QString pEmail = tokens[4];

    // Safely extract the 6th token (SubType).
    // If the text file has an old 5-item row, default it to "Standard" to prevent a crash.
    QString pSubType = (tokens.size() >= 6) ? tokens[5] : "Standard";

    // Pass the new 6th argument to your updated constructor
    return Passenger(pID, pName, pIDNum, pContact, pEmail, pSubType);
}

QString Passenger::getSubType() const {
    return subType;
}
