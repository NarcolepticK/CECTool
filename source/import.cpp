#include <3ds.h>
#include <fstream>

#include "import.hpp"
#include "common/io.hpp"
#include "common/util.hpp"

#include "streetpass/MBox.hpp"

extern "C" {
#include "3ds/services/cecd.h"
}

void displayImportMenu(Screens& screens, Streetpass::StreetpassManager& sm) {
    ClearScreens(screens);
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nImport Menu\n\n");
    printf("[A] Import a Box\n\n");
    //printf("[B] Import Messages\n");
    printf("[Y] Import Streetpass Messages\n\n");
    printf("Press START for Main Menu\n\n");
}

void displayImportSlotSelection(Screens& screens, Streetpass::StreetpassManager& sm, const u8 slotNum) {
    ClearScreens(screens);
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nImport Menu\n\n");
    printf("[A] Select a Box to Import\n\n");
    printf("Press START for Main Menu\n\n");
    consoleSelect(&screens.bottom);
    printf("Slot Number: [%x]\n\n", slotNum);
}

void displayImportStreetpassBoxSelection(Screens& screens, Streetpass::StreetpassManager& sm, const u8 slotNum, const u32 numOfStreetpasses) {
    ClearScreens(screens);
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nImport Menu\n\n");
    printf("[A] Select a Box to Import Streetpasses from\n\n");
    printf("Press START for Main Menu\n\n");
    consoleSelect(&screens.bottom);
    printf("Slot Number: [%x]\n", slotNum);
    printf("Streetpasses Available: [%lu]\n", numOfStreetpasses);
}

void displayImportStreetpassSelection(Screens& screens, Streetpass::StreetpassManager& sm, const u8 slotNum, const u32 inboxCount, const u32 inboxLimit, const std::string& fileName) {
    ClearScreens(screens);
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nImport Menu\n\n");
    printf("[A] Select a Streetpass to Import\n\n");
    printf("Press START for Main Menu\n\n");
    consoleSelect(&screens.bottom);
    printf("Slot Number: [%x],  Inbox Limit: [%lu/%lu]\n\n", slotNum, inboxCount, inboxLimit);
    printf("%s\n\n", fileName.c_str());
}

void importMenu(Screens& screens, Streetpass::StreetpassManager& sm) {
    consoleSelect(&screens.bottom);
    u8 slotNum = 0;
    u32 down = hidKeysDown();
    displayImportMenu(screens, sm);
    while (aptMainLoop() && !(down & KEY_START)) {
        down = hidKeysDown();
        hidScanInput();

        if (down & KEY_A) {
            importBox(screens, sm, slotNum);
            waitForInput();
            break;
        } else if (down & KEY_Y) {
            std::unique_ptr<STDirectory> streetpassesDirectory =
                std::make_unique<STDirectory>("/3ds/CECTool/import/streetpasses/");

            if(streetpassesDirectory->count() < 1) {
                printf("/3ds/CECTool/import/streetpasses is empty\n");
                printf("Nothing to import. Returning to main menu.\n");
                waitForInput();
                break;
            }
            std::vector<std::string> currentBoxNames = sm.BoxList().BoxNames();
            std::string importPath = "/3ds/CECTool/import/streetpasses/" + currentBoxNames[slotNum] + "/";
            std::unique_ptr<STDirectory> importDirectory =
                std::make_unique<STDirectory>(importPath);
            u32 numOfStreetpasses = importDirectory->count();

            displayImportStreetpassBoxSelection(screens, sm, slotNum, numOfStreetpasses);
            while (aptMainLoop() && !(down & KEY_START)) {
                down = hidKeysDown();
                hidScanInput();
                if (down & KEY_A) {
                    importStreetpassMenu(screens, sm, slotNum, *importDirectory, importPath);
                    break;
                } else if (down & KEY_DOWN) {
                    if (slotNum > 0) {
                        slotNum--;
                        importPath = "/3ds/CECTool/import/streetpasses/" + currentBoxNames[slotNum] + "/";
                        importDirectory = std::make_unique<STDirectory>(importPath);
                        numOfStreetpasses = importDirectory->count();
                        displayImportStreetpassBoxSelection(screens, sm, slotNum, numOfStreetpasses);
                    }
                } else if (down & KEY_UP) {
                    if (slotNum < sm.BoxList().MaxNumberOfSlots() - 1) {
                        slotNum++;
                        importPath = "/3ds/CECTool/import/streetpasses/" + currentBoxNames[slotNum] + "/";
                        importDirectory = std::make_unique<STDirectory>(importPath);
                        numOfStreetpasses = importDirectory->count();
                        displayImportStreetpassBoxSelection(screens, sm, slotNum, numOfStreetpasses);
                    }
                }
            }
            break;
        }
    }
}

void importStreetpassMenu(Screens& screens, Streetpass::StreetpassManager& sm, const u8 slotNum, STDirectory& importDirectory, const std::string& importPath) {
    consoleSelect(&screens.bottom);
    std::shared_ptr<Streetpass::MBox> mbox = sm.OpenBox(slotNum);
    if (mbox) {
        u32 inboxCount = mbox->Inbox().Info().NumberOfMessages();
        const u32 inboxLimit = mbox->Inbox().Info().MaxMessages();
        const u32 streetpassesAvailable = importDirectory.count();
        u32 streetpassIndex = 0;

        u32 down = hidKeysDown();
        displayImportStreetpassSelection(screens, sm, slotNum, inboxCount, inboxLimit, importDirectory.item(streetpassIndex));
        while (aptMainLoop() && !(down & KEY_START)) {
            down = hidKeysDown();
            hidScanInput();
            if (down & KEY_A) {
                if (inboxCount < inboxLimit) {
                    importStreetpasses(screens, sm, *mbox, importPath, importDirectory.item(streetpassIndex));
                }
                if (inboxCount < mbox->Inbox().Info().NumberOfMessages()) {
                    printf("Importing streetpass complete.");
                    inboxCount = mbox->Inbox().Info().NumberOfMessages();
                } else {
                    printf("Importing streetpass failed.");
                }

                waitForInput();
                displayImportStreetpassSelection(screens, sm, slotNum, inboxCount, inboxLimit, importDirectory.item(streetpassIndex));
                //break;
            } else if (down & KEY_DOWN) {
                if (streetpassIndex > 0) {
                    streetpassIndex--;

                    displayImportStreetpassSelection(screens, sm, slotNum, inboxCount, inboxLimit, importDirectory.item(streetpassIndex));
                }
            } else if (down & KEY_UP) {
                if (streetpassIndex < streetpassesAvailable - 1) {
                    streetpassIndex++;

                    displayImportStreetpassSelection(screens, sm, slotNum, inboxCount, inboxLimit, importDirectory.item(streetpassIndex));
                }
            }
        }
    }
}

void importBox(Screens& screens, Streetpass::StreetpassManager& sm, const u8 slotNum) {
    consoleSelect(&screens.bottom);
    printf("Unimplemented.\n");
}

void importStreetpasses(Screens& screens, Streetpass::StreetpassManager& sm, Streetpass::MBox& mbox, const std::string& importPath, const std::string& fileName) {
    consoleSelect(&screens.bottom);
    std::ifstream in(importPath + fileName, std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 messageSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> messageBuffer(messageSize);
    if(messageBuffer.capacity() >= messageSize) {
        in.read(reinterpret_cast<char*>(messageBuffer.data()), messageSize);
        if(!in) {
            printf("Error reading message.\n");
        }
    }
    in.close();

    time_t currentTime;
    struct tm timeStruct;
    time(&currentTime);
    gmtime_r(&currentTime, &timeStruct);

    CecTimestamp timeReceived;
    timeReceived.hour = timeStruct.tm_hour;
    timeReceived.minute = timeStruct.tm_min;
    timeReceived.second = timeStruct.tm_sec;
    timeReceived.day = timeStruct.tm_mday;
    timeReceived.month = timeStruct.tm_mon;
    timeReceived.year = timeStruct.tm_year + 1900;

    std::unique_ptr<Streetpass::Message> message = std::make_unique<Streetpass::Message>(messageBuffer);
    message->MessageHeader().received = timeReceived;
    message->MessageHeader().newFlag = 1;
    message->MessageHeader().unopened = 1;
    mbox.Inbox().AddMessage(*message);

    Result res = cecdWriteMessage(mbox.ProgramId(), false, message->MessageId().data, sizeof(CecMessageId), message->data().data(),message->MessageSize());
    if (R_FAILED(res)) {
        printf("Inbox WriteMessage Failed: %lX\n", res);
        //return res;
    }

    res = cecdOpenAndWrite(mbox.Inbox().Info().data().data(), mbox.Inbox().Info().FileSize(), mbox.ProgramId(), CEC_PATH_INBOX_INFO,
                           CEC_WRITE | CEC_CREATE | CEC_CHECK);
    if (R_FAILED(res)) {
        printf("Inbox BoxInfo OpenAndWrite Failed: %lX\n", res);
        //return res;
    }

    mbox.Header().lastReceived = timeReceived;
    res = cecdOpenAndWrite(mbox.data().data(), sizeof(CecMBoxInfoHeader), mbox.ProgramId(), CEC_PATH_MBOX_INFO,
                             CEC_WRITE | CEC_CREATE | CEC_CHECK);
    if (R_FAILED(res)) {
        printf("MBoxInfo OpenAndWrite Failed: %lX\n", res);
        //return res;
    }
}
