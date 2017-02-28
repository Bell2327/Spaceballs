#include "BigEnemyShip.h"

BigEnemyShip::BigEnemyShip(float X, const Surface & surface, BulletManager& BulletM, AnimationFrames& ExploAnim)
	:
	pos(X, -100.0f),
	shipSurface(surface),
	resetX(X),
	bulletM(BulletM),
	exploAnim(ExploAnim, 2)
{}

void BigEnemyShip::Draw(Graphics & gfx)
{
	switch (state)
	{
	case AliveState:
		gfx.DrawSpriteKey(int(pos.x), int(pos.y), shipSurface, shipSurface.GetPixel(0, 0));
		bulletM.DrawBullets(gfx);
		break;
		
	case DyingState:
		exploAnim.Draw(int(pos.x), int(pos.y), gfx);
		break;
	}
}

void BigEnemyShip::Update(float dt)
{
	bulletM.UpdateBullets(dt);
	switch (state)
	{
	case WaitState:
		if ((shipTimer += dt) > waitOver)
		{
			shipTimer = 0.0f;
			state = AliveState;
		}
		break;
	case AliveState:
		Move(dt);
		break;

	case DyingState:
		exploAnim.Advance();
		if (exploAnim.AnimEnd())
		{
			Reset();
			state = WaitState;
		}
		break;
	}
}

void BigEnemyShip::Reset()
{
	pos.y = -100.0f;
	pos.x = resetX;
	hp = 200;
	state = WaitState;
	bulletTimer = 0.0f;
	exploAnim.Reset();
	vel.x = 100.0f;
	shipTimer = 0.0f;
}

RectF BigEnemyShip::GetCollisionRect() const
{
	return RectF(pos, width, height);
}

int BigEnemyShip::GetCollisionDmg() const
{
	return collisionDmg;
}

int BigEnemyShip::GetBulletDmg() const
{
	return bulletDmg;
}

void BigEnemyShip::HandleCollision(int dmg)
{
	if (state == AliveState)
	{
		hp -= dmg;
	}
}

void BigEnemyShip::Attack(float dt)
{
	if ((bulletTimer += dt) > newBullet)
	{
		Vec2 canonPos = pos + canon;
		bulletTimer = 0.0;
		bulletM.FireBullet(canonPos, bulletVel, bulletColor, bulletSize, bulletDmg);
		bulletM.ResetShotsFired();
	}
}

void BigEnemyShip::Move(float dt)
{
	if (pos.y < 25.0f && shipTimer < waitOver)
	{
		pos.y += vel.y * dt;
	}
	else if (shipTimer < waitOver)
	{
		pos.x += vel.x * dt;
		Attack(dt);

		shipTimer += dt;
	}
	if ((pos.y > -100.0f) && shipTimer > waitOver)
	{
		pos.y -= vel.y * dt;
	}
	else if (pos.y <= -100.0f)
	{
		Reset();
		state = WaitState;
	}

	if ((pos.x <= 0 && vel.x < 0.0f) ||
		((pos.x + width) >= Graphics::ScreenWidth && vel.x > 0.0f))
	{
		vel.x = -vel.x;
	}
}
