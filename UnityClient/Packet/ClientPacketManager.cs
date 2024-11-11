using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UnityClient.Packet
{
    internal class ClientPacketManager : PacketManager
    {
        private static readonly ClientPacketManager instance = new();
        public static ClientPacketManager Instance { get { return instance; } }

        public override void Initialize()
        {
            onRecv.Add((ushort)PacketID.SLogin, MakePacket<S_Login>);
            handler.Add((ushort)PacketID.SLogin, PacketHandler.S_LOGIN_Handler);
            onRecv.Add((ushort)PacketID.SEnterGame, MakePacket<S_EnterGame>);
            handler.Add((ushort)PacketID.SEnterGame, PacketHandler.S_ENTER_GAME_Handler);
            onRecv.Add((ushort)PacketID.SJoinGame, MakePacket<S_JoinGame>);
            handler.Add((ushort)PacketID.SJoinGame, PacketHandler.S_JOIN_GAME_Handler);
            onRecv.Add((ushort)PacketID.SChat, MakePacket<S_Chat>);
            handler.Add((ushort)PacketID.SChat, PacketHandler.S_CHAT_Handler);

        }
    }

}
