#include "stdafx.h"
#include "Scores.h"
#include "Game/Config/UserProfile.h"

int GetScoreTotal (CampaignScore* score)
{
	return
		score->gold * k_multiplier_gold +
		score->souls * k_multiplier_souls +
		k_difficulty_bonus[score->difficulty];
}

int GetScoreTotal (EndlessScore* score)
{
	return
		score->kills * k_multiplier_kills +
		score->days * k_multiplier_days +
		score->gold * k_multiplier_gold +
		score->souls * k_multiplier_souls;
}

static bool s_lastScoreWasHighScore = false;
bool LastScoreWasHighScore ()
{
	return s_lastScoreWasHighScore;
}

bool HasEndlessScore (CUserProfile& profile)
{
	s32 exists = 0;
	profile.Get("HighScoreExistsForEndless", exists);
	return exists;
}

void GetEndlessScore (CUserProfile& profile, EndlessScore* score)
{
	GX_ASSERT(HasEndlessScore(profile));
	profile.Get("HighScoreEndlessKills", score->kills);
	profile.Get("HighScoreEndlessDays", score->days);
	profile.Get("HighScoreEndlessGold", score->gold);
	profile.Get("HighScoreEndlessSouls", score->souls);
}

void SetEndlessScore (CUserProfile& profile, EndlessScore* score)
{
	s_lastScoreWasHighScore = true;
	profile.Set("HighScoreExistsForEndless", 1);
	profile.Set("HighScoreEndlessKills", score->kills);
	profile.Set("HighScoreEndlessDays", score->days);
	profile.Set("HighScoreEndlessGold", score->gold);
	profile.Set("HighScoreEndlessSouls", score->souls);
}

void NewEndlessScore (CUserProfile& profile, EndlessScore* score)
{
	s_lastScoreWasHighScore = false;

	if( HasEndlessScore(profile) )
	{
		EndlessScore old;
		GetEndlessScore( profile, &old );

		s32 old_score = GetScoreTotal( &old );
		s32 new_score = GetScoreTotal( score );

		if( (new_score > 0) && (old_score < new_score) )
			SetEndlessScore(profile, score);
	}
	else
	{
		s32 new_score = GetScoreTotal( score );

		if( new_score > 0 )
			SetEndlessScore( profile, score );
	}
}

bool HasCampaignScore (CUserProfile& profile, int level)
{
	s32 exists = 0;
	profile.Get(stringc("HighScoreExistsForLevel") + stringc(level + 1), exists);
	return exists;
}

void GetCampaignScore (CUserProfile& profile, CampaignScore* score, int level)
{
	GX_ASSERT(HasCampaignScore(profile, level));
	profile.Get(stringc("HighScoreLevelGold") + stringc(level + 1), score->gold);
	profile.Get(stringc("HighScoreLevelSouls") + stringc(level + 1), score->souls);
	profile.Get(stringc("HighScoreLevelDifficulty") + stringc(level + 1), score->difficulty);
}

void SetCampaignScore (CUserProfile& profile, CampaignScore* score, int level)
{
	s_lastScoreWasHighScore = true;
	profile.Set(stringc("HighScoreExistsForLevel") + stringc(level + 1), 1);
	profile.Set(stringc("HighScoreLevelGold") + stringc(level + 1), score->gold);
	profile.Set(stringc("HighScoreLevelSouls") + stringc(level + 1), score->souls);
	profile.Set(stringc("HighScoreLevelDifficulty") + stringc(level + 1), score->difficulty);
}

void NewCampaignScore (CUserProfile& profile, CampaignScore* score, int level)
{
	s_lastScoreWasHighScore = false;
	if (HasCampaignScore(profile, level))
	{
		CampaignScore old;
		GetCampaignScore(profile, &old, level);
		if (GetScoreTotal(&old) < GetScoreTotal(score))
			SetCampaignScore(profile, score, level);
	}
	else
	{
		SetCampaignScore(profile, score, level);
	}
}
