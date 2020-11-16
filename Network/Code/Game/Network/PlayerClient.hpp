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
	virtual void Render() override;
	virtual void HandleInput( std::string input ) override;
	void RenderUI();
	void RenderBaseHud();
	void RenderGun();
	void RequestServerCreatePlayer();

	// audio
	void PlayAudioWithSoundID( size_t id );

	void SetUICamera( Camera* camera );
	
	/*Entity* m_entity = nullptr;*/
	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
};