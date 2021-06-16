/*
 * ZipEntry.h
 *
 *  Created on: Nov 29, 2018
 *      Author: ducky
 */

#ifndef FF_COMPRESS_ZIPENTRY_H_
#define FF_COMPRESS_ZIPENTRY_H_

#include <ff/Object.h>
#include <string>

NS_FF_BEG

class ZipEntry {
public:
	explicit ZipEntry(const std::string& entry);
	const std::string& getEntry() const;

private:
	std::string _entry;
};

NS_FF_END

#endif /* FF_COMPRESS_ZIPENTRY_H_ */
