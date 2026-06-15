#ifndef BUS_H
#define BUS_H

#include <QString>

class Bus {
private:
    QString busNumber; // e.g., "BD-1001"
    QString type;      // e.g., "AC" or "Non-AC"
    int capacity;      // e.g., 40 or 35
    QString status;    // e.g., "Active" or "Inactive"

public:
    // Constructors
    Bus();
    Bus(QString busNum, QString busType, int cap, QString stat);

    // Getters and Setters
    QString getBusNumber() const;
    QString getType() const;
    int getCapacity() const;
    QString getStatus() const;

    void setStatus(const QString &stat);

    // Data Serialization Helpers
    QString toPipeString() const;                     // Converts to "BD-1001|AC|40|Active"
    static Bus fromPipeString(const QString &line);   // Parses back into a Bus object
};

#endif // BUS_H
