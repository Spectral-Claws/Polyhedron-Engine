// LICENSE HERE.

//
// weapons.h
//
//
// Contains basic weapons functionalities.
//
#ifndef __SVGAME_PLAYER_WEAPONS_H__
#define __SVGAME_PLAYER_WEAPONS_H__

// These are extern, used in the weapons/* code files.
extern qboolean is_quad;
extern byte     is_silenced;

class ServerGameEntity;
class PlayerClient;

// Player project source.
vec3_t SVG_PlayerProjectSource(ServerClient* client, const vec3_t &point, const vec3_t& distance, const vec3_t& forward, const vec3_t& right);

void SVG_PlayerNoise(ServerGameEntity* who, vec3_t where, int type);

qboolean    Pickup_Weapon(ServerGameEntity* ent, PlayerClient* other);
void        SVG_ChangeWeapon(PlayerClient* ent);
void        NoAmmoWeaponChange(PlayerClient* ent);
void        SVG_ThinkWeapon(PlayerClient* ent);
void        Use_Weapon(PlayerClient *ent, GameItem* item);
void        Drop_Weapon(PlayerClient *ent, GameItem* item);
void        Weapon_Generic(PlayerClient *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST, int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int* pause_frames, int* fire_frames, void (*fire)(PlayerClient* ent));

#endif // __SVGAME_PLAYER_WEAPONS_H__