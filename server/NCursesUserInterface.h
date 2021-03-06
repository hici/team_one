#ifndef READLINEUSERINTERFACE_H_INCLUDED
#define READLINEUSERINTERFACE_H_INCLUDED

#include "UserInterface.h"

#include <curses.h>

#include <memory>
#include <string>
#include <thread>

/**
 * @file NCursesUserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * NCurses implementation for the user command line interface.
 */

namespace userinterface_errors
{
	/**
	 * A generic user interface exception of the NCurses implementation.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct NCursesError
		: Failure
	{
		/**
		 * Construct a new NCurses failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		NCursesError(std::string const &message);
	};
}

class NCursesUserInterface
	: public UserInterface
{
public:
	/**
	 * Create an interface with NCurses.
	 * Setup output window and input area.
	 */
	NCursesUserInterface();

public:
	/**
	 * Release all resources used by the interface and reset
	 * the window to its initial state.
	 */
	~NCursesUserInterface();

	void run();
	void deinitialize();

private:
	void printfv(char const *format, ...);

	bool deinitialized_;
	SCREEN *screen_;
	WINDOW *input_window_;
	std::wstring::size_type current_position_;
	std::mutex mutex_;
};

#endif
