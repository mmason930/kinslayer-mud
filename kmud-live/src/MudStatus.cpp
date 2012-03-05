#include "MudStatus.h"

std::list<MudStatus*> MudStatus::enums;

MudStatus *MudStatus::notRunning = new MudStatus(0, "Not Running");
MudStatus *MudStatus::runningNotConnected = new MudStatus(1, "Running, Not Connected");
MudStatus *MudStatus::booting = new MudStatus(3, "Booting");
MudStatus *MudStatus::running = new MudStatus(4, "Running");
MudStatus *MudStatus::shuttingDown = new MudStatus(5, "Shutting Down");
MudStatus *MudStatus::hung = new MudStatus(6, "Hung");
