#ifndef USERDATABASE_H_INCLUDED
#define USERDATABASE_H_INCLUDED

#include "Database.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

/**
 * @file UserDatabase.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the user implementation.
 * Basically this is just an object that interacts with a
 * database and provides mechanisms to create and remove users
 * and to check the credentials for users.
 */

class UserInterface;

class UserDatabase
{
public:
	typedef std::array<char, 20> password_hash_t;

	/**
	 * Construct the user database.
	 *
	 * @param database A shared database handle.
	 * @param user_interface The user interface instance.
	 */
	UserDatabase(std::shared_ptr<Database> database,
	             UserInterface &user_interface);

	/**
	 * Check credentials of a user.
	 *
	 * @param name The name the user is referenced by.
	 * @param password_hash The password SHA-1 hash that was generated.
	 */
	void check(std::string const &name, password_hash_t const &password_hash);

	/**
	 * Create a user in the database with the specified name and password hash.
	 * The password hash can be generated by the hash_bytes member function.
	 *
	 * @param name The name the user is referenced by.
	 * @param password_hash The password SHA-1 hash that was generated.
	 */
	void create(std::string const &name, password_hash_t const &password_hash);

	/**
	 * Create a user in the database with the specified name and plain password.
	 *
	 * @param name The name the user is referenced by.
	 * @param password The plain text password.
	 */
	void create(std::string const &name, std::string const &password);


	/**
	 * Delete a user in the database with the specified name.
	 *
	 * @param Name The name the user is referenced by.
	 */
	void remove(std::string const &name);

	/**
	 * Create a hash for the specificied byte sequence.
	 *
	 * @param bytes A sequence of bytes as data input for the hash algorithm.
	 * @return The hash sequence for the specified bytes.
	 */
	password_hash_t hash_bytes(std::vector<char> const &bytes);

private:
	std::shared_ptr<Database> database_;
	UserInterface &user_interface_;
};

#endif
