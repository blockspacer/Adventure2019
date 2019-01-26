/*
* Door.h
*
* Class Description: A class designed to represent exits from a room.
*
* Created on: January 17, 2019
* Author: Brittany Ryan, 301217765
*/

#ifndef WEBSOCKETNETWORKING_DOOR_H
#define WEBSOCKETNETWORKING_DOOR_H

#include <string>
#include <vector>

namespace model {

	class Door {

	private:
		std::string dir; //direction
		std::vector<std::string> desc;
		std::vector<std::string> keywords;
		int to; //id of room door leads to

		friend std::ostream& operator<<(std::ostream& os, const Door& rhs);

	public:
		//constructors
		Door();
		Door(std::string dir, int to);
		Door(std::string dir, std::vector<std::string> desc, std::vector<std::string> keywords, int to);

		//getters and setters
		std::string getDir();
		std::vector<std::string> getDesc();
		std::vector<std::string> getKeywords();
		int getTo();
		void setDir(std::string dir);
		void setDesc(std::vector<std::string> desc);
		void setKeywords(std::vector<std::string> keywords);
		void setTo(int to);

	};
}

#endif
