#pragma once

//Packet opCode Definition
#define OP_SF_REGISTERFILESERVER 147
#define OP_FS_REGISTERFILESERVERRESP 148

#define OP_SF_AUTHUSER 149
#define OP_FS_AUTHUSERRESP 150

#define OP_CF_LOGINFILE 200
#define OP_FC_LOGINFILERESP 201

#define OP_CF_FILEJOBREQ 202
#define OP_CFFC_FILEINFO 203
#define OP_FC_FILEJOBRESULT 204

#define OP_CF_MOVEDIR 205
#define OP_FC_MOVEDIRRESP 206

#define OP_CF_LISTFILE 207
#define OP_FC_LISTFILERESP 208
#define OP_CF_LISTPAGEMOVE 209
#define OP_FC_LISTPAGEMOVERESP 210

#define OP_CS_FILESRVCONNREQ 211
#define OP_SC_FILESRVCONNRESP 212

#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
		} Data;
		char buf[8];
	} sf_RegisterFileServer;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		} Data;
		char buf[9];
	} fs_RegisterFileServerResp;

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//operation code -> to classify packet
			unsigned long dataLen;
			unsigned char UserFileServerAuthKey[32];//random sha256 hash
		} Data;
		char buf[40];
	} sf_AuthUser;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fs_AuthUserResp;

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//operation code -> to classify packet
			unsigned long dataLen;
			unsigned char UserFileServerAuthKey[32];//random sha256 hash
	} Data;
		char buf[40];
	} cf_LoginFile;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_LoginFileResp;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char jobType; //0 = download, 1 = upload (from the viewpoint of client)
		}Data;
		char buf[9];
	} cf_FileJobReq;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char jobType;  //0 = download, 1 = upload (from the viewpoint of client)
			unsigned char fileName[255]; //since NTFS filesystem's max file name length is 255
			unsigned char fileHash[32];
			unsigned long long fileSize;
		}Data;
		char buf[304];
	} all_FileInfo;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_FileJobResult;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char moveDir[255]; //ntfs max
		}Data;
		char buf[263];
	} cf_MoveDir;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_MoveDirResp;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char pageNum;
		}Data;
		char buf[9];
	} cf_ListFile;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
			unsigned char fileCount; //max 20
			unsigned char fileName[20][255];
		}Data;
		char buf[5110];
	} cf_ListFileResp;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char moveTo; //0 = prev page, 1 = next page
		}Data;
		char buf[9];
	} cf_ListPageMove;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
		}Data;
		char buf[8];
	} cs_FileSrvConReq;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
			unsigned char fileSrvAddr[16];
			unsigned long fileSrvPort;
			unsigned char authKey[32];
		}Data;
		char buf[61];
	} sc_FileSrvConResp;

#pragma pack(pop)
#ifdef __cplusplus
};
#endif