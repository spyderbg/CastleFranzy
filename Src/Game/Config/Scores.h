#pragma once

class CUserProfile;

static const int k_multiplier_kills = 5;
static const int k_multiplier_days  = 1000;
static const int k_multiplier_gold  = 2;
static const int k_multiplier_souls = 100;
static const int k_difficulty_bonus[] = { 1000, 10000, 100000 };

struct CampaignScore
{
	int gold;
	int souls;
	int difficulty;
};

struct EndlessScore
{
	int kills;
	int days;
	int gold;
	int souls;
};

int GetScoreTotal (CampaignScore* score);
int GetScoreTotal (EndlessScore* score);

bool LastScoreWasHighScore ();

bool HasEndlessScore (CUserProfile& profile);
void GetEndlessScore (CUserProfile& profile, EndlessScore* score);
void NewEndlessScore (CUserProfile& profile, EndlessScore* score);

bool HasCampaignScore (CUserProfile& profile, int level);
void GetCampaignScore (CUserProfile& profile, CampaignScore* score, int level);
void NewCampaignScore (CUserProfile& profile, CampaignScore* score, int level);
