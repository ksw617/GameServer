#pragma once

class RecvBuffer
{
private:
	int32 bufferSize = 0;
	int32 readPos = 0;
	int32 writePos = 0;
	vector<BYTE> buffer; 
public:
	RecvBuffer(int32 _bufferSize);
	~RecvBuffer();
public:
	void Clean();
	bool OnRead(int32 bytes);
	bool OnWrite(int32 bytes);

public:
	//buffer аж╪р  [&]  		 [&]
	//[][][][][][][r][][][][][w][][][][][][]
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() {return &buffer[writePos]; }
	//[][][][Data Size ][Free Size     ]
	//[][][][r][][][][w][][][][][][][][]
	int32 DataSize() { return writePos - readPos; }
	int32 FreeSize() { return bufferSize - writePos; }
};

