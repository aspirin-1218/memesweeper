#include "MemeField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"


MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width*height);
	std::random_device rd;
	std::mt19937 rng(rd());  //generate a random 32 bit number
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; ++nSpawned) {
		Vei2 spawnPos;
		do {
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());

		TileAt(spawnPos).SpawnMine();
	}
	
	//reveal test
	for (int i = 0; i < 120; i++)
	{
		const Vei2 gridPos = { xDist(rng), yDist(rng) };
		if (!TileAt(gridPos).IsRevealed()) {
			TileAt(gridPos).Reveal();
		}
		
	}


}

void MemeField::Draw(Graphics & gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).Draw(gridPos*SpriteCodex::tileSize, gfx);
		}
	}

}

RectI MemeField::GetRect() const
{
	return RectI(0, width*SpriteCodex::tileSize, 0, height*SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2 & screenPos)
{
	const Vei2 & gridPos = MemeField::Screen2Grid(screenPos);
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed()) {
		tile.Reveal();
	}
}


Vei2 & MemeField::Screen2Grid(const Vei2 & screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}

MemeField::~MemeField()
{
}

MemeField::Tile& MemeField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.y*width + gridPos.x];
}

const MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos) const
{
	return field[gridPos.y*width + gridPos.x];
}

void MemeField::Tile::SpawnMine()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return false;
}

void MemeField::Tile::Draw(const Vei2 & screenPos, Graphics & gfx) const
{
	switch (state) {
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Revealed:
		if (!HasMeme()) {
			SpriteCodex::DrawTile0(screenPos, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		break;
	}
}

void MemeField::Tile::Reveal()
{
	assert(State::Hidden == state);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}
