/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	networkentity.h
Description	:	Generic class for a network entity
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#ifndef __NETWORKENTITY_H__
#define __NETWORKENTITY_H__

#include <string>
#include <sstream>

enum EMessageType : unsigned char
{
	HANDSHAKE,
	HANDSHAKE_SUCCESS,
	HANDSHAKE_FAIL,
	PLAYER_NUM,
	DATA,
	KEEPALIVE,
	COMMAND,
	ASTRO_BROADCAST,
	GAME_UPDATE
};

struct Packet
{
	unsigned short messageType;
	char messageContent[50];
	char packetData[60];
	unsigned short packetSize;

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void Serialize(EMessageType _MessageType, const char* _message)
	{

		messageType = _MessageType;
		strcpy_s(messageContent, strlen(_message) + 1, _message);

		std::ostringstream oss;
		oss << messageType;
		oss << " ";
		oss << messageContent;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(packetData, strlen(_pcToSend) + 1, _pcToSend);

		packetSize = static_cast<unsigned short>(_strToSend.size());
	}

	Packet Deserialize(const char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->messageType;

		//iss >> this->MessageContent;
		std::string _tempMessageBuffer;
		getline(iss, _tempMessageBuffer);
		strcpy_s(messageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());

		return *this;
	}

};

class NetworkEntity
{
public:
	virtual bool Initialise() = 0; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend) = 0;
	virtual void ReceiveData(char* _pcBufferToReceiveData, int& isEnd) = 0;
	virtual void GetRemoteIPAddress(char *_pcSendersIP) = 0;
	virtual unsigned short GetRemotePort() = 0;

	void SetIsOnline(bool _isOnline) { isOnline = _isOnline; };

protected:
	//Additional state variable to indicate whether a network entity is online or not
	bool isOnline;
};
#endif 