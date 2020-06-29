#include "SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"


SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int* spriteIndexes, int spriteIndexNum, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/ )
	:m_spriteSheet(sheet)
	,m_durationSeconds(durationSeconds)
	,m_playbackType(playbackType)
{
	for( int i = 0; i < spriteIndexNum; i++ ) {
		m_animFrames.push_back( spriteIndexes[i] );
	}
}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int totalIndex = (int)m_animFrames.size();
	float totalDurationSeconds = (totalIndex) * m_durationSeconds;
	switch (m_playbackType)
	{
		case SpriteAnimPlaybackType::ONCE :
		{
			if(seconds < 0){
				const SpriteDefinition&	result = m_spriteSheet.GetSpriteDefinition( m_animFrames[0] );
				return result;
			}
			else{
				float temIndexFloat =  seconds / m_durationSeconds; 
				int temIndex = RoundDownToInt( temIndexFloat );
				if( temIndex < m_animFrames.size() ){
					const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames[temIndex] );
					return result;
				}
				else{
					const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames.back() );
					return result;
				}
			}
		}
		case SpriteAnimPlaybackType::LOOP:
		{
			while(seconds < 0){
				seconds += totalDurationSeconds;
			}
			while(seconds > totalDurationSeconds){
				seconds -= totalDurationSeconds;
			}

			float temIndexFloat = seconds / m_durationSeconds;
			int temIndex = RoundDownToInt( temIndexFloat );
			const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames[temIndex] );
			return result;
		}
		case SpriteAnimPlaybackType::PINGPONG:
		{
			int halfDuration = totalIndex;
			float totalDurationPingSeconds = halfDuration * 2 * m_durationSeconds;

			while( seconds < 0 ) {
				seconds += totalDurationPingSeconds;
			}
			while( seconds > totalDurationPingSeconds ) {
				seconds -= totalDurationPingSeconds;
			}

			float temIndexFloat = seconds / m_durationSeconds;
			int temIndex = RoundDownToInt(temIndexFloat);
			if(temIndex < m_animFrames.size() ){
				const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames[temIndex] );
				return result;
			}
			else{
				temIndex -= (int)m_animFrames.size();
				int nextIndex = (int)m_animFrames.size() - temIndex;
				const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames[nextIndex] );
				return result;
			}
		}
	}
	const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( m_animFrames[0] );
	return result;
}
