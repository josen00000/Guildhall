#pragma once
#include "Game/Network/Client.hpp"
#include "Game/Entity.hpp"

class Server;
class Camera;

class PlayerClient : public Client {
public:
	PlayerClient() = default;
	~PlayerClient();
	explicit PlayerClient( Server* owner, Camera* camera );
	PlayerClient( PlayerClient const& copyFrom ) = delete;
	PlayerClient( PlayerClient const&& moveFrom ) = delete;

	virtual void Startup() override;
	virtual void Shutdown() override;
	
	virtual void Update( float deltaSeconds ) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	void HandleUserInput();
	void PreUpdatePlayer();
	void Render();


	Entity* m_player = nullptr;
	Camera* m_gameCamera = nullptr;
};