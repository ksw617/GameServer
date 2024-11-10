namespace UnityClient.Network
{
    public class SendBuffer
    {
        // 데이터를 저장할 버퍼 배열
        readonly byte[] buffer;
        // 현재까지 쓰여진 데이터 크기
        int writeSize = 0;

        // 버퍼 내 남은 여유 공간 크기 (전체 크기 - 현재 쓰기 위치)
        public int FreeSize { get { return buffer.Length - writeSize; } }

        // 생성자: 지정된 크기의 버퍼 배열 초기화
        public SendBuffer(int size)
        {
            buffer = new byte[size];
        }

        // 쓰기 작업을 위한 세그먼트를 열어주는 메서드
        public ArraySegment<byte>? Open(int size)
        {
            // 요청된 크기가 버퍼 내 여유 공간을 초과하면 null 반환
            if (size > FreeSize)
                return null;

            // 지정된 크기만큼의 세그먼트를 반환 (쓰기 가능한 범위)
            return new ArraySegment<byte>(buffer, writeSize, size);
        }

        // 쓰기 작업 완료 후, 해당 세그먼트를 닫고 쓰기 위치를 갱신하는 메서드
        public ArraySegment<byte> Close(int size)
        {
            // 요청된 크기만큼의 세그먼트를 반환
            ArraySegment<byte> segment = new ArraySegment<byte>(buffer, writeSize, size);
            
            // 쓰기 위치를 갱신하여 다음 쓰기 작업을 준비
            writeSize += size;
            
            return segment;
        }
    }
}
