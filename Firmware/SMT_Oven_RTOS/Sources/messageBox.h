/*
 * messageBox.h
 *
 *  Created on: 8Oct.,2016
 *      Author: podonoghue
 */

#ifndef SOURCES_MESSAGEBOX_H_
#define SOURCES_MESSAGEBOX_H_

#include "switch_debouncer.h"

enum MessageBoxSelection {MSG_OK, MSG_OK_CANCEL, MSG_YES_NO, MSG_YES_NO_CANCEL};
enum MessageBoxResult    {MSG_IS_OK, MSG_IS_CANCEL, MSG_IS_YES, MSG_IS_NO};

/**
 * Writes a full screen message to LCD
 *
 * @param title      Title for screen
 * @param message    Message to display
 * @param selection  Key selection to display at bottom of screen
 *
 * @note Waits for valid key press before returning.
 *
 * @return Value reflecting key pressed
 */
extern MessageBoxResult messageBox(const char *title, const char *message, MessageBoxSelection selection=MSG_OK);

/**
 * Checks if thermocouples are present and enabled\n
 * Displays a message and waits for key press if not.
 *
 * @return true => At least one thermocouple is active
 */
bool checkThermocouples();

#endif /* SOURCES_MESSAGEBOX_H_ */
