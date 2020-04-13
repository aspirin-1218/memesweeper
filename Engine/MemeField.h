#pragma once
#include "Graphics.h"
#include "RectI.h"
class MemeField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};

		void SpawnMine();
		bool HasMeme() const;
		void Draw(const Vei2 & screenPos, Graphics & gfx) const;
	private:
		State state = State::Hidden;
		bool hasMeme = false;
	};
public:
	MemeField( int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	~MemeField();
private:
	Tile& TileAt(Vei2 &gridPos);
	const Tile& TileAt(Vei2 &gridPos) const;
private:
	static constexpr int width = 20; //width shall be initialized as constant at compile-time.
	static constexpr int height = 16;
	Tile field[width*height];
};

