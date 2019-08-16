#pragma once

#include "streetpass/StreetpassManager.hpp"

void displayExportMenu(Streetpass::StreetpassManager& sm);
void displayExportSlotSelection(Streetpass::StreetpassManager& sm, const u8 slotNum);
void exportMenu(Streetpass::StreetpassManager& sm);

void exportAllBoxes(Streetpass::StreetpassManager& sm);
void exportBox(Streetpass::StreetpassManager& sm, const u8 slotNum);

void exportStreetpassMessage(Streetpass::StreetpassManager& sm, const u8 slotNum);
void exportAllStreetpasses(Streetpass::StreetpassManager& sm);
