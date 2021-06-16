/*
 * ZipEntry.cpp
 *
 *  Created on: Nov 29, 2018
 *      Author: ducky
 */

#include <ff/ZipEntry.h>

NS_FF_BEG

ZipEntry::ZipEntry(const std::string& entry) {
	this->_entry = entry;
}

const std::string& ZipEntry::getEntry() const {
	return _entry;
}

NS_FF_END
