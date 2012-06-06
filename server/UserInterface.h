#ifndef USERINTERFACE_H_INCLUDED
#define USERINTERFACE_H_INCLUDED

#include <string>

/**
 * @file UserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Abstractions for the user command line interface.
 * One should be able to wait for input and retrieve what was typed
 * in.
 */

class UserInterface
{
public:
	/**
	 * Construct a user command line interface object.
	 * Provide a default constructor, because this is just an interface.
	 */
	UserInterface() = default;

	/**
	 * Deconstruct a user command line interface object, freeing all its resources
	 * and probably closing the window if any.
	 */
	virtual ~UserInterface();

	/**
	 * Delete the default copy constructor, making copying a user command line
	 * interface object impossible.
	 */
	UserInterface(UserInterface const &) = delete;

	/**
	 * Delete the default assignment operator, making assigning a user command line
	 * interface object impossible.
	 */
	UserInterface &operator=(UserInterface const &) = delete;

	/**
	 * Obtain one line of input. Block until it's received.
	 *
	 * @return The line of input which was typed with newline removed.
	 * @throws std::runtime_error If processing in the underlying
	 *                            implementation failed.
	 */
	virtual std::string get_input() = 0;
};

#endif
