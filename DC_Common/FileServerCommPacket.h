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
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
		} Data;
		char buf[8];
	} sf_RegisterFileServer;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
		} Data;
		char buf[9];
	} fs_RegisterFileServerResp;

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char UserFileServerAuthKey[32];//random sha256 hash
			unsigned long userUID;//유저 고유번호
		} Data;
		char buf[44];
	} sf_AuthUser;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fs_AuthUserResp;

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char UserFileServerAuthKey[32];//random sha256 hash
	} Data;
		char buf[40];
	} cf_LoginFile;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_LoginFileResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char jobType; //0 = download, 1 = upload (from the viewpoint of client)
		}Data;
		char buf[9];
	} cf_FileJobReq;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char jobType;  //0 = download, 1 = upload (from the viewpoint of client)
			unsigned char fileName[255]; //since NTFS filesystem's max file name length is 255
			unsigned char fileHash[32];//파일 해쉬
			unsigned char IV[16];//IV값은 DB에 저장되거나 복호화에 사용됨
			unsigned long fileSize;//파일 사이즈
		}Data;
		char buf[316];
	} cffc_FileInfo;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_FileJobResult;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char moveDir[255]; //ntfs max
		}Data;
		char buf[263];
	} cf_MoveDir;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fc_MoveDirResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
		}Data;
		char buf[8];
	} cf_ListFile;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
			unsigned char fileCount; //max 10
			unsigned char fileName[10][255];//파일 이름
			unsigned char fileType[10]; //0 = 폴더, 1 = 파일
			unsigned char currentPage;//현재 페이지
			unsigned char totalPage;//전체 페이지
			unsigned char currentDir[255];//현재 디렉토리
		}Data;
		char buf[2827];
	} fc_ListFileResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char moveTo; //0 = prev page, 1 = next page
		}Data;
		char buf[9];
	} cf_ListPageMove;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//상태 코드
		}Data;
		char buf[9];
	} fc_ListPageMoveResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
		}Data;
		char buf[8];
	} cs_FileSrvConReq;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode; //0 = fail, 1 = success
			unsigned char fileSrvAddr[16];//파일서버 주소
			unsigned long fileSrvPort;//파일서버 포트
			unsigned char authKey[32];//파일서버 인증키
		}Data;
		char buf[61];
	} sc_FileSrvConResp;

#pragma pack(pop)
#ifdef __cplusplus
};
#endif