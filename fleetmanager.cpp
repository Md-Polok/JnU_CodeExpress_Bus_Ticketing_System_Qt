#include "fleetmanager.h"
#include <QFile>
#include <QTextStream>

FleetManager::FleetManager() {
    loadFleetData();
}

void FleetManager::addBus(const Bus &b) { buses.append(b); saveFleetData(); }
void FleetManager::addRoute(const Route &r) { routes.append(r); saveFleetData(); }
void FleetManager::addSchedule(const Schedule &s) { schedules.append(s); saveFleetData(); }

QVector<Bus> FleetManager::getBuses() const { return buses; }
QVector<Route> FleetManager::getRoutes() const { return routes; }
QVector<Schedule> FleetManager::getSchedules() const { return schedules; }

Schedule* FleetManager::findScheduleByID(const QString &id) {
    for (int i = 0; i < schedules.size(); ++i) {
        if (schedules[i].getScheduleID() == id) return &schedules[i];
    }
    return nullptr;
}

Bus* FleetManager::findBusByNum(const QString &busNum) {
    for (int i = 0; i < buses.size(); ++i) {
        if (buses[i].getBusNumber() == busNum) return &buses[i];
    }
    return nullptr;
}

void FleetManager::saveFleetData() {
    // Save Buses
    QFile fileBuses("buses.txt");
    if (fileBuses.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileBuses);
        for (const auto &b : buses) out << b.toPipeString() << "\n";
        fileBuses.close();
    }

    // Save Routes
    QFile fileRoutes("routes.txt");
    if (fileRoutes.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileRoutes);
        for (const auto &r : routes) out << r.toPipeString() << "\n";
        fileRoutes.close();
    }

    // Save Schedules
    QFile fileSchedules("schedules.txt");
    if (fileSchedules.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileSchedules);
        for (const auto &s : schedules) out << s.toPipeString() << "\n";
        fileSchedules.close();
    }
}

void FleetManager::loadFleetData() {
    buses.clear(); routes.clear(); schedules.clear();
    QString line;

    // Load Buses
    QFile fileBuses("buses.txt");
    if (fileBuses.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileBuses);
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.isEmpty()) buses.append(Bus::fromPipeString(line));
        }
        fileBuses.close();
    }

    // Load Routes
    QFile fileRoutes("routes.txt");
    if (fileRoutes.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileRoutes);
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.isEmpty()) routes.append(Route::fromPipeString(line));
        }
        fileRoutes.close();
    }

    // Load Schedules
    QFile fileSchedules("schedules.txt");
    if (fileSchedules.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileSchedules);
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.isEmpty()) schedules.append(Schedule::fromPipeString(line));
        }
        fileSchedules.close();
    }
}

bool FleetManager::removeBusByID(const QString &busID) {
    for (int i = 0; i < buses.size(); ++i) {
        if (buses[i].getBusNumber() == busID) {
            buses.removeAt(i); // Removes the bus from the QVector
            return true;       // Successfully found and removed
        }
    }
    return false; // Bus not found
}

bool FleetManager::removeScheduleByID(const QString &schID) {
    for (int i = 0; i < schedules.size(); ++i) {
        if (schedules[i].getScheduleID() == schID) {
            schedules.removeAt(i); // Removes the schedule from the list
            saveFleetData();       // Save the updated list to your text file
            return true;
        }
    }
    return false; // Schedule not found
}

bool FleetManager::removeRouteByID(const QString &routeID) {
    // Note: Change 'routes' to whatever your QVector<Route> variable is named
    for (int i = 0; i < routes.size(); ++i) {
        if (routes[i].getRouteID() == routeID) {
            // 1. Remove the route from active memory
            routes.removeAt(i);

            // 2. Overwrite the physical text file to make the deletion permanent
            // IMPORTANT: Change "routes.txt" to your exact file name
            QFile file("routes.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (const Route &r : routes) {
                    out << r.toPipeString() << "\n";
                }
                file.close();
            }

            return true;
        }
    }
    return false; // Route not found
}
