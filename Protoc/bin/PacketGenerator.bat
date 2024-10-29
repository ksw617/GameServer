protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto
IF ERRORLEVEL 1 PAUSE