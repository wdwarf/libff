/*
 * Uuid.cpp
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#include <ff/UUID.h>
#include <cstring>
#include <ff/Random.h>

using namespace std;

NS_FF_BEG

namespace {

struct uuid {
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint16_t clock_seq;
	uint8_t node[6];
};

inline void uuid_pack(const struct uuid *uu, uuid_t ptr) {
	uint32_t tmp;
	unsigned char *out = ptr;

	tmp = uu->time_low;
	out[3] = (unsigned char) tmp;
	tmp >>= 8;
	out[2] = (unsigned char) tmp;
	tmp >>= 8;
	out[1] = (unsigned char) tmp;
	tmp >>= 8;
	out[0] = (unsigned char) tmp;

	tmp = uu->time_mid;
	out[5] = (unsigned char) tmp;
	tmp >>= 8;
	out[4] = (unsigned char) tmp;

	tmp = uu->time_hi_and_version;
	out[7] = (unsigned char) tmp;
	tmp >>= 8;
	out[6] = (unsigned char) tmp;

	tmp = uu->clock_seq;
	out[9] = (unsigned char) tmp;
	tmp >>= 8;
	out[8] = (unsigned char) tmp;

	memcpy(out + 10, uu->node, 6);
}

inline void uuid_unpack(const uuid_t in, struct uuid *uu) {
	const uint8_t *ptr = in;
	uint32_t tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_low = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_mid = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_hi_and_version = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->clock_seq = tmp;

	memcpy(uu->node, ptr, 6);
}

inline void uuidRandom(uuid_t out, int *num) {
	uuid_t buf;
	struct uuid uu;
	int i, n;

	if (!num || !*num)
		n = 1;
	else
		n = *num;

	Random ran;
	for (i = 0; i < n; i++) {
		ran.getRandomBytes(buf, sizeof(buf));
		uuid_unpack(buf, &uu);

		uu.clock_seq = (uu.clock_seq & 0x3FFF) | 0x8000;
		uu.time_hi_and_version = (uu.time_hi_and_version & 0x0FFF) | 0x4000;
		uuid_pack(&uu, out);
		out += sizeof(uuid_t);
	}
}

int uuid_parse(const char *in, uuid_t uu) {
	struct uuid uuid;
	int i;
	const char *cp;
	char buf[3];

	if (strlen(in) != 36)
		return -1;
	for (i = 0, cp = in; i <= 36; i++, cp++) {
		if ((i == 8) || (i == 13) || (i == 18) || (i == 23)) {
			if (*cp == '-')
				continue;
			else
				return -1;
		}
		if (i == 36)
			if (*cp == 0)
				continue;
		if (!isxdigit(*cp))
			return -1;
	}
	uuid.time_low = strtoul(in, NULL, 16);
	uuid.time_mid = strtoul(in + 9, NULL, 16);
	uuid.time_hi_and_version = strtoul(in + 14, NULL, 16);
	uuid.clock_seq = strtoul(in + 19, NULL, 16);
	cp = in + 24;
	buf[2] = 0;
	for (i = 0; i < 6; i++) {
		buf[0] = *cp++;
		buf[1] = *cp++;
		uuid.node[i] = strtoul(buf, NULL, 16);
	}

	uuid_pack(&uuid, uu);
	return 0;
}

static const char *fmt_lower =
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";

static const char *fmt_upper =
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";

#ifdef UUID_UNPARSE_DEFAULT_UPPER
#define FMT_DEFAULT fmt_upper
#else
#define FMT_DEFAULT fmt_lower
#endif

static void uuid_unparse_x(const uuid_t uu, char *out, const char *fmt) {
	struct uuid uuid;

	uuid_unpack(uu, &uuid);
	sprintf(out, fmt, uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
			uuid.clock_seq >> 8, uuid.clock_seq & 0xFF, uuid.node[0],
			uuid.node[1], uuid.node[2], uuid.node[3], uuid.node[4],
			uuid.node[5]);
}

void uuid_unparse(const uuid_t uu, char *out) {
	uuid_unparse_x(uu, out, FMT_DEFAULT);
}

}

Uuid::Uuid() {
	int num = 1;
	uuidRandom(this->m_uuid, &num);
}

Uuid::Uuid(const std::string& uuidStr) {
	(void) uuid_parse(uuidStr.c_str(), this->m_uuid);
}

Uuid::~Uuid() {
}

const uuid_t* Uuid::getBuffer() const {
	return &this->m_uuid;
}

string Uuid::toString() const {
	char buf[64];
	uuid_unparse(this->m_uuid, buf);
	return buf;
}

Uuid::operator std::string() const {
	return this->toString();
}

std::ostream& operator<<(std::ostream& o, const Uuid& u) {
	o << (std::string) u;
	return o;
}

NS_FF_END
