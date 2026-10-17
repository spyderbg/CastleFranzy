///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

class ASprite;

///////////////////////////////////////////////////////////////////////

class ASpriteInstance
{
	public:	
		ASpriteInstance();
		~ASpriteInstance();

		void Stop();

		void SetAnim(int anim, int loop = 0xFFFFFF);
		void SetPosition(int x, int y);
		void SetASprite(ASprite* sprite);
		ASprite* GetASprite () const;

		int GetCurrentAnim() const { return m_nCurrentAnim; }

		int GetFrame () const { return m_nCurrentAFrame; }
		int GetX () const { return x; }
		int GetY () const { return y; }

		void StartAnim(ASprite* sprite, int anim, int x, int y, int loop = 0xFFFFFF);

		void SetTransitionAnim(int anim, int nextAnim);

		bool IsAnimEnded();

		void Update(float time);
		void Draw(int alpha = 255);

		void Draw3D(const irr::core::vector3df& pos, float scale = 1.0f);

	protected:
		int x;
		int y;

		int m_nCurrentAnim;
		int m_nCurrentAFrame;
		float m_nCurrentFTime;
		int m_nLoop;

		int m_nNextAnim;

		ASprite* m_nCurrentSprite;
};

