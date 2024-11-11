pushd %~dp0

protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto

protoc.exe -I=./ --csharp_out=./ ./Struct.proto 
protoc.exe -I=./ --csharp_out=./ ./Protocol.proto 

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Protocol.pb.h "../../GameClient"
XCOPY /Y Protocol.pb.cc "../../GameClient"
XCOPY /Y Struct.pb.h "../../GameClient"
XCOPY /Y Struct.pb.cc "../../GameClient"

XCOPY /Y Protocol.pb.h "../../GameServer"
XCOPY /Y Protocol.pb.cc "../../GameServer"
XCOPY /Y Struct.pb.h "../../GameServer"
XCOPY /Y Struct.pb.cc "../../GameServer"


XCOPY /Y Protocol.cs "../../UnityClient/Packet"
XCOPY /Y Struct.cs "../../UnityClient/Packet"