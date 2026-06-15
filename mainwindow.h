#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fleetmanager.h"
#include "bookingmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Tab 1: Fleet slots
    void on_btnRegisterVehicle_clicked();
    void on_btnDecommissionBus_clicked();

    // Tab 2: Route & Schedule slots
    void on_btnSaveRoute_clicked();
    void on_btnAuthSchedule_clicked();
    void on_btnTermTrip_clicked();

    // Tab 3: Passenger slots
    void on_btnCommitRecord_clicked();
    void on_btnDeletePassenger_clicked(); // ADD THIS LINE

    // Tab 4: Ticketing slots
    void on_btnVoidTicket_clicked();
    void on_cmbTargetTrip_currentTextChanged(const QString &arg1);
    void handleSeatGridClick(); // Correct custom slot mapping layout

    void on_btnCancelTicket_clicked();

    void on_btnViewTicket_clicked();

    void on_dltRoute_clicked();

    void on_txtSearchDest_textChanged(const QString &arg1);

    void on_btnExecSearch_clicked();

private:
    Ui::MainWindow *ui;

    // Backend engines
    FleetManager fleet;
    BookingManager booking;

    QString selectedSeatNumber; // Stores currently clicked seat (e.g., "A1")

    // Helper functions to populate your tables and components
    void refreshAllTables();
    void refreshBusTable();
    void refreshRouteTable();
    void refreshPassengerTable();
    void refreshScheduleTable();
    void refreshPassengerDropdown();
    void refreshHistoryLogTable();
    void refreshRouteDropdown();
    void refreshScheduleDropdown();

    // Ticketing Desktop Initializers
    void populateTicketingDropdowns();
    void generateSeatGridMap(const QString &scheduleID);

    void populateScheduleDropdowns(); // Add this line!

    void on_tabWidget_currentChanged(int index);

    bool verifyAdmin();

};

#endif // MAINWINDOW_H