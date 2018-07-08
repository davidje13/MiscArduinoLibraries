/*
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef GAME_INVADERS_INCLUDED
#define GAME_INVADERS_INCLUDED

#include "InvaderResources.h"
#include "Strings.h"

#include <Vector.h>
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <Font.h>
#include <FontRenderer.h>
#include <FontTiny.h>
#include <FontVariable.h>
#include <ProgMem.h>

// Thanks,
//  http://computerarcheology.com/Arcade/SpaceInvaders/
//  https://www.walkofmind.com/programming/side/soundboard/sound_step.htm

// TODO:
//  Invader missile types (3 types, each with different behaviours)
//  Flying saucer? (enough space?)

static PROGMEM const uint8_t invaderHitWidths[3] = {
	INVADER_WIDTH - 2,
	INVADER_WIDTH - 2,
	INVADER_WIDTH - 4
};
static PROGMEM const uint8_t invaderKillScores[3] = {1, 2, 3};

// Step frequencies:
//  freq = 1 / (log(2) * (Ra + 2 * Rb) * C)
//  Rb = 75000, C = 0.0000001
//  Ra = 40000, 68000, 82000, 100000
//  freq = 1 / (Ra * 0.000000069314718 + 0.010397207708399)
//  freq = 75.93Hz
//  freq = 66.18Hz
//  freq = 62.19Hz
//  freq = 57.71Hz
static PROGMEM const uint8_t stepFrequencies[4] = {76, 66, 62, 58};

template <
	uint8_t w,
	uint8_t h,
	uint8_t invadersX = 6,
	uint8_t invadersY = 3,
	uint8_t shieldCount = 4,

	uint8_t bottom = h - 8,
	int8_t shipY = bottom - 2 - DEFENDER_SHIP_HEIGHT,
	int8_t shipBufferL = DEFENDER_SHIP_WIDTH / 2 + 2,
	int8_t shipBufferR = shipBufferL + 2,
	int8_t invaderBufferL = INVADER_WIDTH / 2 + 2,
	int8_t invaderBufferR = invaderBufferL,
	int8_t shieldsX = 10,
	int8_t shieldsY = bottom - 16,
	int8_t shieldsDX = (
		(w - shieldsX * 2 - SHIELD_WIDTH * shieldCount) / (shieldCount - 1) +
		SHIELD_WIDTH
	),
	int8_t invaderDX = 14,
	int8_t invaderDY = 9,
	int8_t invaderSpeedX = 2,
	int8_t invaderSpeedY = 4,
	int8_t lastInvaderSpeedXR = 3,
	int8_t lastInvaderSpeedXL = 2,
	int8_t invaderWinY = shipY - INVADER_HEIGHT + 2,

	uint8_t missileHeight = 4,
	uint8_t missileDepth = 1,
	uint8_t missileSpeed = 1,
	uint8_t missileWidth = 2,
	uint8_t fastInvaderMissileSpeed = 2,
	uint8_t fastInvaderMissileCriteria = 4,

	uint8_t explosionDuration = 5,
	uint8_t fireAnimDelay = 2,

	uint8_t whiteNoiseDelay = 6,

	uint8_t fireDuration = 25,
	uint8_t respawnDelay = 50,
	uint8_t winDelay = 75,
	uint8_t allLivesLostDelay = 75,
	uint8_t invaderLandedDelay = 75,

	uint8_t scoreWin = 10
>
class GameInvaders {
	typedef vector2d<uint8_t> ind_t;
	typedef vector2d<int8_t> pos_t;

	enum class State : uint8_t {
		PLAYING,
		SHIP_FIRE,
		RESPAWNING,
		WINNING,
		GAMEOVER
	};

	uint8_t moveFrame;
	uint8_t animFrame;

	Bitmask18<invadersX, invadersY> invaders;
	uint8_t invaderTypes[invadersY];
	pos_t invaderOrigin;
	bool moveRight;

	Bitmask18<w,SHIELD_HEIGHT> shields;

	int8_t shipX;
	int8_t crudeShipX;
	uint8_t lives;
	uint16_t _score;

	pos_t playerMissile;
	pos_t invaderMissile;
	uint8_t invaderMissileCooldown;

	pos_t explosion;
	uint8_t explosionFrame;

	uint8_t nextSound;
	uint8_t soundDelay;

	State state;
	uint8_t delaying;

	template <typename Fn>
	void visit_invaders(Fn fn) const {
		for(uint8_t y = 0; y < invadersY; ++ y) {
			for(uint8_t x = 0; x < invadersX; ++ x) {
				if(invaders.get_pixel(x, y)) {
					fn(ind_t(x, y));
				}
			}
		}
	}

	pos_t invader_position(ind_t index) {
		return pos_t(
			invaderOrigin.x + index.x * invaderDX,
			invaderOrigin.y + index.y * invaderDY
		);
	}

	ind_t invader_index(uint8_t sequence) const {
		ind_t r;
		visit_invaders([&sequence, &r] (ind_t index) {
			if(!sequence) {
				r = index;
			}
			-- sequence;
		});
		return r;
	}

	uint8_t count_invaders(void) const {
		uint8_t invaderCount = 0;
		visit_invaders([&invaderCount] (ind_t) {
			++ invaderCount;
		});
		return invaderCount;
	}

	uint8_t invader_cooldown_delay(void) const {
		if(_score < 20) {
			return 40;
		} else if(_score < 60) {
			return 30;
		} else if(_score < 100) {
			return 15;
		} else if(_score < 200) {
			return 10;
		} else if(_score < 300) {
			return 8;
		} else {
			return 7;
		}
	}

	template <typename PosCtl>
	void update_defender_pos(PosCtl *posCtl) {
		crudeShipX += posCtl->delta() * 2;
		if(crudeShipX < shipBufferL) {
			crudeShipX = shipBufferL;
		}
		if(crudeShipX > w - shipBufferR) {
			crudeShipX = w - shipBufferR;
		}
		shipX = crudeShipX + posCtl->fraction() / 2;
	}

	void walk_invaders(void) {
		ind_t left(invadersX, 0);
		ind_t right(0, 0);
		ind_t base(0, 0);
		uint8_t count = 0;
		visit_invaders([&left, &right, &base, &count] (ind_t index) {
			if(index.x < left.x) {
				left = index;
			}
			if(index.x > right.x) {
				right = index;
			}
			if(index.y > base.y) {
				base = index;
			}
			++ count;
		});

		int8_t velX;
		if(count == 1) {
			velX = (moveRight ? lastInvaderSpeedXR : -lastInvaderSpeedXL);
		} else {
			velX = (moveRight ? invaderSpeedX : -invaderSpeedX);
		}
		if(
			invader_position(left).x + velX < invaderBufferL ||
			invader_position(right).x + velX > w - invaderBufferR
		) {
			invaderOrigin.y += invaderSpeedY;
			moveRight = !moveRight;
			if(invader_position(base).y >= invaderWinY) {
				delaying = invaderLandedDelay;
				state = State::GAMEOVER;
			}
		} else {
			invaderOrigin.x += velX;
			animFrame = 1 - animFrame;
		}
	}

	void fire_defender_missile(void) {
		playerMissile = pos_t(shipX, shipY);
	}

	void fire_invader_missile(ind_t index) {
		invaderMissile = invader_position(index);
		invaderMissile.y += INVADER_HEIGHT - missileHeight;
		invaderMissileCooldown = invader_cooldown_delay();
	}

	template <typename PosCtl, typename FireCtl>
	void step_defender(PosCtl *posCtl, FireCtl *fireCtl) {
		update_defender_pos(posCtl);
		if(!playerMissile.x && fireCtl->held()) {
			fire_defender_missile();
		}
	}

	bool step_delay(void) {
		if(delaying) {
			-- delaying;
		}
		return delaying == 0;
	}

	void step_fire(void) {
		if(step_delay()) {
			state = State::RESPAWNING;
			delaying = respawnDelay;
		}
	}

	void step_respawn(void) {
		if(step_delay()) {
			shipX = crudeShipX = w / 2;
			invaderMissileCooldown = invader_cooldown_delay();
			-- lives;
			if(lives) {
				state = State::PLAYING;
			} else {
				delaying = allLivesLostDelay;
				state = State::GAMEOVER;
			}
		}
	}

	uint8_t sound_delay(void) const {
		const auto count = count_invaders();
		if(count < 12) {
			return 8 + count * 2;
		} else {
			return 32 + (count - 12) * 1;
		}
	}

	void _reset(void) {
		for(uint8_t i = 0; i < shieldCount; ++ i) {
			shields.render_bitmap(
				SHIELD,
				nullptr,
				shieldsX + i * shieldsDX,
				0,
				SHIELD_WIDTH,
				SHIELD_HEIGHT,
				SHIELD_STEP
			);
		}

		invaders.clear(BlendMode::ON);

		invaderOrigin = pos_t(invaderBufferL, 0);
		moveRight = true;
		moveFrame = 0;

		playerMissile.x = 0;
		invaderMissile.x = 0;
		invaderMissileCooldown = invader_cooldown_delay();

		nextSound = 0;
		soundDelay = sound_delay();

		delaying = 0;
		state = State::PLAYING;
	}

	void step_win(void) {
		if(step_delay()) {
			_reset();
		}
	}

	void step_gameover(void) {
		step_delay();
	}

	void step_explosions(void) {
		if(explosionFrame) {
			-- explosionFrame;
		}
	}

	static uint8_t invader_hit_width(uint8_t invaderType) {
		return MakeProgMem(invaderHitWidths)[invaderType];
	}

	void trample_shields(void) {
		visit_invaders([this] (ind_t index) {
			const uint8_t hitWidth = invader_hit_width(invaderTypes[index.y]);
			auto p = invader_position(index);
			shields.fill_rect(
				p.x - hitWidth / 2,
				p.y - shieldsY,
				hitWidth,
				INVADER_HEIGHT,
				BlendMode::OFF
			);
		});
	}

	void step_invaders(void) {
		const uint8_t invaderCount = count_invaders();
		if(!invaderCount) {
			return;
		}
		if(invaderMissileCooldown) {
			-- invaderMissileCooldown;
		} else if(!invaderMissile.x) {
			fire_invader_missile(invader_index(random() % invaderCount));
		}

		uint8_t movementDelay = 2 + invaderCount * 2;
		if((++ moveFrame) >= movementDelay) {
			moveFrame = 0;
			walk_invaders();
			trample_shields();
		}
	}

	void destroy_shield(pos_t p) {
		shields.render_bitmap(
			HOLE,
			HOLE,
			p.x - HOLE_WIDTH / 2,
			p.y - shieldsY - HOLE_HEIGHT / 2,
			HOLE_WIDTH,
			HOLE_HEIGHT,
			HOLE_STEP,
			BlendMode::OFF
		);
	}

	bool try_destroy_shield(pos_t p) {
		int8_t hw = missileWidth / 2;
		for(int8_t xx = p.x - hw; xx < p.x + hw; ++ xx) {
			if(shields.get_pixel(xx, p.y - shieldsY)) {
				destroy_shield(p);
				return true;
			}
		}
		return false;
	}

	void destroy_invader(ind_t index) {
		invaders.set_pixel(index.x, index.y, BlendMode::OFF);

		explosion = invader_position(index);
		explosion.y += INVADER_HEIGHT / 2;
		explosionFrame = explosionDuration;

		_score += MakeProgMem(invaderKillScores)[invaderTypes[index.y]];

		if(count_invaders() == 0) {
			_score += scoreWin;
			state = State::WINNING;
			delaying = winDelay;
			invaderMissile.x = 0;
		}
	}

	bool try_destroy_invader(pos_t p) {
		if(p.y < invaderOrigin.y || p.x + INVADER_WIDTH / 2 < invaderOrigin.x) {
			return false;
		}
		ind_t index(
			(p.x + INVADER_WIDTH / 2 - invaderOrigin.x) / invaderDX,
			(p.y - invaderOrigin.y) / invaderDY
		);
		if(!invaders.get_pixel(index.x, index.y)) {
			return false;
		}
		const pos_t invaderPos = invader_position(index);
		const uint8_t hitWidth = invader_hit_width(invaderTypes[index.y]);
		if(
			p.x + missileWidth / 2 <= invaderPos.x - hitWidth / 2 ||
			p.x - missileWidth / 2 >= invaderPos.x + hitWidth / 2
		) {
			return false;
		}
		destroy_invader(index);
		return true;
	}

	void destroy_defender(void) {
		delaying = fireDuration;
		state = State::SHIP_FIRE;
	}

	bool try_destroy_defender(pos_t p) {
		if(p.x < shipX - DEFENDER_SHIP_WIDTH / 2 || p.x >= shipX + DEFENDER_SHIP_WIDTH / 2) {
			return false;
		}
		if(p.y < shipY) {
			return false;
		}
		destroy_defender();
		return true;
	}

	bool check_missile_wipeout(pos_t p1, pos_t p2) const {
		if(p1.x - missileWidth >= p2.x || p2.x - missileWidth >= p1.x) {
			return false;
		}
		if(p1.y - missileHeight > p2.y || p2.y - missileHeight > p1.y) {
			return false;
		}
		return true;
	}

	void step_missiles(void) {
		if(playerMissile.x) {
			if(playerMissile.y <= -missileHeight) {
				playerMissile.x = 0;
			} else {
				playerMissile.y -= missileSpeed;
			}
		}
		if(invaderMissile.x) {
			uint8_t speed;
			if(count_invaders() <= fastInvaderMissileCriteria) {
				speed = fastInvaderMissileSpeed;
			} else {
				speed = missileSpeed;
			}

			if(invaderMissile.y > bottom - speed - missileHeight) {
				invaderMissile.x = 0;
			} else {
				invaderMissile.y += speed;
			}
		}
	}

	void check_missile_collisions(void) {
		pos_t pD(
			playerMissile.x,
			playerMissile.y + missileDepth
		);
		pos_t pI(
			invaderMissile.x,
			invaderMissile.y + missileHeight - 1 - missileDepth
		);
		if(playerMissile.x && invaderMissile.x && check_missile_wipeout(pD, pI)) {
			playerMissile.x = 0;
			invaderMissile.x = 0;
			return;
		}
		if(playerMissile.x) {
			if(try_destroy_shield(pD)) {
				playerMissile.x = 0;
			} else if(try_destroy_invader(pD)) {
				playerMissile.x = 0;
			}
		}
		if(invaderMissile.x) {
			if(try_destroy_shield(pI)) {
				invaderMissile.x = 0;
			} else if(try_destroy_defender(pI)) {
				invaderMissile.x = 0;
			}
		}
	}

	template <typename Bitmask>
	void render_ground(Bitmask &bitmask) const {
		bitmask.fill_rect(
			0,
			bottom,
			w,
			1,
			BlendMode::ON,
			PATTERN_CHECKER
		);
	}

	template <typename Bitmask>
	void render_lives(Bitmask &bitmask) const {
		for(uint8_t i = 0; i < lives - 1; ++ i) {
			bitmask.render_bitmap(
				DEFENDER_SHIP,
				DEFENDER_SHIP,
				2 + i * (DEFENDER_SHIP_WIDTH + 2),
				h - DEFENDER_SHIP_HEIGHT - 1,
				DEFENDER_SHIP_WIDTH,
				DEFENDER_SHIP_HEIGHT,
				DEFENDER_SHIP_STEP
			);
		}
	}

	template <typename Bitmask>
	void render_score(Bitmask &bitmask) const {
		auto font = MakeFontTiny();
		auto r = MakeFontRenderer(&bitmask, 0, 0);
		r.move(w - 1 - 5 * 4, bottom + 2);
		r.print_number(font, _score, 4);
		r.print(font, '0');
	}

	template <typename Bitmask>
	void render_defender(Bitmask &bitmask) const {
		if(!lives) {
			return;
		}

		uint8_t img;
		switch(state) {
		case State::RESPAWNING:
			return;
		case State::SHIP_FIRE:
			img = 1 + (delaying / fireAnimDelay) % 2;
			break;
		default:
			img = 0;
			break;
		}

		auto data = DEFENDER_SHIP + (img * DEFENDER_SHIP_WIDTH);
		bitmask.render_bitmap(
			data,
			data,
			shipX - DEFENDER_SHIP_WIDTH / 2,
			shipY,
			DEFENDER_SHIP_WIDTH,
			DEFENDER_SHIP_HEIGHT,
			DEFENDER_SHIP_STEP
		);
	}

	template <typename Bitmask>
	void render_shields(Bitmask &bitmask) const {
		bitmask.render_bitmap(
			shields.raw(),
			shields.raw(),
			0,
			shieldsY,
			shields.width(),
			shields.height(),
			shields.raw_step()
		);
	}

	template <typename Bitmask>
	void render_invader(Bitmask &bitmask, pos_t p, uint8_t type) const {
		auto data = (
			INVADERS +
			(animFrame + type * 2) * INVADER_WIDTH
		);
		bitmask.render_bitmap(
			data,
			data,
			p.x - INVADER_WIDTH / 2,
			p.y,
			INVADER_WIDTH,
			INVADER_HEIGHT,
			INVADER_STEP
		);
	}

	template <typename Bitmask>
	void render_invaders(Bitmask &bitmask) const {
		visit_invaders([this, &bitmask] (ind_t index) {
			const auto pos = invader_position(index);
			render_invader(bitmask, pos, invaderTypes[index.y]);
		});
	}

	template <typename Bitmask>
	void render_missile(Bitmask &bitmask, pos_t p) const {
		bitmask.fill_rect(
			p.x - missileWidth / 2,
			p.y,
			missileWidth,
			missileHeight,
			BlendMode::ON,
			PATTERN_CHECKER
		);
	}

	template <typename Bitmask>
	void render_explosion(Bitmask &bitmask, pos_t p) const {
		bitmask.render_bitmap(
			EXPLOSION,
			EXPLOSION,
			p.x - EXPLOSION_WIDTH / 2,
			p.y - EXPLOSION_HEIGHT / 2,
			EXPLOSION_WIDTH,
			EXPLOSION_HEIGHT,
			EXPLOSION_STEP
		);
	}

	template <typename Bitmask>
	void render_explosions(Bitmask &bitmask) const {
		if(explosionFrame) {
			render_explosion(bitmask, explosion);
		}
	}

	template <typename Bitmask>
	void render_defender_missiles(Bitmask &bitmask) const {
		if(playerMissile.x) {
			render_missile(bitmask, playerMissile);
		}
	}

	template <typename Bitmask>
	void render_invader_missiles(Bitmask &bitmask) const {
		if(invaderMissile.x) {
			render_missile(bitmask, invaderMissile);
		}
	}

	template <typename Bitmask>
	void render_gameover(Bitmask &bitmask) const {
		auto font = MakeFontVariable();
		auto r = MakeFontRenderer(&bitmask, 0, 0);
		const auto width = font.measure(GAME_OVER);
		const uint8_t height = 8;
		const uint8_t xx = (w - width) / 2;
		const uint8_t yy = (h - height) / 2;
		bitmask.fill_rect(xx - 4, yy - 2, width + 8, height + 4, BlendMode::OFF);
		r.move(xx, yy);
		r.print(font, GAME_OVER);
	}

public:
	void set_lives(uint8_t newLives) {
		lives = newLives;
	}

	bool over(void) const {
		return (state == State::GAMEOVER) && !delaying;
	}

	uint16_t score(void) const {
		return _score;
	}

	template <typename PosCtl, typename FireCtl>
	void step(PosCtl *posCtl, FireCtl *fireCtl) {
		switch(state) {
		case State::PLAYING:
			step_defender(posCtl, fireCtl);
			step_invaders();
			break;
		case State::SHIP_FIRE:
			step_fire();
			break;
		case State::RESPAWNING:
			step_respawn();
			posCtl->reset();
			break;
		case State::WINNING:
			step_defender(posCtl, fireCtl);
			step_win();
			break;
		case State::GAMEOVER:
			step_gameover();
			return;
		}
		step_explosions();
		step_missiles();
		check_missile_collisions();
	}

	template <typename Bitmask>
	void render(Bitmask &bitmask) const {
		bitmask.clear(false);

		render_defender_missiles(bitmask);
		render_invader_missiles(bitmask);
		render_ground(bitmask);
		render_lives(bitmask);
		render_score(bitmask);
		render_defender(bitmask);
		render_shields(bitmask);
		render_invaders(bitmask);
		render_explosions(bitmask);

		if(state == State::GAMEOVER) {
			render_gameover(bitmask);
		}
	}

	template <typename Bleeper>
	void play_sounds(Bleeper *bleeper, uint16_t endTm) {
		switch(state) {
		case State::PLAYING:
			// Sounds are not supposed to be in sync with movement!
			// Actually gets faster in steps as alien count decreases.

			if(soundDelay) {
				-- soundDelay;
				break;
			}
			// tone library glitches sometimes unless we stop the old sound
			bleeper->stop();
			bleeper->play(MakeProgMem(stepFrequencies)[nextSound], 220);
			nextSound = (nextSound + 1) & 3;
			soundDelay = sound_delay();
			break;

		case State::SHIP_FIRE:
			bleeper->stop();
			bleeper->play(random(200) + 50, 30);
			while(uint16_t(endTm - millis() - whiteNoiseDelay) < 0x8000) {
				delay(whiteNoiseDelay);
				bleeper->stop();
				bleeper->play(random(200) + 50, 30);
			}
			break;

		default:
			break;
		}
		uint16_t d = uint16_t(endTm - millis());
		if(d < 0x8000) {
			delay(d);
		}
	}

	GameInvaders(void)
		: moveFrame(0)
		, animFrame(0)
		, shipX(0)
		, crudeShipX(w / 2)
		, lives(0)
		, _score(0)
		, explosionFrame(0)
		, soundDelay(0)
	{
		for(uint8_t i = 0; i < invadersY; ++ i) {
			invaderTypes[i] = (i * 3) / invadersY;
		}
		_reset();
	}
};

template <typename Display, typename PosCtl, typename FireCtl, typename Bleeper>
uint16_t playInvaders(
	Display &display,
	PosCtl *posCtl,
	FireCtl *fireCtl,
	Bleeper *bleeper
) {
	constexpr uint8_t w = display.width() - 16;
	constexpr uint8_t h = display.height();
	Bitmask18<w,h> bitmask;
	GameInvaders<w,h> game;

	game.set_lives(3);

	display.send(false);
	posCtl->reset();
	fireCtl->reset();

	const uint16_t frameDelay = 1000 / 50;

	uint16_t lastTm = millis();
	while(!game.over()) {
		game.step(posCtl, fireCtl);
		game.render(bitmask);

		display.send(
			bitmask,
			(display.width() - bitmask.width()) / 2,
			(display.height() - bitmask.height()) / 2
		);

		lastTm += frameDelay;
		game.play_sounds(bleeper, lastTm);
	}

	delay(3000);

	return game.score();
}

#endif
