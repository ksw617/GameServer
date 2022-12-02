#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 _bufferSize)
{
    //bufferSize에 초기화
    bufferSize = _bufferSize;
    //buffer 크기값 할당
    buffer.resize(bufferSize);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
    //[][][][Data Size ][Free Size     ]
    //[][][][r][][][][w][][][][][][][][]
    int32 dataSize = DataSize();
    //[][][][][][][][  Free Size     ]
    //[][][][][][][][rw][][][][][][][][]
    if (dataSize == 0)
    {
        readPos = 0;
        writePos = 0;
    //[  Free Size                     ]
    //[rw][][][][][][][][][][][][][][][]
    }
    //[][][][Data Size ][Free Size     ]
    //[][][][r][][][][w][][][][][][][][]
    else
        //[ ][][][Data Size ][Free Size     ]
        //[&][][][&][][][][w][][][][][][][][]
    {   //[0][][][r][][][][w][][][][][][][][]

        //[   ][ ][ ][r 1][2  ][3][4][w][][][][][][][][]
        memcpy(&buffer[0], &buffer[readPos], dataSize);
        //[   ][ ][ ][   ][ 덮어 쓸꺼라 값들이 있어서 상관 없음] 
        //[r 1][2][3][4  ][w 2][3][4][][][][][][][][][][][]
        //readPos 초기화
        readPos = 0;
        //writePos는 0에서 부터 dataSize 위치
        writePos = dataSize;

    }

}

bool RecvBuffer::OnRead(int32 bytes)
{
    //문제있는 상황
    if (bytes > DataSize())
    {
        return false;
    }

    readPos += bytes;
    return true;
}

bool RecvBuffer::OnWrite(int32 bytes)
{
    //문제있는 상황
    if (bytes > FreeSize())
    {
        return false;
    }

    writePos += bytes;
    return true;
}
