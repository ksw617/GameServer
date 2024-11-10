using System;
using System.Threading;

namespace UnityClient.Network
{
    public class SendBufferManager
    {
        // ThreadLocal<SendBuffer>를 사용하여 각 스레드마다 독립적인 SendBuffer 인스턴스를 유지
#pragma warning disable CS8603 // Possible null reference return.
        private static readonly ThreadLocal<SendBuffer> CurrentBuffer = new (() => null);
#pragma warning restore CS8603 // Possible null reference return.

        // 각 SendBuffer의 기본 청크 크기 설정 (65535 * 100 = 6553500 바이트)
        public static int ChunkSize { get; set; } = 65535 * 100;

        // 지정된 크기만큼 버퍼를 오픈하여 사용 가능한 ArraySegment<byte> 반환
        public static ArraySegment<byte> Open(int reserveSize)
        {
            // CurrentBuffer가 null일 경우 새로운 SendBuffer 인스턴스를 생성
            if (CurrentBuffer.Value == null)
                CurrentBuffer.Value = new SendBuffer(ChunkSize);

            // 현재 버퍼의 여유 공간이 부족할 경우 새로운 SendBuffer로 교체
            if (CurrentBuffer.Value.FreeSize < reserveSize)
                CurrentBuffer.Value = new SendBuffer(ChunkSize);

            // reserveSize 크기의 배열 세그먼트를 반환
            return CurrentBuffer.Value.Open(reserveSize) ?? throw new InvalidOperationException("Failed to open buffer segment.");
        }

        // 사용한 크기만큼 버퍼를 닫고, ArraySegment<byte> 반환
        public static ArraySegment<byte> Close(int usedSize)
        {
            // CurrentBuffer가 null일 수 있으므로 예외 처리를 추가
            if (CurrentBuffer.Value == null)
                throw new InvalidOperationException("Buffer has not been opened.");

            // 사용한 크기만큼 닫은 배열 세그먼트를 반환
            return CurrentBuffer.Value.Close(usedSize);
        }
    }
}
