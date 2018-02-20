// this module is freely based on tinyosc by Martin Roth
/**
 * Copyright (c) 2015, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once
#ifdef OSC_ENABLE
#define NUM_SCENES  (8)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include "oscCommunicator.hpp"
#define strncpy(_dst, _src, _len) strncpy_s(_dst, _len, _src, _TRUNCATE)
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

class OSC;
struct OSCBundle
{
	static const int64_t BUNDLE_ID = 0x2362756E646C6500L; // "#bundle"

	char *marker; // the current write head (where the next message will be written)
	char *buffer; // the original buffer
	uint32_t bufLen; // the byte length of the original buffer
	uint32_t bundleLen; // the byte length of the total bundle

	/**
	* Returns the timetag of an OSC bundle.
	*/
	uint64_t GetTimeTag()
	{
		return ntohll(*((uint64_t *)(buffer + 8)));
	}

	/**
	 * Returns the length in bytes of the bundle.
	 */
	uint32_t GetLength()
	{
		return bundleLen;
	}

	/**
	 * Starts writing a bundle to the given buffer with length.
	 */
	void Write(uint64_t timetag, char *buffer, const int len)
	{
		*((uint64_t *)buffer) = htonll(BUNDLE_ID);
		*((uint64_t *)(buffer + 8)) = htonll(timetag);

		buffer = buffer;
		marker = buffer + 16;
		bufLen = len;
		bundleLen = 16;
	}

	/**
	 * Write a message to a bundle buffer. Returns the number of bytes written.
	 */
	uint32_t WriteNextMessage(OSC *osc, const char *address, const char *format, ...);
	
};

struct OscReceivedMsg
{
	char *format;  // a pointer to the format field
	char *marker;  // the current read head
	char *buffer;  // the original message data (also points to the address)
	uint32_t len;  // length of the buffer data
	

	int NumParameters() { return (int)strlen(format); }
	char GetParamFormat(int index) { return format[index]; }
	bool GetNextParam(int index, OSCParam *rv)
	{
		switch (rv->paramType = GetParamFormat(index))
		{
		case 'b':
			GetNextBlob(&rv->s, &rv->paramLen);
			break;

		case 'm':
			GetNextMidi(rv->midi);
			break;

		case 'f':
			rv->f = GetNextFloat();
			break;

		case 'd':
			rv->d = GetNextDouble();
			break;

		case 'i':
			rv->i32 = GetNextInt32();
			break;

		case 'h':
			rv->i64 = GetNextInt64();
			break;

		case 't':
			rv->time = GetNextTimeTag();
			break;

		case 's':
			rv->s = GetNextString();
			rv->paramLen = rv->s ? strlen(rv->s) : 0;
			break;

		case 'F': //false 
		case 'T':// true
		case 'I': //inf
		case 'N': //nil
			break;

		default:
			return false;
		}

		return true;
	}

	/**
		* Returns a point to the address block of the OSC buffer.
		* This is also the start of the buffer.
		*/
	char *GetAddress()
	{
		return buffer;
	}

	bool GetNextMessage(OSCBundle *b)
	{
		bool rv = (b->marker - b->buffer) < b->bundleLen;
		if (rv)
		{
			uint32_t len = (uint32_t)ntohl(*((int32_t *)b->marker));
			Parse(b->marker + 4, len);
			b->marker += (4 + len); // move marker to next bundle element
		}
		return rv;
	}

	/**
		 * Returns the length in bytes of this message.
		 */
	uint32_t GetLength()
	{
		return len;
	}

	/**
		* Returns a pointer to the format block of the OSC buffer.
		*/
	char *GetFormat()
	{
		return format;
	}

	/**
		 * Returns the next 32-bit int. Does not check buffer bounds.
		 */
	int32_t GetNextInt32()
	{
		// convert from big-endian (network btye order)
		const int32_t i = (int32_t)ntohl(*((uint32_t *)marker));
		marker += 4;
		return i;
	}

	/**
	 * Returns the next 64-bit int. Does not check buffer bounds.
	 */
	int64_t GetNextInt64()
	{
		const int64_t i = (int64_t)ntohll(*((uint64_t *)marker));
		marker += 8;
		return i;
	}

	/**
	 * Returns the next 64-bit timetag. Does not check buffer bounds.
	 */
	uint64_t GetNextTimeTag()
	{
		return (uint64_t)GetNextInt64();
	}

	/**
	 * Returns the next 32-bit float. Does not check buffer bounds.
	 */
	float GetNextFloat()
	{
		// convert from big-endian (network btye order)
		const uint32_t i = ntohl(*((uint32_t *)marker));
		marker += 4;
		return *((float *)(&i));
	}

	/**
	 * Returns the next 64-bit float. Does not check buffer bounds.
	 */
	double GetNextDouble()
	{
		const uint64_t i = ntohll(*((uint64_t *)marker));
		marker += 8;
		return *((double *)(&i));
	}

	/**
	 * Returns the next string, or NULL if the buffer length is exceeded.
	 */
	const char *GetNextString()
	{
		int i = (int)strlen(marker);
		if (marker + i >= buffer + len) return NULL;
		const char *s = marker;
		i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
		marker += i;
		return s;
	}

	/**
	 * Points the given buffer pointer to the next blob.
	 * The len pointer is set to the length of the blob.
	 * Returns NULL and 0 if the OSC buffer bounds are exceeded.
	 */
	void GetNextBlob(const char **rbuffer, int *rlen)
	{
		int i = (int)ntohl(*((uint32_t *)marker)); // get the blob length
		if (marker + 4 + i <= buffer + len)
		{
			*rlen = i; // length of blob
			*rbuffer = marker + 4;
			i = (i + 7) & ~0x3;
			marker += i;
		}
		else
		{
			*rlen = 0;
			*rbuffer = NULL;
		}
	}

	/**
	 * Returns the next set of midi bytes. Does not check bounds.
	 * Bytes from MSB to LSB are: port id, status byte, data1, data2.
	 */
	void GetNextMidi(unsigned char midi[4])
	{
		for (int k = 0; k < 4; k++)
			midi[k] = *marker++;
	}

	int Parse(char *rbuffer, const int rlen)
	{
		// NOTE(mhroth): if there's a comma in the address, that's weird
		int i = 0;
		while (rbuffer[i] != '\0') ++i; // find the null-terimated address
		while (rbuffer[i] != ',') ++i; // find the comma which starts the format string
		if (i >= rlen) return -1; // error while looking for format string
		// format string is null terminated
		format = rbuffer + i + 1; // format starts after comma

		while (i < rlen && rbuffer[i] != '\0') ++i;
		if (i == rlen) return -2; // format string not null terminated

		i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
		marker = rbuffer + i;

		buffer = rbuffer;
		len = rlen;

		return 0;
	}
};

class OSC
{
public:
	OSC(int buffer_len = 2048)
	{
		bufferLen = buffer_len;
		inBuffer = new char[bufferLen];
		skt = INVALID_SOCKET;
		memset(lastError, 0, sizeof(lastError));
		initialized = false;
	}

	~OSC()
	{
		Close();
		WSACleanup();
		delete[] inBuffer;
	}

	void Close()
	{
		if(skt != INVALID_SOCKET)
			closesocket(skt);
		skt = INVALID_SOCKET;
	}

	bool Open(PCTSTR client, int inPort = 9000, int outPort = 9001)
	{
		if (!initialized)
		{
			if (!init())
				return false;
		}

		struct sockaddr_in sin;

		sin.sin_family = AF_INET;
		sin.sin_port = htons(inPort);
		sin.sin_addr.s_addr = INADDR_ANY;
		if (bind(skt, (struct sockaddr *) &sin, sizeof(sin)) != 0)
		{
			sprintf_s(lastError, sizeof(lastError), "bind (input) failed with error: %d", WSAGetLastError());
			return false;
		}

		sout.sin_family = AF_INET;
		sout.sin_port = htons(outPort);
		inet_pton(AF_INET, client, &(sout.sin_addr));
		return true;
	}

	bool Select()
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(skt, &readSet);
		struct timeval timeout = { 1, 0 }; // select times out after 1 second
		return select(0, &readSet, NULL, NULL, &timeout) > 0;
	}

	int Read()
	{
		struct sockaddr_in sa;
		int sa_len = sizeof(sa);
		return recvfrom(skt, inBuffer, bufferLen, 0, (sockaddr *)&sa, &sa_len);
	}

	/**
	 * Returns true if the buffer refers to a bundle of OSC messages. False otherwise.
	 */
	bool IsBundle()
	{
		return ((*(const int64_t *)inBuffer) == htonll(OSCBundle::BUNDLE_ID));
	}

	/**
	 * Reads a buffer containing a bundle of OSC messages.
	 */
	void ParseBundle(OSCBundle *b, const int len)
	{
		b->buffer = (char *)inBuffer;
		b->marker = inBuffer + 16; // move past '#bundle ' and timetag fields
		b->bufLen = len;
		b->bundleLen = len;
	}

	/**
	 * Parse a buffer containing an OSC message.
	 * The contents of the buffer are NOT copied.
	 * The message struct only points at relevant parts of the original buffer.
	 * Returns 0 if there is no error. An error code (a negative number) otherwise.
	 */
	int ParseMessage(OscReceivedMsg *o, const int len)
	{
		return o->Parse(inBuffer, len);
	}

	/**
	 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
	 * The entire buffer is cleared before writing.
	 */
	bool Write(char *buffer, const int len, const char *address, const char *format, ...)
	{
		va_list ap;
		va_start(ap, format);
		int numWritten = write(buffer, len, address, format, ap);
		va_end(ap);
		if (numWritten > 0)
		{
			int server_length = sizeof(sout);
			if (sendto(skt, buffer, numWritten, 0, (struct sockaddr *)&sout, server_length) == SOCKET_ERROR)
			{
				sprintf_s(lastError, sizeof(lastError), "send UDP dgram failed with error: %d", GetLastError());
				return false;
			}
		}
		return true;
	}

	/**
	 * A convenience function to (non-destructively) print a pre-parsed OSC message
	 * to stdout.
	 */
	const char *LastError() { return lastError; }

private:
#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

	int inet_pton(int af, const char *src, in_addr *dst)
	{
		switch(af)
		{
		case AF_INET:
			return inet_pton4(src, (char *)dst);
		case AF_INET6:
			return inet_pton6(src, (char *)dst);
		default:
			return -1;
		}
	}
	int inet_pton4(const char *src, char *dst)
	{
		uint8_t tmp[NS_INADDRSZ], *tp;

		int saw_digit = 0;
		int octets = 0;
		*(tp = tmp) = 0;

		int ch;
		while((ch = *src++) != '\0')
		{
			if(ch >= '0' && ch <= '9')
			{
				uint32_t n = *tp * 10 + (ch - '0');

				if(saw_digit && *tp == 0)
					return 0;

				if(n > 255)
					return 0;

				*tp = n;
				if(!saw_digit)
				{
					if(++octets > 4)
						return 0;
					saw_digit = 1;
				}
			} else if(ch == '.' && saw_digit)
			{
				if(octets == 4)
					return 0;
				*++tp = 0;
				saw_digit = 0;
			} else
				return 0;
		}
		if(octets < 4)
			return 0;

		memcpy(dst, tmp, NS_INADDRSZ);

		return 1;
	}
	int inet_pton6(const char *src, char *dst)
	{
		static const char xdigits[] = "0123456789abcdef";
		uint8_t tmp[NS_IN6ADDRSZ];

		uint8_t *tp = (uint8_t*)memset(tmp, '\0', NS_IN6ADDRSZ);
		uint8_t *endp = tp + NS_IN6ADDRSZ;
		uint8_t *colonp = NULL;

		/* Leading :: requires some special handling. */
		if(*src == ':')
		{
			if(*++src != ':')
				return 0;
		}

		const char *curtok = src;
		int saw_xdigit = 0;
		uint32_t val = 0;
		int ch;
		while((ch = tolower(*src++)) != '\0')
		{
			const char *pch = strchr(xdigits, ch);
			if(pch != NULL)
			{
				val <<= 4;
				val |= (pch - xdigits);
				if(val > 0xffff)
					return 0;
				saw_xdigit = 1;
				continue;
			}
			if(ch == ':')
			{
				curtok = src;
				if(!saw_xdigit)
				{
					if(colonp)
						return 0;
					colonp = tp;
					continue;
				} else if(*src == '\0')
				{
					return 0;
				}
				if(tp + NS_INT16SZ > endp)
					return 0;
				*tp++ = (uint8_t)(val >> 8) & 0xff;
				*tp++ = (uint8_t)val & 0xff;
				saw_xdigit = 0;
				val = 0;
				continue;
			}
			if(ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
				inet_pton4(curtok, (char*)tp) > 0)
			{
				tp += NS_INADDRSZ;
				saw_xdigit = 0;
				break; /* '\0' was seen by inet_pton4(). */
			}
			return 0;
		}
		if(saw_xdigit)
		{
			if(tp + NS_INT16SZ > endp)
				return 0;
			*tp++ = (uint8_t)(val >> 8) & 0xff;
			*tp++ = (uint8_t)val & 0xff;
		}
		if(colonp != NULL)
		{
			/*
			* Since some memmove()'s erroneously fail to handle
			* overlapping regions, we'll do the shift by hand.
			*/
			const int n = tp - colonp;

			if(tp == endp)
				return 0;

			for(int i = 1; i <= n; i++)
			{
				endp[-i] = colonp[n - i];
				colonp[n - i] = 0;
			}
			tp = endp;
		}
		if(tp != endp)
			return 0;

		memcpy(dst, tmp, NS_IN6ADDRSZ);

		return 1;
	}
	uint32_t write(char *buffer, const int len, const char *address, const char *format, va_list ap)
	{
		memset(buffer, 0, len); // clear the buffer
		uint32_t i = (uint32_t)strlen(address);
		if (address == NULL || i >= len) return -1;
		strncpy(buffer, address, i + 1);
		i = (i + 4) & ~0x3;
		buffer[i++] = ',';
		int s_len = (int)strlen(format);
		if (format == NULL || (i + s_len) >= len) return -2;
		strncpy(buffer + i, format, s_len + 1);
		i = (i + 4 + s_len) & ~0x3;

		for (int j = 0; format[j] != '\0'; ++j) {
			switch (format[j]) {
			case 'b': {
				const uint32_t n = (uint32_t)va_arg(ap, int); // length of blob
				if (i + 4 + n > len) return -3;
				char *b = (char *)va_arg(ap, void *); // pointer to binary data
				*((uint32_t *)(buffer + i)) = htonl(n); i += 4;
				memcpy(buffer + i, b, n);
				i = (i + 3 + n) & ~0x3;
				break;
			}
			case 'f': {
				if (i + 4 > len) return -3;
				const float f = (float)va_arg(ap, double);
				*((uint32_t *)(buffer + i)) = htonl(*((uint32_t *)&f));
				i += 4;
				break;
			}
			case 'd': {
				if (i + 8 > len) return -3;
				const double f = (double)va_arg(ap, double);
				*((uint64_t *)(buffer + i)) = htonll(*((uint64_t *)&f));
				i += 8;
				break;
			}
			case 'i': {
				if (i + 4 > len) return -3;
				const uint32_t k = (uint32_t)va_arg(ap, int);
				*((uint32_t *)(buffer + i)) = htonl(k);
				i += 4;
				break;
			}
			case 'm': {
				if (i + 4 > len) return -3;
				const unsigned char *const k = (unsigned char *)va_arg(ap, void *);
				memcpy(buffer + i, k, 4);
				i += 4;
				break;
			}
			case 't':
			case 'h': {
				if (i + 8 > len) return -3;
				const uint64_t k = (uint64_t)va_arg(ap, long long);
				*((uint64_t *)(buffer + i)) = htonll(k);
				i += 8;
				break;
			}
			case 's': {
				const char *str = (const char *)va_arg(ap, void *);
				s_len = (int)strlen(str);
				if (i + s_len >= len) return -3;
				strncpy(buffer + i, str, len - i - s_len);
				i = (i + 4 + s_len) & ~0x3;
				break;
			}
			case 'T': // true
			case 'F': // false
			case 'N': // nil
			case 'I': // infinitum
				break;
			default: return -4; // unknown type
			}
		}

		return i; // return the total number of bytes written
	}
	bool init()
	{
		initialized = true;
		// open a socket to listen for datagrams (i.e. UDP packets) on port 9000
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			/* Tell the user that we could not find a usable */
			/* Winsock DLL.                                  */
			sprintf_s(lastError, sizeof(lastError), "WSAStartup failed with error: %d", err);
			return false;
		}

		skt = socket(AF_INET, SOCK_DGRAM, 0);
		u_long iMode = 1;
		err = ioctlsocket(skt, FIONBIO, &iMode);
		if (err != NO_ERROR)
		{
			sprintf_s(lastError, sizeof(lastError), "ioctlsocket (in) failed with error: %ld", err);
			return false;
		}

		return true;
	}

	char lastError[2048];
	SOCKET skt;
	bool initialized;
	int bufferLen;
	char *inBuffer;
	struct sockaddr_in sout;
};

class IPCServer
{
private:
	uint8_t *pMap;
	HANDLE m_hMapFile;      // Handle to the mapped memory file
	oscCircBuffer *oscToClientBuff[NUM_SCENES];	// OSC -> client
	oscCircBuffer *clientToOscBuff[NUM_SCENES];	// client -> OSC
	
public:
	IPCServer()
	{
		for(int k = 0; k < NUM_SCENES; k++)
			oscToClientBuff[k] = clientToOscBuff[k] = NULL;
		m_hMapFile = NULL;		
		pMap = NULL;
	}
	
	bool Open()
	{
		int b_l = OSCBUFFER_SIZE * sizeof(OSCMsg) + oscCircBuffer::bufferOverhead();
		int rawsize = NUM_SCENES * 2 * b_l + sizeof(uint32_t) /*flag SERVER_POS*/;
#ifdef DEBUG
		info("IPCServer: total size = %i bufflen=%i", rawsize, b_l);
#endif
		bool opened;
		try
		{
			m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, rawsize, "OSC_mem");
			if(m_hMapFile != NULL)
				opened = true;
		} catch(_exception ex)
		{
			opened = false;
		}

		if(opened)
		{
			pMap = (uint8_t *)MapViewOfFile(m_hMapFile, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, rawsize);
			if(pMap == NULL)
			{
				Close();
			} else
			{
				for(int k = 0; k < NUM_SCENES; k++)
				{
					int offset = b_l * 2 * k;
					clientToOscBuff[k] = new oscCircBuffer(b_l, pMap, START_OF_BUFFER+offset);
#ifdef DEBUG
					info("clientToOscBuff[%i] created @ offset = %i", k, START_OF_BUFFER + offset);
#endif
					oscToClientBuff[k] = new oscCircBuffer(b_l, pMap, START_OF_BUFFER + b_l+offset);
#ifdef DEBUG
					info("oscToClientBuff[%i] created @ offset = %i", k, START_OF_BUFFER +b_l+ offset);
#endif

				}
				*((uint32_t *)pMap) = 0x01;
			}
		}

		return opened;
	}

	void Close()
	{
		if(pMap != NULL)
			*((uint32_t *)pMap) = 0x00;

		for(int k = 0; k < NUM_SCENES; k++)
		{
			oscToClientBuff[k] = clientToOscBuff[k] = NULL;
		}

		if(pMap != NULL)
		{
			UnmapViewOfFile(pMap);
			pMap = NULL;
		}

		// Close the file handle
		if(m_hMapFile != NULL)
		{
			CloseHandle(m_hMapFile);
			m_hMapFile = NULL;
		}
	}

	void Write(OSCMsg *msg)
	{
		oscToClientBuff[msg->scene-1]->WriteChunk(msg);
	}

	bool Read(int scene, OSCMsg *dest)
	{
		return clientToOscBuff[scene]->ReadChunk(dest);
	}
};
#endif //OSC
