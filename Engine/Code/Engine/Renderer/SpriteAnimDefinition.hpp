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
	SpriteAnimDefinition(const SpriteSheet& sheet, int* spriteIndexes, int spriteIndexNum, float durationSeconds, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);
	const SpriteSheet& GetSpriteSheet()const {return m_spriteSheet;}
	const SpriteDefinition& GetSpriteDefAtTime(float seconds) const;
	// Accessor
	int GetAnimFramesNum() const { return (int)m_animFrames.size(); }

private:
	std::vector<int>		m_animFrames;				// need to implement
	int						m_framesNum;
	const SpriteSheet&		m_spriteSheet;
	float					m_durationSeconds = 1.f;
	SpriteAnimPlaybackType	m_playbackType = SpriteAnimPlaybackType::LOOP;
};

