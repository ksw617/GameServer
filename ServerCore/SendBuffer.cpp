#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int size)
{
	//���Ҵ�
    buffer.resize(size);
}

SendBuffer::~SendBuffer()
{
}

bool SendBuffer::CopyData(void* data, int len)
{
	int size = buffer.size();

	//������ �������� ���̰� ���� �뷮���� ũ�ٸ�
	if (Capacity() < len)
		return false;

	//������ ����
	memcpy(buffer.data(), data, len);
	writeSize = len;

    return true;
}
