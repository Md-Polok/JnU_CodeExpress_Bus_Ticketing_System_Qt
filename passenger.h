#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>
#include <QStringList>

class Passenger {
private:
    QString passengerID;
    QString name;
    QString idNumber;
    QString contact;
    QString email;
    QString subType; // The new 6th variable for VIP/Standard

public:
    // Constructors
    Passenger();
    Passenger(QString pID, QString pName, QString pIDNum, QString pContact, QString pEmail, QString pSubType);

    // Getters
    QString getPassengerID() const;
    QString getName() const;
    QString getIdNumber() const;
    QString getContact() const;
    QString getEmail() const;
    QString getSubType() const;

    // Data Serialization Helpers
    QString toPipeString() const;
    static Passenger fromPipeString(const QString &line);
};

#endif // PASSENGER_H