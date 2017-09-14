#include "hud.h"
#include "cl_util.h"
#include "../common/event_api.h"
#include "parsemsg.h"

DECLARE_MESSAGE( m_Score, ScoreDeact )
DECLARE_MESSAGE( m_Score, ScoreValue )
DECLARE_MESSAGE( m_Score, ScoreCheat )

extern globalvars_t *gpGlobals;


int CHudScore::Init(void)
{
	HOOK_MESSAGE( ScoreDeact );
	HOOK_MESSAGE( ScoreValue );
	HOOK_MESSAGE( ScoreCheat );
	
	gHUD.AddHudElem( this );

	return 1;
}

void CHudScore::Reset( void )
{
	m_iFlags = 0;

	cheated = false;
	currentScore = 0;
	comboMultiplier = 1;
	comboMultiplierReset = 0.0f;
}

int CHudScore::Draw( float flTime )
{
	if ( ( gHUD.m_iHideHUDDisplay & HIDEHUD_ALL ) || gEngfuncs.IsSpectateOnly() ) {
		return 1;
	}

	int r = MESSAGE_BRIGHTENESS;
	int g = MESSAGE_BRIGHTENESS;
	int b = MESSAGE_BRIGHTENESS;

	int x = ScreenWidth - CORNER_OFFSET;
	int y = 0;

	std::string numberString = std::to_string( currentScore );
	float formattedScoreSpriteWidth = gHUD.GetNumberSpriteWidth() * ( numberString.size() + ( ( numberString.size() - 1 ) / 3 ) / 2.0f );
	int numberSpriteHeight = gHUD.GetNumberSpriteHeight();

	if ( comboMultiplier > 1 ) {
		int r2 = r;
		int g2 = g;
		int b2 = b;
		int alpha = ( comboMultiplierReset / 8.0f ) * 1024;
		if ( alpha > 255 ) {
			alpha = 255;
		}

		ScaleColors( r2, g2, b2, alpha );
		gHUD.DrawHudStringKeepRight( x, y, 200, ( "x" + std::to_string( comboMultiplier ) ).c_str(), r2, g2, b2 );
	}
	y += CORNER_OFFSET;

	if ( cheated ) {
		gHUD.DrawFormattedNumber( currentScore, x - formattedScoreSpriteWidth, y, r, 0, 0 );
	} else {
		gHUD.DrawFormattedNumber( currentScore, x - formattedScoreSpriteWidth, y, r, g, b );
	}

	return 1;
}

int CHudScore::MsgFunc_ScoreValue( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	currentScore = READ_LONG();
	comboMultiplier = READ_LONG();
	comboMultiplierReset = READ_FLOAT();

	m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudScore::MsgFunc_ScoreCheat( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	cheated = true;

	return 1;
}

int CHudScore::MsgFunc_ScoreDeact( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	this->Reset();

	return 1;
}