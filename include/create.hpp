#pragma once

#include <3ds/types.h>
#include <memory>
#include <string>

#include "streetpass/Box.hpp"
#include "streetpass/MBoxList.hpp"
#include "streetpass/StreetpassManager.hpp"

void displayCreateMenu(Streetpass::StreetpassManager& sm);
void createMenu(Streetpass::StreetpassManager& sm);

/*
class CreateBox {
public:
    explicit CreateBox(const std::string& id);
    ~CreateBox() = default;

    bool CheckMBoxList();
    bool CheckTemplateFiles();
    bool CheckOutboxMessages();

    bool CreateFile(const std::string& templateFile, const CecDataPathType cecPath);
    bool CreateMessage(const std::string& templateFile);

    bool CreateInbox();
    bool CreateOutbox();
    bool CreateOutboxMessages();

    bool CreateMBoxData();

    bool UpdateMBoxList();

private:
    std::string id;
    u32 boxId;

    //Streetpass::CecMBoxListHeader mboxList;
    std::unique_ptr<Streetpass::Box> box;
};
*/
void createBox(const std::string& id);

void createBox(Streetpass::StreetpassManager& sm);
