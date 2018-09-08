//
// Created by apostrophe on 30.10.17.
//

#ifndef CTHOOKINT_GUI_H
#define CTHOOKINT_GUI_H

#include "../Menu.h"
#include "Tabs/hvhtab.h"
#include "Tabs/misctab.h"
#include "Tabs/aimtab.h"
#include "Tabs/visualtab.h"
namespace gui {

    void RenderWindow();
}
namespace Main {

    extern bool showWindow;
}
namespace UI {
	void KeyBindButton(ButtonCode_t* key);
	bool ColorPicker(float* col, bool alphabar);
	bool ColorPicker3(float col[3]);
	bool ColorPicker4(float col[4]);
	void SetupColors();
}
#endif //CTHOOKINT_GUI_H
