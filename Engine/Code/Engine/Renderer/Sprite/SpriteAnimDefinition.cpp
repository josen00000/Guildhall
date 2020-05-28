#include "SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Sprite/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/ )
	:m_spriteSheet(sheet)
	,m_startSpriteIndex(startSpriteIndex)
	,m_endSpriteIndex(endSpriteIndex)
	,m_durationSeconds(durationSeconds)
	,m_playbackType(playbackType)
{

}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int totalIndex = m_endSpriteIndex - m_startSpriteIndex;
	float totalDurationSeconds = (totalIndex + 1) * m_durationSeconds;
	switch (m_playbackType)
	{
		case SpriteAnimPlaybackType::ONCE :
		{
			if(seconds < 0){
				const SpriteDefinition&	result = m_spriteSheet.GetSpriteDefinition(m_startSpriteIndex);
				return result;
			}
			else{
				float temIndexFloat =  seconds / m_durationSeconds; 
				int temIndex = RoundDownToInt(temIndexFloat);
				temIndex += m_startSpriteIndex;
				if(temIndex < m_endSpriteIndex){
					const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition(temIndex);
					return result;
				}
				else{
					const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition(m_endSpriteIndex);
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
			int temIndex = RoundDownToInt(temIndexFloat);
			temIndex += m_startSpriteIndex;
			const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition(temIndex);
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
			temIndex += m_startSpriteIndex;
			if(temIndex < m_endSpriteIndex){
				const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition(temIndex);
				return result;
			}
			else{
				temIndex -= m_endSpriteIndex;
				int nextIndex = m_endSpriteIndex - temIndex;
				const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition(nextIndex);
				return result;
			}
		}
	}
	const SpriteDefinition& result = m_spriteSheet.GetSpriteDefinition( 0 );
	return result;
}
