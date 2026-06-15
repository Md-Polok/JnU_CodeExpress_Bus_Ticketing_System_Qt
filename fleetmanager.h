#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

#include <QVector>
#include "bus.h"
#include "route.h"
#include "schedule.h"

class FleetManager {
private:
    QVector<Bus> buses;
    QVector<Route> routes;
    QVector<Schedule> schedules;

public:
    FleetManager();

    // File Input/Output Systems
    void loadFleetData();
    void saveFleetData();

    // Data Management Accessors
    void addBus(const Bus &b);
    void addRoute(const Route &r);
    void addSchedule(const Schedule &s);

    bool removeBusByID(const QString &busID);
    bool removeScheduleByID(const QString &schID);
    bool removeRouteByID(const QString &routeID);

    QVector<Bus> getBuses() const;
    QVector<Route> getRoutes() const;
    QVector<Schedule> getSchedules() const;

    // Cross-referencing lookup utilities
    Schedule* findScheduleByID(const QString &id);
    Bus* findBusByNum(const QString &busNum);
};

#endif // FLEETMANAGER_H
