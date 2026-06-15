#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QChar>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QInputDialog>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    selectedSeatNumber = "";
    refreshAllTables();
    populateTicketingDropdowns();
    populateScheduleDropdowns(); // Add this line!

    if (ui->mainTabWidget) {
        ui->mainTabWidget->setCurrentIndex(3);

        // 🔥 FOOLPROOF FIX: Explicitly force Qt to link the tab click to your security function
        connect(ui->mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_tabWidget_currentChanged);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==========================================
// TABLE REFRESH LOGIC
// ==========================================

void MainWindow::refreshAllTables() {
    refreshBusTable();
    refreshRouteTable();
    refreshPassengerTable();
    refreshScheduleTable(); // Add this line!
    refreshHistoryLogTable();
    refreshScheduleDropdown();
}

void MainWindow::refreshBusTable() {
    ui->tblBuses->setRowCount(0);
    auto buses = fleet.getBuses();
    for (int i = 0; i < buses.size(); ++i) {
        ui->tblBuses->insertRow(i);
        ui->tblBuses->setItem(i, 0, new QTableWidgetItem(buses[i].getBusNumber()));
        ui->tblBuses->setItem(i, 1, new QTableWidgetItem(buses[i].getType()));
        ui->tblBuses->setItem(i, 2, new QTableWidgetItem(QString::number(buses[i].getCapacity())));
    }
}

void MainWindow::refreshRouteTable() {
    ui->tblRoutes->setRowCount(0);
    auto routes = fleet.getRoutes();
    for (int i = 0; i < routes.size(); ++i) {
        ui->tblRoutes->insertRow(i);
        ui->tblRoutes->setItem(i, 0, new QTableWidgetItem(routes[i].getRouteID()));
        QString pathLayout = routes[i].getDepartureCity() + " - " + routes[i].getArrivalCity();
        ui->tblRoutes->setItem(i, 1, new QTableWidgetItem(pathLayout));
        ui->tblRoutes->setItem(i, 2, new QTableWidgetItem(QString::number(routes[i].getDistanceKM()) + " km"));
    }
}

void MainWindow::refreshPassengerTable() {
    ui->tblPassengers->setRowCount(0);
    auto passengers = booking.getPassengers();
    for (int i = 0; i < passengers.size(); ++i) {
        ui->tblPassengers->insertRow(i);
        ui->tblPassengers->setItem(i, 0, new QTableWidgetItem(passengers[i].getPassengerID()));
        ui->tblPassengers->setItem(i, 1, new QTableWidgetItem(passengers[i].getName()));
        ui->tblPassengers->setItem(i, 2, new QTableWidgetItem(passengers[i].getIdNumber()));

        // FIXED: Now it dynamically pulls "VIP" or "Standard" from the passenger object
        ui->tblPassengers->setItem(i, 3, new QTableWidgetItem(passengers[i].getSubType()));
    }
}

// ==========================================
// TAB 1: FLEET LOGISTICS
// ==========================================

void MainWindow::on_btnRegisterVehicle_clicked() {
    QString busNo = ui->txtBusNo->text();
    QString type = ui->cmbBusType->currentText();
    int capacity = ui->spinCapacity->value();

    if (busNo.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Bus Number cannot be empty!");
        return;
    }

    auto existingBuses = fleet.getBuses();
    for (int i = 0; i < existingBuses.size(); ++i) {
        if (existingBuses[i].getBusNumber() == busNo) {
            QMessageBox::warning(this, "Duplicate Entry", "A bus with ID '" + busNo + "' already exists! Please use a unique ID.");
            return; // Stop the function here so it doesn't save
        }
    }

    // Fixed: Added "Active" status argument to match your 4-parameter constructor
    Bus newBus(busNo, type, capacity, "Active");
    fleet.addBus(newBus);

    ui->txtBusNo->clear();
    ui->spinCapacity->setValue(40);

    refreshBusTable();
    QMessageBox::information(this, "Success", "Vehicle Registered Successfully!");

    populateScheduleDropdowns();
}

void MainWindow::on_btnDecommissionBus_clicked() {
    // NOTE: Replace 'ui->tblBuses' with the actual name of your QTableWidget in Qt Designer
    int selectedRow = ui->tblBuses->currentRow();

    // 1. Check if a row is actually clicked/highlighted
    if (selectedRow < 0) {
        QMessageBox::information(this, "Notice", "Select a bus from the table to decommission.");
        return;
    }

    // 2. Extract the Bus ID from Column 0 of the selected row
    QString targetBusID = ui->tblBuses->item(selectedRow, 0)->text();

    // 3. Optional but recommended: Ask for confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Decommission",
                                  "Are you sure you want to permanently decommission Bus: " + targetBusID + "?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    // 4. Remove the bus from your backend database
    // NOTE: You will need a function in your Fleet/Manager class that removes the bus by ID.
    if (fleet.removeBusByID(targetBusID)) {
        fleet.saveFleetData();      // Save the text file
        refreshBusTable();          // Redraw the table to make the row disappear
        QMessageBox::information(this, "Success", "Bus decommissioned successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to find the bus in the database.");
    }
}

// ==========================================
// TAB 2: SCHEDULES & ROUTES
// ==========================================

void MainWindow::on_btnSaveRoute_clicked() {
    QString routeID = ui->txtRouteID->text();
    QString depCity = ui->txtDepCity->text();
    QString arrCity = ui->txtArrCity->text();
    int distance = static_cast<int>(ui->spinDistance->value());

    if (routeID.trimmed().isEmpty() || depCity.trimmed().isEmpty() || arrCity.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all Route fields!");
        return;
    }

    Route newRoute(routeID, depCity, arrCity, distance);
    fleet.addRoute(newRoute);

    ui->txtRouteID->clear();
    ui->txtDepCity->clear();
    ui->txtArrCity->clear();
    ui->spinDistance->setValue(1.00);

    refreshRouteTable();
    QMessageBox::information(this, "Success", "Route Layout Saved!");

    populateScheduleDropdowns();
}

void MainWindow::on_btnAuthSchedule_clicked() {
    // 1. Read input fields (Removed the dangerous Extreme Fallback!)
    QLineEdit *schIdEdit = this->findChild<QLineEdit*>("txtSchID");
    if (!schIdEdit) schIdEdit = this->findChild<QLineEdit*>("txtScheduleID");
    if (!schIdEdit) schIdEdit = this->findChild<QLineEdit*>("txtScheduleNo");
    if (!schIdEdit) schIdEdit = this->findChild<QLineEdit*>("txtTripID");
    if (!schIdEdit) schIdEdit = this->findChild<QLineEdit*>("lineEditScheduleID");

    // If it still can't find it, or it's empty, schID becomes a blank string
    QString schID = schIdEdit ? schIdEdit->text().trimmed() : "";

    // 2. Extract values from dynamic UI dropdown elements
    QString rID = "";
    QComboBox *routeCombo = this->findChild<QComboBox*>("cmbTargetRoute");
    if (!routeCombo) routeCombo = this->findChild<QComboBox*>("cmbRoute");
    if (routeCombo) {
        rID = routeCombo->currentData().toString();
        if (rID.isEmpty()) {
            rID = routeCombo->currentText().split(" ").first().trimmed();
        }
    }

    QString bNum = "";
    QComboBox *busCombo = this->findChild<QComboBox*>("cmbAssignedBus");
    if (!busCombo) busCombo = this->findChild<QComboBox*>("cmbBus");
    if (busCombo) {
        bNum = busCombo->currentData().toString();
        if (bNum.isEmpty()) {
            bNum = busCombo->currentText().split(" ").first().trimmed();
        }
    }

    // 3. Absolute Rule Validation Check - This will NOW properly catch blank IDs!
    if (schID.isEmpty() || rID.isEmpty() || bNum.isEmpty()) {
        QMessageBox::warning(this, "Deployment Error", "Please provide a Schedule ID and select a Route and Vehicle!");
        return;
    }

    // 4. Extract configurations from secondary UI parameters safely
    QString travelDate = "Unknown Date";
    QDateTimeEdit *dateEdit = this->findChild<QDateTimeEdit*>("dateEditDeparture");
    if (!dateEdit) dateEdit = this->findChild<QDateTimeEdit*>("dateTimeEdit");
    if (!dateEdit) dateEdit = this->findChild<QDateTimeEdit*>("dateEdit");
    if (dateEdit) {
        travelDate = dateEdit->date().toString("dd/MM/yyyy");
    }

    QString travelTime = "Unknown Time";
    // Because QTimeEdit inherits from QDateTimeEdit, searching for the base class catches BOTH types!
    QDateTimeEdit *timeEdit = this->findChild<QDateTimeEdit*>("timeEditWindow");
    if (!timeEdit) timeEdit = this->findChild<QDateTimeEdit*>("timeEdit");

    if (timeEdit) {
        travelTime = timeEdit->time().toString("hh:mm AP");
    }

    int ticketFare = 650;
    QDoubleSpinBox *fareSpin = this->findChild<QDoubleSpinBox*>("spinBaseFare");
    if (!fareSpin) fareSpin = this->findChild<QDoubleSpinBox*>("spinFare");
    if (fareSpin) {
        ticketFare = static_cast<int>(fareSpin->value());
    }

    int baseSeats = 40;

    // Match bus capacity rules automatically
    auto buses = fleet.getBuses();
    for (const auto &bus : buses) {
        if (bus.getBusNumber() == bNum) {
            baseSeats = bus.getCapacity();
            break;
        }
    }

    // 5. Construct, Save, and Log Schedule dataset instance
    Schedule newSchedule(schID, rID, bNum, travelDate, travelTime, ticketFare, baseSeats);
    fleet.addSchedule(newSchedule);

    // 6. Clean UI Inputs
    if (schIdEdit) schIdEdit->clear();

    // 7. Refresh Tables and Synchronize data loops
    refreshScheduleTable();
    refreshScheduleDropdown();

    // (Assuming you have this function created somewhere else)
    // populateTicketingDropdowns();

    QMessageBox::information(this, "Success", "Intercity Transit Run Authorized and Deployed successfully!");
}

void MainWindow::refreshScheduleTable() {
    // 1. Locate the schedule table
    QTableWidget *schTable = this->findChild<QTableWidget*>("tblSchedules");
    if (!schTable) schTable = this->findChild<QTableWidget*>("tableWidget"); // Fallback

    if (!schTable) return; // Exit if table isn't found in UI

    // 2. Fetch data and prepare table
    auto schedules = fleet.getSchedules();
    schTable->setRowCount(0); // Wipe existing rows to redraw fresh

    // 3. Populate rows correctly
    for (int i = 0; i < schedules.size(); ++i) {
        schTable->insertRow(i);
        const Schedule &s = schedules[i];

        // Column 0: Trip ID
        schTable->setItem(i, 0, new QTableWidgetItem(s.getScheduleID()));

        // Column 1: Route Reference
        schTable->setItem(i, 1, new QTableWidgetItem(s.getRouteID()));

        // Column 2: Bus Allocation
        schTable->setItem(i, 2, new QTableWidgetItem(s.getBusNumber()));

        // Column 3: Time Window (Combining Date and Time)
        QString timeWindow = s.getDate() + " @ " + s.getTime();
        schTable->setItem(i, 3, new QTableWidgetItem(timeWindow));

        // Column 4: Available Seats (FIXED: Removing BDT and pulling getAvailableSeats)
        schTable->setItem(i, 4, new QTableWidgetItem(QString::number(s.getAvailableSeats()) + " Seats"));
    }
}

// ==========================================
// TAB 3: PASSENGER RELATIONSHIPS
// ==========================================

void MainWindow::on_btnCommitRecord_clicked() {
    QString passID = ui->txtPassID->text();
    QString name = ui->txtFullName->text();
    QString nid = ui->txtNID->text();
    QString contact = ui->txtContact->text();
    QString email = ui->txtEmail->text();

    // FIXED: Directly pulling from the UI pointer just like the text boxes!
    // **IMPORTANT**: If your combo box is named something else in Qt Designer, change 'cmbPassengerSubType' to match it.
    QString subType = ui->cmbPassSubType->currentText();

    if (passID.trimmed().isEmpty() || name.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Passenger ID and Name are required!");
        return;
    }

    Passenger newPass(passID, name, nid, contact, email, subType);
    booking.registerPassenger(newPass);

    ui->txtPassID->clear();
    ui->txtFullName->clear();
    ui->txtNID->clear();
    ui->txtContact->clear();
    ui->txtEmail->clear();

    refreshPassengerTable();
    QMessageBox::information(this, "Success", "Passenger Record Committed!");

    refreshPassengerDropdown();
}

// ==========================================
// TAB 4: TICKETING DESK DASHBOARD
// ==========================================

void MainWindow::populateTicketingDropdowns() {
    // 1. Target Trip Dropdown
    if (ui->cmbTargetTrip) {
        ui->cmbTargetTrip->clear();
        auto schedules = fleet.getSchedules();
        for (const auto &sch : schedules) {
            ui->cmbTargetTrip->addItem(sch.getScheduleID() + " (Bus: " + sch.getBusNumber() + ")", sch.getScheduleID());
        }
    }

    // 2. Booking Passenger Dropdown
    if (ui->cmbBookingPassenger) {
        ui->cmbBookingPassenger->clear();
        auto passengers = booking.getPassengers();
        for (const auto &p : passengers) {
            ui->cmbBookingPassenger->addItem(p.getPassengerID() + " - " + p.getName(), p.getPassengerID());
        }
    }
}

void MainWindow::generateSeatGridMap(const QString &scheduleID) {
    // Safety check: Finds layout grid regardless of if it is called laySeatGrid or if it's currently missing
    QGridLayout *grid = this->findChild<QGridLayout*>("laySeatGrid");
    if (!grid) return;

    QLayoutItem *child;
    while ((child = grid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (scheduleID.isEmpty()) return;

    auto activeTickets = booking.getTicketsBySchedule(scheduleID);
    QSet<QString> bookedSeats;
    for (const auto &t : activeTickets) {
        bookedSeats.insert(t.getSeatNumber());
    }

    char rowChar = 'A';
    int totalRows = 10;

    for (int r = 0; r < totalRows; ++r) {
        // Fixed syntax error: Converted mathematical offset cleanly using QChar wrapper
        QString rowLabel = QString(QChar(rowChar + r));

        for (int c = 1; c <= 4; ++c) {
            QString seatID = rowLabel + QString::number(c);
            QPushButton *seatButton = new QPushButton(seatID, this);
            seatButton->setFixedSize(50, 45);

            if (bookedSeats.contains(seatID)) {
                // Booked seats are Red and disabled
                seatButton->setStyleSheet("background-color: #EF4444; color: white; font-weight: bold; border-radius: 4px;");
                seatButton->setEnabled(false);
            } else if (seatID == selectedSeatNumber) {
                // NEW: The currently selected seat is Blue
                seatButton->setStyleSheet("background-color: #3B82F6; color: white; font-weight: bold; border: 2px solid #FFFFFF; border-radius: 4px;");
            } else {
                // Available seats are Orange (front) or Green (back)
                if (r < 2) {
                    seatButton->setStyleSheet("background-color: #F59E0B; color: black; font-weight: bold; border-radius: 4px;");
                } else {
                    seatButton->setStyleSheet("background-color: #10B981; color: white; font-weight: bold; border-radius: 4px;");
                }
            }

            // Fixed naming scheme link
            connect(seatButton, &QPushButton::clicked, this, &MainWindow::handleSeatGridClick);

            int layoutColumn = (c > 2) ? c + 1 : c;
            grid->addWidget(seatButton, r, layoutColumn);
        }
    }
}

void MainWindow::on_cmbTargetTrip_currentTextChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    if (!ui->cmbTargetTrip) return;
    QString currentScheduleID = ui->cmbTargetTrip->currentData().toString();
    generateSeatGridMap(currentScheduleID);
    selectedSeatNumber = "";

    QLabel *displayLbl = this->findChild<QLabel*>("lblSelectedSeatDisplay");
    if (displayLbl) displayLbl->setText("None Selected");
}

void MainWindow::handleSeatGridClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // Save the seat number globally
    selectedSeatNumber = clickedButton->text();

    // Update the text label
    QLabel *displayLbl = this->findChild<QLabel*>("lblSelectedSeatDisplay");
    if (displayLbl) displayLbl->setText(selectedSeatNumber);

    // Redraw the grid (this will safely highlight the new selection)
    if (ui->cmbTargetTrip) {
        QString currentScheduleID = ui->cmbTargetTrip->currentData().toString();
        generateSeatGridMap(currentScheduleID);
    }
}

void MainWindow::on_btnVoidTicket_clicked() {
    if (!ui->cmbTargetTrip) return;

    QString schID = ui->cmbTargetTrip->currentData().toString();
    QString passID = "";

    if (ui->cmbBookingPassenger) passID = ui->cmbBookingPassenger->currentData().toString();

    if (schID.isEmpty() || passID.isEmpty() || selectedSeatNumber.isEmpty()) {
        QMessageBox::warning(this, "Ticketing Desk Error", "Ensure Trip, Passenger, and Seat coordinates are selected!");
        return;
    }

    // --- NEW VIP SECURITY CHECKPOINT ---
    // 1. Check if the selected seat is in the VIP rows (A or B)
    bool isVIPSeat = selectedSeatNumber.startsWith("A") || selectedSeatNumber.startsWith("B");

    // 2. Fetch the passenger profile
    Passenger* bookingPassenger = booking.findPassengerByID(passID);

    if (bookingPassenger) {
        // 3. Get classification. (Ensure 'getSubType()' exactly matches the method name in passenger.h)
        QString passType = bookingPassenger->getSubType();

        // 4. Reject if standard passenger tries to book VIP
        if (isVIPSeat && passType == "Standard") {
            QMessageBox::critical(this, "Access Denied",
                                  "Standard tier passengers are not permitted to book VIP Seating (Rows A & B).\n\n"
                                  "Please select an Economy seat or upgrade the passenger profile.");
            return; // Instantly aborts the booking
        }
    }
    // --- END VIP SECURITY CHECKPOINT ---

    QString generatedTktID = "TKT" + QString::number(10000 + booking.getTickets().size() + 1);

    int tripFareValue = 0;
    QString tripDate = "";
    QString tripTime = "";
    QString routeID = "";

    // Issue the ticket and save it
    Ticket printedTicket(generatedTktID, passID, schID, selectedSeatNumber, tripFareValue, "Confirmed");
    booking.issueTicket(printedTicket);

    // Deduct the seat from the Fleet Schedule and extract details for the receipt
    Schedule* activeSch = fleet.findScheduleByID(schID);
    if (activeSch) {
        tripFareValue = activeSch->getFare();
        tripDate = activeSch->getDate();
        tripTime = activeSch->getTime();
        routeID = activeSch->getRouteID();

        int currentSeats = activeSch->getAvailableSeats();
        if (currentSeats > 0) {
            activeSch->setAvailableSeats(currentSeats - 1);
            fleet.saveFleetData();      // Save the new count to schedules.txt
            refreshScheduleTable();     // Update the visual table instantly
        }
    }

    // --- LOGIC: GENERATE TEXT FILE AND DISPLAY RECEIPT ---
    QString receiptFilename = generatedTktID + "_Receipt.txt";

    // 1. Extract the passenger name cleanly
    QString passText = ui->cmbBookingPassenger->currentText();
    QString passName = passText.contains("-") ? passText.split("-")[1].trimmed() : passID;

    // 2. Build the receipt as a single massive string
    QString receiptString =
        "========================================\n"
        "            INTERCITY TRANSIT           \n"
        "             BOARDING PASS              \n"
        "========================================\n"
        "Ticket No   : " + generatedTktID + "\n"
                           "Date Issued : " + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm AP") + "\n"
                                                                         "----------------------------------------\n"
                                                                         "Passenger   : " + passName + " (ID: " + passID + ")\n"
                                         "Route ID    : " + routeID + "\n"
                    "Schedule ID : " + schID + "\n"
                  "Departure   : " + tripDate + " @ " + tripTime + "\n"
                                        "Seat No     : " + selectedSeatNumber + "\n"
                               "Total Fare  : " + QString::number(tripFareValue) + " BDT\n"
                                           "Status      : CONFIRMED\n"
                                           "========================================\n"
                                           "        Thank you for traveling!        \n"
                                           "========================================\n";

    // 3. Save it to the text file
    QFile receiptFile(receiptFilename);
    if (receiptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&receiptFile);
        out << receiptString;
        receiptFile.close();
    }

    // 4. Reset UI selections
    generateSeatGridMap(schID);
    selectedSeatNumber = "";
    QLabel *displayLbl = this->findChild<QLabel*>("lblSelectedSeatDisplay");
    if (displayLbl) displayLbl->setText("None Selected");

    refreshHistoryLogTable();

    // 5. Display the ticket directly on the screen using HTML formatting!
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Boarding Pass - " + generatedTktID);
    msgBox.setText("<b>Ticket Successfully Issued!</b><br>A copy has been saved as <i>" + receiptFilename + "</i>");
    msgBox.setInformativeText("<pre>" + receiptString + "</pre>");
    msgBox.setStyleSheet("QLabel { min-width: 350px; }"); // Makes the box wide enough to look good
    msgBox.exec();
}

void MainWindow::populateScheduleDropdowns() {
    // 1. Find and populate the Route Path Dropdown safely
    // Tries to look for common name variants like cmbTargetRoute or cmbRoute
    QComboBox *routeCombo = this->findChild<QComboBox*>("cmbTargetRoute");
    if (!routeCombo) routeCombo = this->findChild<QComboBox*>("cmbRoute");

    if (routeCombo) {
        routeCombo->clear();
        auto routes = fleet.getRoutes();
        for (const auto &r : routes) {
            // Displays: "R-101 (Dhaka - Sylhet)" and saves "R-101" as hidden backend data
            QString displayStr = r.getRouteID() + " (" + r.getDepartureCity() + " - " + r.getArrivalCity() + ")";
            routeCombo->addItem(displayStr, r.getRouteID());
        }
    }

    // 2. Find and populate the Assigned Bus Dropdown safely
    // Tries to look for common name variants like cmbAssignedBus or cmbBus
    QComboBox *busCombo = this->findChild<QComboBox*>("cmbAssignedBus");
    if (!busCombo) busCombo = this->findChild<QComboBox*>("cmbBus");

    if (busCombo) {
        busCombo->clear();
        auto buses = fleet.getBuses();
        for (const auto &b : buses) {
            // Displays: "BD-1001 [AC - Cap: 40]" and saves "BD-1001" as hidden backend data
            QString displayStr = b.getBusNumber() + " [" + b.getType() + " - Cap: " + QString::number(b.getCapacity()) + "]";
            busCombo->addItem(displayStr, b.getBusNumber());
        }
    }
}

void MainWindow::on_btnCancelTicket_clicked() {
    // 1. Ask the user for the Ticket ID via a clean popup
    bool ok;
    QString tktID = QInputDialog::getText(this, "Cancel Ticket",
                                          "Enter Ticket ID to cancel (e.g., TKT10001):",
                                          QLineEdit::Normal, "", &ok);

    // If user clicks Cancel on the popup or leaves it blank, just exit safely
    if (!ok || tktID.trimmed().isEmpty()) return;

    tktID = tktID.trimmed().toUpper(); // Ensure standard formatting

    // 2. Find the ticket details BEFORE deleting it (we need to know which schedule to refund the seat to)
    QString targetScheduleID = "";
    bool ticketFound = false;

    auto tickets = booking.getTickets();
    for (const auto &t : tickets) {
        if (t.getTicketID() == tktID) {
            targetScheduleID = t.getScheduleID();
            ticketFound = true;
            break;
        }
    }

    if (!ticketFound) {
        QMessageBox::warning(this, "Error", "Ticket ID " + tktID + " not found in the system!");
        return;
    }

    // 3. Remove the ticket from the backend booking system
    if (booking.cancelTicketByID(tktID)) {

        // 4. Refund the seat back to the Fleet Schedule
        Schedule* activeSch = fleet.findScheduleByID(targetScheduleID);
        if (activeSch) {
            int currentSeats = activeSch->getAvailableSeats();
            activeSch->setAvailableSeats(currentSeats + 1); // Add the seat back!
            fleet.saveFleetData();                          // Save schedule changes to file
            refreshScheduleTable();                         // Update UI table
        }

        // 5. Redraw the visual seat grid if we are currently looking at the affected trip
        if (ui->cmbTargetTrip && ui->cmbTargetTrip->currentData().toString() == targetScheduleID) {
            generateSeatGridMap(targetScheduleID);
        }

        refreshHistoryLogTable();

        QMessageBox::information(this, "Refund Success", "Ticket " + tktID + " has been successfully cancelled and the seat has been freed.");
    } else {
        QMessageBox::critical(this, "Critical Error", "Failed to cancel the ticket. Database mismatch.");
    }
}

void MainWindow::on_btnTermTrip_clicked() {
    // 1. Find your Schedule Table (Adjust the name in quotes if your Qt Designer name is different)
    QTableWidget *schTable = this->findChild<QTableWidget*>("tblSchedules");
    if (!schTable) schTable = this->findChild<QTableWidget*>("tableWidget"); // Fallback

    if (!schTable) {
        QMessageBox::critical(this, "System Error", "Could not locate the schedule table UI.");
        return;
    }

    // 2. Check if a row is selected
    int selectedRow = schTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::information(this, "Notice", "Select a trip from the table to terminate.");
        return;
    }

    // 3. Extract the Schedule ID from Column 0 of the selected row
    QString targetSchID = schTable->item(selectedRow, 0)->text();

    // 4. Ask for confirmation before deleting
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Termination",
                                  "Are you sure you want to permanently terminate Trip: " + targetSchID + "?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    // 5. Remove from backend and refresh
    if (fleet.removeScheduleByID(targetSchID)) {
        refreshScheduleTable(); // Redraws the table so the row vanishes
        QMessageBox::information(this, "Success", "Trip assignment terminated successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to find the trip in the database.");
    }
    refreshScheduleDropdown();
}

void MainWindow::on_btnDeletePassenger_clicked() {
    QTableWidget *passTable = this->findChild<QTableWidget*>("tblPassengers");
    if (!passTable) passTable = this->findChild<QTableWidget*>("tableWidget_2");
    if (!passTable) return;

    int selectedRow = passTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::information(this, "Notice", "Select a passenger from the table to delete.");
        return;
    }

    // Call our new custom dialog!
    if (!verifyAdmin()) {
        return; // Stops deletion instantly if they fail or cancel
    }

    QString targetPassID = passTable->item(selectedRow, 0)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Deletion",
                                                              "Permanently delete passenger record: " + targetPassID + "?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    if (booking.removePassengerByID(targetPassID)) {
        refreshPassengerTable();
        QMessageBox::information(this, "Success", "Passenger deleted successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to locate passenger in the database.");
    }

    refreshPassengerDropdown();
}

void MainWindow::refreshPassengerDropdown() {
    // IMPORTANT: Check Qt Designer and change 'cmbBookingPassenger' if your box is named differently!
    ui->cmbBookingPassenger->clear();

    auto passengers = booking.getPassengers();
    for (const auto &p : passengers) {
        // Formats the text exactly like your screenshot: "P_001 - Mr. Pial"
        QString displayText = p.getPassengerID() + " - " + p.getName();

        // We add the text, and secretly store the ID as 'itemData' so you can easily grab it later when booking!
        ui->cmbBookingPassenger->addItem(displayText, p.getPassengerID());
    }
}

void MainWindow::refreshHistoryLogTable() {
    // 1. Wipe the visual table completely clean
    ui->tblTicketHistory->setRowCount(0);

    // 2. Fetch the most up-to-date data from memory
    auto tickets = booking.getTickets();

    // 3. Rebuild the table row by row
    for (int i = 0; i < tickets.size(); ++i) {
        // Skip cancelled tickets so they don't appear in the active log
        if (tickets[i].getStatus() == "Cancelled") continue;

        int row = ui->tblTicketHistory->rowCount();
        ui->tblTicketHistory->insertRow(row);

        QString pName = tickets[i].getPassengerID();
        Passenger* p = booking.findPassengerByID(tickets[i].getPassengerID());
        if (p) pName = p->getName();

        ui->tblTicketHistory->setItem(row, 0, new QTableWidgetItem(tickets[i].getTicketID()));
        ui->tblTicketHistory->setItem(row, 1, new QTableWidgetItem(pName));
        ui->tblTicketHistory->setItem(row, 2, new QTableWidgetItem(tickets[i].getScheduleID()));
        ui->tblTicketHistory->setItem(row, 3, new QTableWidgetItem(tickets[i].getSeatNumber()));
    }
}

void MainWindow::on_btnViewTicket_clicked() {
    // 1. Check which row is currently highlighted in the table
    int row = ui->tblTicketHistory->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a ticket from the history log first!");
        return;
    }

    // 2. Extract the Ticket ID from the first column (Column 0) of that highlighted row
    QString targetTktID = ui->tblTicketHistory->item(row, 0)->text();

    // 3. Dig into the backend to find the matching Ticket, Passenger, and Schedule
    Ticket* targetTicket = booking.findTicketByID(targetTktID);
    if (!targetTicket) return; // Failsafe just in case

    Passenger* pass = booking.findPassengerByID(targetTicket->getPassengerID());
    QString passName = pass ? pass->getName() : "Unknown Passenger";

    Schedule* activeSch = fleet.findScheduleByID(targetTicket->getScheduleID());
    QString routeID = activeSch ? activeSch->getRouteID() : "Unknown Route";
    QString tripDate = activeSch ? activeSch->getDate() : "Unknown Date";
    QString tripTime = activeSch ? activeSch->getTime() : "Unknown Time";

    // 4. Rebuild the exact receipt string from your booking function
    QString receiptString =
        "========================================\n"
        "            INTERCITY TRANSIT           \n"
        "             BOARDING PASS              \n"
        "========================================\n"
        "Ticket No   : " + targetTicket->getTicketID() + "\n"
                                        "Date Printed: " + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm AP") + "\n"
                                                                         "----------------------------------------\n"
                                                                         "Passenger   : " + passName + " (ID: " + targetTicket->getPassengerID() + ")\n"
                                                                 "Route ID    : " + routeID + "\n"
                    "Schedule ID : " + targetTicket->getScheduleID() + "\n"
                                          "Departure   : " + tripDate + " @ " + tripTime + "\n"
                                        "Seat No     : " + targetTicket->getSeatNumber() + "\n"
                                          "Total Fare  : " + QString::number(targetTicket->getFinalPrice()) + " BDT\n"
                                                           "Status      : " + targetTicket->getStatus().toUpper() + "\n"
                                                "========================================\n"
                                                "        Thank you for traveling!        \n"
                                                "========================================\n";

    // 5. Fire the exact same MessageBox pop-up!
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Ticket Viewer - " + targetTicket->getTicketID());
    msgBox.setText("<b>Boarding Pass Copy</b>");
    msgBox.setInformativeText("<pre>" + receiptString + "</pre>");
    msgBox.setStyleSheet("QLabel { min-width: 350px; }");
    msgBox.exec();
}

void MainWindow::on_dltRoute_clicked() {
    // 1. Get the currently selected row from your Route Table
    // IMPORTANT: Replace 'tblRoutes' with the actual objectName of your route table!
    int row = ui->tblRoutes->currentRow();

    // 2. Make sure the user actually clicked a row before hitting delete
    if (row < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a route from the table first!");
        return;
    }

    // 3. Extract the Route ID from the first column (Column 0)
    QString targetRouteID = ui->tblRoutes->item(row, 0)->text();

    // 4. (Optional but recommended) Ask for confirmation before deleting
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion",
                                  "Are you sure you want to permanently delete Route: " + targetRouteID + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) return;

    // 5. Call the backend function to delete it and save the file
    if (fleet.removeRouteByID(targetRouteID)) {

        // Refresh the visual table so the row disappears
        refreshRouteTable(); // Replace with your actual table refresh function name

        QMessageBox::information(this, "Success", "Route deleted successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Could not delete the route from the system.");
    }
}

void MainWindow::refreshScheduleDropdown() {
    // FIXED: Changed from ui->cmbTargetRoute to ui->cmbTargetTrip to correctly target the Ticketing tab
    QComboBox *scheduleBox = ui->cmbTargetTrip;
    if (!scheduleBox) return;

    scheduleBox->clear(); // Wipe the old list

    // Fetch the active schedules from your backend manager
    auto schedules = fleet.getSchedules();

    for (int i = 0; i < schedules.size(); ++i) {
        // Format to match your screenshot: "Sch_001 (Bus: B_001)"
        QString displayText = schedules[i].getScheduleID() + " (Bus: " + schedules[i].getBusNumber() + ")";

        // Add to combobox: Display text goes to user, Schedule ID goes into hidden background data
        scheduleBox->addItem(displayText, schedules[i].getScheduleID());
    }
}

void MainWindow::on_tabWidget_currentChanged(int index) {
    static int previousSafeTab = 3;

    if (index == 0 || index == 1) {
        // Call our new custom dialog!
        if (verifyAdmin()) {
            previousSafeTab = index; // Password correct, let them in
        } else {
            // Blocked! Send them back.
            ui->mainTabWidget->blockSignals(true);
            ui->mainTabWidget->setCurrentIndex(previousSafeTab);
            ui->mainTabWidget->blockSignals(false);
        }
    } else {
        previousSafeTab = index;
    }
}

void MainWindow::on_txtSearchDest_textChanged(const QString &arg1)
{
    // 1. Safely locate the Route Table
    QTableWidget *routeTable = this->findChild<QTableWidget*>("tblRoutes");
    if (!routeTable) return;

    // 2. Loop through every row in the table
    for (int i = 0; i < routeTable->rowCount(); ++i) {
        // Column 1 holds the "Path Layout" (e.g., "Dhaka - Narayanganj")
        QTableWidgetItem *pathItem = routeTable->item(i, 1);

        if (pathItem) {
            // 3. Check if the row's path text contains the typed letters (case-insensitive)
            // 'arg1' is automatically provided by Qt and holds whatever is currently typed in the box
            bool match = pathItem->text().contains(arg1, Qt::CaseInsensitive);

            // 4. Hide the row if it doesn't match, show it if it does
            routeTable->setRowHidden(i, !match);
        }
    }
}

bool MainWindow::verifyAdmin() {
    // 1. Read current password from file (Defaults to "1234" if file doesn't exist)
    QString currentPassword = "1234";
    QFile pwdFile("admin_pwd.txt");
    if (pwdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        currentPassword = pwdFile.readAll().trimmed();
        pwdFile.close();
    }

    // 2. Create the custom pop-up window
    QDialog dialog(this);
    dialog.setWindowTitle("Admin Authentication");
    dialog.setModal(true);
    dialog.setMinimumWidth(350);

    // 3. Create the UI Layouts & Widgets
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QLabel *lblMessage = new QLabel("Accessing management records requires Administrator rights.\n\nEnter Password:", &dialog);
    QLineEdit *txtPassword = new QLineEdit(&dialog);
    txtPassword->setEchoMode(QLineEdit::Password); // Masks text with dots
    mainLayout->addWidget(lblMessage);
    mainLayout->addWidget(txtPassword);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSetPwd = new QPushButton("Set Password", &dialog);
    QPushButton *btnLogin = new QPushButton("Login", &dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", &dialog);

    // 1. Stop the Set Password button from stealing the Enter key
    btnSetPwd->setAutoDefault(false);

    // 2. Force the Login button to be the default action when Enter is pressed
    btnLogin->setDefault(true);

    btnLayout->addWidget(btnSetPwd);
    btnLayout->addStretch(); // Pushes login/cancel to the right side
    btnLayout->addWidget(btnLogin);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    bool isAuthenticated = false;

    // 4. Connect the Login Button
    connect(btnLogin, &QPushButton::clicked, [&]() {
        if (txtPassword->text() == currentPassword) {
            isAuthenticated = true;
            dialog.accept(); // Closes the window successfully
        } else {
            QMessageBox::critical(&dialog, "Access Denied", "Incorrect Administrator Password!");
            txtPassword->clear(); // Wipes the text box for another try
        }
    });

    // 5. Connect the Cancel Button
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    // 6. Connect the Set Password Button
    connect(btnSetPwd, &QPushButton::clicked, [&]() {
        bool ok;
        // We use Normal mode here so the teacher can see the new password they are typing
        QString newPwd = QInputDialog::getText(&dialog, "System Setup",
                                               "Create a new Admin Password:",
                                               QLineEdit::Normal, "", &ok);
        if (ok && !newPwd.isEmpty()) {
            QFile file("admin_pwd.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << newPwd;
                file.close();
                currentPassword = newPwd; // Update the active password immediately
                QMessageBox::information(&dialog, "Success", "Password updated successfully!\nYou can now login with your new password.");
            }
        }
    });

    // Run the window and wait for the user to close it
    dialog.exec();

    return isAuthenticated;
}
