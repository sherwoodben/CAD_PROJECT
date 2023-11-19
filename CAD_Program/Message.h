#pragma once

#include <string>
#include <queue>

struct Message
{
	std::string messageType;
	std::string messageData;
};


class MessageManager
{
private:
	std::queue<Message> messageQueue;

public:
	void ReceiveMessage(Message newMessage) { this->messageQueue.push(newMessage); };
	Message DispatchMessage() { Message messageToSend = messageQueue.front(); messageQueue.pop(); return messageToSend; }
	bool QueueHasMessages() { return !this->messageQueue.empty(); };
};
