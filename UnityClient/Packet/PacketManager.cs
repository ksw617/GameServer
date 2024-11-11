using Google.Protobuf;
using System.Reflection.Metadata;
using UnityClient.Network;

namespace UnityClient.Packet
{
    public abstract class PacketManager
    {
        protected Dictionary<ushort, Action<PacketSession, IMessage>> handler = new();
        protected Dictionary<ushort, Action<ushort, PacketSession, ArraySegment<byte>>> onRecv = new();
        protected PacketManager()
        {
            Initialize();
        }

        public abstract void Initialize();

        public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
        {
            if (buffer.Array == null) { return; }

            ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + sizeof(ushort));

            Action<ushort, PacketSession, ArraySegment<byte>>? action;

            if (onRecv.TryGetValue(id, out action))
            {
                action.Invoke(id, session, buffer);
            }
        }

        protected void MakePacket<T>(ushort id, PacketSession session, ArraySegment<byte> buffer) where T : IMessage, new()
        {
            T packet = new();
            packet.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);
            Action<PacketSession, IMessage>? action;
            if (handler.TryGetValue(id, out action))
            {
                action.Invoke(session, packet);
            }
        }

        public Action<PacketSession, IMessage>? GetPacketHandler(ushort id)
        {
            Action<PacketSession, IMessage>? action;
            if (handler.TryGetValue(id, out action))
                return action;
            return null;
        }
    }

}
