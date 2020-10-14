#pragma once

class Client {
	Client() = default;
	virtual ~Client();
	Client( Client const& copyFrom ) = delete;
	Client( Client const&& moveFrom ) = delete;

};