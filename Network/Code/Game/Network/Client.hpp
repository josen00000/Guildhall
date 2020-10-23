#pragma once

class Server;

class Client {
public:
	Client() = default;
	virtual ~Client();
	explicit Client( Server* owner );
	Client( Client const& copyFrom ) = delete;
	Client( Client const&& moveFrom ) = delete;

	virtual void Startup();
	virtual void Shutdown();
	virtual void BeginFrame();
	virtual void Update( float deltaSeconds );
	virtual void EndFrame();
	virtual void Render();

	Server* m_owner = nullptr;
};