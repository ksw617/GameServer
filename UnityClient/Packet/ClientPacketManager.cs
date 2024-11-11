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
        private static readonly ClientPacketManager instance = new ();
        public static ClientPacketManager Instance { get { return instance; } }

        public override void Initialize()
        {
            onRecv.Add((ushort)PacketID.PacketSLogin, MakePacket<S_LOGIN>);
            handler.Add((ushort)PacketID.PacketSLogin, PacketHandler.S_LOGIN_Handler);
            onRecv.Add((ushort)PacketID.PacketSEnterGame, MakePacket<S_ENTER_GAME>);
            handler.Add((ushort)PacketID.PacketSEnterGame, PacketHandler.S_ENTER_GAME_Handler);
            onRecv.Add((ushort)PacketID.PacketSJoinGame, MakePacket<S_JOIN_GAME>);
            handler.Add((ushort)PacketID.PacketSJoinGame, PacketHandler.S_JOIN_GAME_Handler);
            onRecv.Add((ushort)PacketID.PacketSChat, MakePacket<S_CHAT>);
            handler.Add((ushort)PacketID.PacketSChat, PacketHandler.S_CHAT_Handler);

        }
    }

}
