#pragma once
#include <vector>

class SpriteSheet;
class SpriteDefinition;

enum class SpriteAnimPlaybackType {
	ONCE,
	LOOP,
	PINGPONG,
};

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);
	const SpriteSheet& GetSpriteSheet()const {return m_spriteSheet;}
	const SpriteDefinition& GetSpriteDefAtTime(float seconds) const;

private:
	std::vector<int>		m_animFrames;				// need to implement
	const SpriteSheet&		m_spriteSheet;
	int						m_startSpriteIndex = -1;	// need delete
	int						m_endSpriteIndex = -1;		// need delete
	float					m_durationSeconds = 1.f;
	SpriteAnimPlaybackType	m_playbackType = SpriteAnimPlaybackType::LOOP;
};

