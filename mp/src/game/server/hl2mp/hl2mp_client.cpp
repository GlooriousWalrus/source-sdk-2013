//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== tf_client.cpp ========================================================

  HL2 client/server game specific stuff

*/

#include "cbase.h"
#include "hl2mp_player.h"
#include "hl2mp_gamerules.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"
#include "team.h"
#include "viewport_panel_names.h"

#include "tier0/vprof.h"

#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void Host_Say( edict_t *pEdict, bool teamonly );

ConVar sv_motd_unload_on_dismissal( "sv_motd_unload_on_dismissal", "0", 0, "If enabled, the MOTD contents will be unloaded when the player closes the MOTD." );

extern CBaseEntity*	FindPickerEntityClass( CBasePlayer *pPlayer, char *classname );
extern bool			g_fGameOver;

void FinishClientPutInServer( CHL2MP_Player *pPlayer )
{
	pPlayer->InitialSpawn();
	pPlayer->Spawn();


	char sName[128];
	Q_strncpy( sName, pPlayer->GetPlayerName(), sizeof( sName ) );

	// First parse the name and remove any %'s
	for ( char *pApersand = sName; pApersand != NULL && *pApersand != 0; pApersand++ )
	{
		// Replace it with a space
		if ( *pApersand == '%' )
				*pApersand = ' ';
	}

	// notify other clients of player joining the game
	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Game_connected", sName[0] != 0 ? sName : "<unconnected>" );

	if ( HL2MPRules()->IsTeamplay() == true )
	{
		ClientPrint( pPlayer, HUD_PRINTTALK, "You are on team %s1\n", pPlayer->GetTeam()->GetName() );
	}

// 	const ConVar *hostname = cvar->FindVar( "hostname" );
// 	const char *title = (hostname) ? hostname->GetString() : "MESSAGE OF THE DAY";
//
// 	KeyValues *data = new KeyValues("data");
// 	data->SetString( "title", title );		// info panel title
// 	data->SetString( "type", "1" );			// show userdata from stringtable entry
// 	data->SetString( "msg",	"motd" );		// use this stringtable entry
// 	data->SetBool( "unload", sv_motd_unload_on_dismissal.GetBool() );
//
// 	pPlayer->ShowViewPortPanel( PANEL_INFO, true, data );
//
// 	data->deleteThis();


//if (Transitioned)
//{
  //SecobMod__ChangeME!
  //SecobMod__FixMe For whatever reason the new secobmod won't find the cfg file after a map change unless we hard code the file path. Maybe someone with better filesystem knowledge can fix this back
  // to how it used to be (just cfg/transition.cfg). Probably to do with mounting other content in the gameinfo.txt search paths (well that's my guess at least).
  KeyValues *pkvTransitionRestoreFile = new KeyValues( "cfg/transition.cfg" );
   if ( pkvTransitionRestoreFile->LoadFromFile( filesystem, "cfg/transition.cfg" ) )
   {
      while ( pkvTransitionRestoreFile )
      {
        const char *pszSteamID = pkvTransitionRestoreFile->GetName(); //Gets our header, which we use the players SteamID for.
		const char *PlayerSteamID = engine->GetPlayerNetworkIDString(pPlayer->edict()); //Finds the current players Steam ID.

		Msg ("In-File SteamID is %s.\n",pszSteamID);
		Msg ("In-Game SteamID is %s.\n",PlayerSteamID);

		 if ( Q_strcmp( PlayerSteamID, pszSteamID ) != 0)
		 {
		 		if (pkvTransitionRestoreFile == NULL)
				{
				break;
				}
			//SecobMod__Information  No SteamID found for this person, maybe they're new to the game or have "STEAM_ID_PENDING". Show them the class menu and break the loop.
			pPlayer->ShowViewPortPanel( PANEL_CLASS, true, NULL );
			break;
		}

		  Msg ("SteamID Match Found!");

		 //Health
		 KeyValues *pkvHealth = pkvTransitionRestoreFile->FindKey( "Health" );

		 //Armour
		 KeyValues *pkvArmour = pkvTransitionRestoreFile->FindKey( "Armour" );

		 //CurrentHeldWeapon
		 KeyValues *pkvActiveWep = pkvTransitionRestoreFile->FindKey( "ActiveWeapon" );

		 //Weapon_0.
		 KeyValues *pkvWeapon_0 = pkvTransitionRestoreFile->FindKey( "Weapon_0" );
		 KeyValues *pkvWeapon_0_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_0_PriClip" );
		 KeyValues *pkvWeapon_0_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_0_SecClip" );
		 KeyValues *pkvWeapon_0_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_0_PriClipAmmo" );
		 KeyValues *pkvWeapon_0_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_0_SecClipAmmo" );
		 //Weapon_1.
		 KeyValues *pkvWeapon_1 = pkvTransitionRestoreFile->FindKey( "Weapon_1" );
		 KeyValues *pkvWeapon_1_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_1_PriClip" );
		 KeyValues *pkvWeapon_1_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_1_SecClip" );
		 KeyValues *pkvWeapon_1_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_1_PriClipAmmo" );
		 KeyValues *pkvWeapon_1_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_1_SecClipAmmo" );
		 //Weapon_2.
		 KeyValues *pkvWeapon_2 = pkvTransitionRestoreFile->FindKey( "Weapon_2" );
		 KeyValues *pkvWeapon_2_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_2_PriClip" );
		 KeyValues *pkvWeapon_2_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_2_SecClip" );
		 KeyValues *pkvWeapon_2_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_2_PriClipAmmo" );
		 KeyValues *pkvWeapon_2_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_2_SecClipAmmo" );
		 //Weapon_3.
		 KeyValues *pkvWeapon_3 = pkvTransitionRestoreFile->FindKey( "Weapon_3" );
		 KeyValues *pkvWeapon_3_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_3_PriClip" );
		 KeyValues *pkvWeapon_3_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_3_SecClip" );
		 KeyValues *pkvWeapon_3_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_3_PriClipAmmo" );
		 KeyValues *pkvWeapon_3_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_3_SecClipAmmo" );
		 //Weapon_4.
		 KeyValues *pkvWeapon_4 = pkvTransitionRestoreFile->FindKey( "Weapon_4" );
		 KeyValues *pkvWeapon_4_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_4_PriClip" );
		 KeyValues *pkvWeapon_4_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_4_SecClip" );
		 KeyValues *pkvWeapon_4_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_4_PriClipAmmo" );
		 KeyValues *pkvWeapon_4_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_4_SecClipAmmo" );
		 //Weapon_5.
		 KeyValues *pkvWeapon_5 = pkvTransitionRestoreFile->FindKey( "Weapon_5" );
		 KeyValues *pkvWeapon_5_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_5_PriClip" );
		 KeyValues *pkvWeapon_5_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_5_SecClip" );
		 KeyValues *pkvWeapon_5_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_5_PriClipAmmo" );
		 KeyValues *pkvWeapon_5_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_5_SecClipAmmo" );
		 //Weapon_6.
		 KeyValues *pkvWeapon_6 = pkvTransitionRestoreFile->FindKey( "Weapon_6" );
		 KeyValues *pkvWeapon_6_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_6_PriClip" );
		 KeyValues *pkvWeapon_6_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_6_SecClip" );
		 KeyValues *pkvWeapon_6_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_6_PriClipAmmo" );
		 KeyValues *pkvWeapon_6_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_6_SecClipAmmo" );
		 //Weapon_7.
		 KeyValues *pkvWeapon_7 = pkvTransitionRestoreFile->FindKey( "Weapon_7" );
		 KeyValues *pkvWeapon_7_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_7_PriClip" );
		 KeyValues *pkvWeapon_7_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_7_SecClip" );
		 KeyValues *pkvWeapon_7_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_7_PriClipAmmo" );
		 KeyValues *pkvWeapon_7_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_7_SecClipAmmo" );
		 //Weapon_8.
		 KeyValues *pkvWeapon_8 = pkvTransitionRestoreFile->FindKey( "Weapon_8" );
		 KeyValues *pkvWeapon_8_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_8_PriClip" );
		 KeyValues *pkvWeapon_8_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_8_SecClip" );
		 KeyValues *pkvWeapon_8_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_8_PriClipAmmo" );
		 KeyValues *pkvWeapon_8_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_8_SecClipAmmo" );
		 //Weapon_9.
		 KeyValues *pkvWeapon_9 = pkvTransitionRestoreFile->FindKey( "Weapon_9" );
		 KeyValues *pkvWeapon_9_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_9_PriClip" );
		 KeyValues *pkvWeapon_9_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_9_SecClip" );
		 KeyValues *pkvWeapon_9_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_9_PriClipAmmo" );
		 KeyValues *pkvWeapon_9_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_9_SecClipAmmo" );
		 //Weapon_10.
		 KeyValues *pkvWeapon_10 = pkvTransitionRestoreFile->FindKey( "Weapon_10" );
		 KeyValues *pkvWeapon_10_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_10_PriClip" );
		 KeyValues *pkvWeapon_10_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_10_SecClip" );
		 KeyValues *pkvWeapon_10_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_10_PriClipAmmo" );
		 KeyValues *pkvWeapon_10_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_10_SecClipAmmo" );
		 //Weapon_11.
		 KeyValues *pkvWeapon_11 = pkvTransitionRestoreFile->FindKey( "Weapon_11" );
		 KeyValues *pkvWeapon_11_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_11_PriClip" );
		 KeyValues *pkvWeapon_11_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_11_SecClip" );
		 KeyValues *pkvWeapon_11_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_11_PriClipAmmo" );
		 KeyValues *pkvWeapon_11_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_11_SecClipAmmo" );
		 //Weapon_12.
		 KeyValues *pkvWeapon_12 = pkvTransitionRestoreFile->FindKey( "Weapon_12" );
		 KeyValues *pkvWeapon_12_PriClip = pkvTransitionRestoreFile->FindKey( "Weapon_12_PriClip" );
		 KeyValues *pkvWeapon_12_SecClip = pkvTransitionRestoreFile->FindKey( "Weapon_12_SecClip" );
		 KeyValues *pkvWeapon_12_PriClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_12_PriClipAmmo" );
		 KeyValues *pkvWeapon_12_SecClipAmmo = pkvTransitionRestoreFile->FindKey( "Weapon_12_SecClipAmmo" );


//=====================================================================

		   if ( pszSteamID )
         {

			//Set ints for the class,health and armour.
			int PlayerHealthValue = pkvHealth->GetInt();

			//Armour.
			int PlayerArmourValue = pkvArmour->GetInt();

			//CurrentWeapon
			const char *pkvActiveWep_Value = pkvActiveWep->GetString();

			//Set String and Ints for Weapon_0
			const char *pkvWeapon_0_Value = pkvWeapon_0->GetString();
			int Weapon_0_PriClip_Value = pkvWeapon_0_PriClip->GetInt();
			const char *pkvWeapon_0_PriClipAmmo_Value = pkvWeapon_0_PriClipAmmo->GetString();
			int Weapon_0_SecClip_Value = pkvWeapon_0_SecClip->GetInt();
			const char *pkvWeapon_0_SecClipAmmo_Value = pkvWeapon_0_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_1
			const char *pkvWeapon_1_Value = pkvWeapon_1->GetString();
			int Weapon_1_PriClip_Value = pkvWeapon_1_PriClip->GetInt();
			const char *pkvWeapon_1_PriClipAmmo_Value = pkvWeapon_1_PriClipAmmo->GetString();
			int Weapon_1_SecClip_Value = pkvWeapon_1_SecClip->GetInt();
			const char *pkvWeapon_1_SecClipAmmo_Value = pkvWeapon_1_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_2
			const char *pkvWeapon_2_Value = pkvWeapon_2->GetString();
			int Weapon_2_PriClip_Value = pkvWeapon_2_PriClip->GetInt();
			const char *pkvWeapon_2_PriClipAmmo_Value = pkvWeapon_2_PriClipAmmo->GetString();
			int Weapon_2_SecClip_Value = pkvWeapon_2_SecClip->GetInt();
			const char *pkvWeapon_2_SecClipAmmo_Value = pkvWeapon_2_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_3
			const char *pkvWeapon_3_Value = pkvWeapon_3->GetString();
			int Weapon_3_PriClip_Value = pkvWeapon_3_PriClip->GetInt();
			const char *pkvWeapon_3_PriClipAmmo_Value = pkvWeapon_3_PriClipAmmo->GetString();
			int Weapon_3_SecClip_Value = pkvWeapon_3_SecClip->GetInt();
			const char *pkvWeapon_3_SecClipAmmo_Value = pkvWeapon_3_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_4
			const char *pkvWeapon_4_Value = pkvWeapon_4->GetString();
			int Weapon_4_PriClip_Value = pkvWeapon_4_PriClip->GetInt();
			const char *pkvWeapon_4_PriClipAmmo_Value = pkvWeapon_4_PriClipAmmo->GetString();
			int Weapon_4_SecClip_Value = pkvWeapon_4_SecClip->GetInt();
			const char *pkvWeapon_4_SecClipAmmo_Value = pkvWeapon_4_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_5
			const char *pkvWeapon_5_Value = pkvWeapon_5->GetString();
			int Weapon_5_PriClip_Value = pkvWeapon_5_PriClip->GetInt();
			const char *pkvWeapon_5_PriClipAmmo_Value = pkvWeapon_5_PriClipAmmo->GetString();
			int Weapon_5_SecClip_Value = pkvWeapon_5_SecClip->GetInt();
			const char *pkvWeapon_5_SecClipAmmo_Value = pkvWeapon_5_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_6
			const char *pkvWeapon_6_Value = pkvWeapon_6->GetString();
			int Weapon_6_PriClip_Value = pkvWeapon_6_PriClip->GetInt();
			const char *pkvWeapon_6_PriClipAmmo_Value = pkvWeapon_6_PriClipAmmo->GetString();
			int Weapon_6_SecClip_Value = pkvWeapon_6_SecClip->GetInt();
			const char *pkvWeapon_6_SecClipAmmo_Value = pkvWeapon_6_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_7
			const char *pkvWeapon_7_Value = pkvWeapon_7->GetString();
			int Weapon_7_PriClip_Value = pkvWeapon_7_PriClip->GetInt();
			const char *pkvWeapon_7_PriClipAmmo_Value = pkvWeapon_7_PriClipAmmo->GetString();
			int Weapon_7_SecClip_Value = pkvWeapon_7_SecClip->GetInt();
			const char *pkvWeapon_7_SecClipAmmo_Value = pkvWeapon_7_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_8
			const char *pkvWeapon_8_Value = pkvWeapon_8->GetString();
			int Weapon_8_PriClip_Value = pkvWeapon_8_PriClip->GetInt();
			const char *pkvWeapon_8_PriClipAmmo_Value = pkvWeapon_8_PriClipAmmo->GetString();
			int Weapon_8_SecClip_Value = pkvWeapon_8_SecClip->GetInt();
			const char *pkvWeapon_8_SecClipAmmo_Value = pkvWeapon_8_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_9
			const char *pkvWeapon_9_Value = pkvWeapon_9->GetString();
			int Weapon_9_PriClip_Value = pkvWeapon_9_PriClip->GetInt();
			const char *pkvWeapon_9_PriClipAmmo_Value = pkvWeapon_9_PriClipAmmo->GetString();
			int Weapon_9_SecClip_Value = pkvWeapon_9_SecClip->GetInt();
			const char *pkvWeapon_9_SecClipAmmo_Value = pkvWeapon_9_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_10
			const char *pkvWeapon_10_Value = pkvWeapon_10->GetString();
			int Weapon_10_PriClip_Value = pkvWeapon_10_PriClip->GetInt();
			const char *pkvWeapon_10_PriClipAmmo_Value = pkvWeapon_10_PriClipAmmo->GetString();
			int Weapon_10_SecClip_Value = pkvWeapon_10_SecClip->GetInt();
			const char *pkvWeapon_10_SecClipAmmo_Value = pkvWeapon_10_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_11
			const char *pkvWeapon_11_Value = pkvWeapon_11->GetString();
			int Weapon_11_PriClip_Value = pkvWeapon_11_PriClip->GetInt();
			const char *pkvWeapon_11_PriClipAmmo_Value = pkvWeapon_11_PriClipAmmo->GetString();
			int Weapon_11_SecClip_Value = pkvWeapon_11_SecClip->GetInt();
			const char *pkvWeapon_11_SecClipAmmo_Value = pkvWeapon_11_SecClipAmmo->GetString();

			//Set String and Ints for Weapon_12
			const char *pkvWeapon_12_Value = pkvWeapon_12->GetString();
			int Weapon_12_PriClip_Value = pkvWeapon_12_PriClip->GetInt();
			const char *pkvWeapon_12_PriClipAmmo_Value = pkvWeapon_12_PriClipAmmo->GetString();
			int Weapon_12_SecClip_Value = pkvWeapon_12_SecClip->GetInt();
			const char *pkvWeapon_12_SecClipAmmo_Value = pkvWeapon_12_SecClipAmmo->GetString();


//============================================================================================

				Msg("Respawning...\n");
				pPlayer->RemoveAllItems( true );

				//Give our player the Weapon_0 slots.
				Msg ("Weapon is %s\n",pkvWeapon_0_Value);
				Msg ("Weapon ammo count is %i\n",Weapon_0_PriClip_Value);
				Msg ("Weapon ammo type is %s\n",pkvWeapon_0_PriClipAmmo_Value);
				pPlayer->GiveNamedItem( (pkvWeapon_0_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_0_PriClip_Value,	pkvWeapon_0_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_0_SecClip_Value,	pkvWeapon_0_SecClipAmmo_Value );

				//Give our player the Weapon_1 slots.
				Msg ("Weapon is %s\n",pkvWeapon_1_Value);
				Msg ("Weapon ammo count is %i\n",Weapon_1_PriClip_Value);
				Msg ("Weapon ammo type is %s\n",pkvWeapon_1_PriClipAmmo_Value);
				pPlayer->GiveNamedItem( (pkvWeapon_1_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_1_PriClip_Value,	pkvWeapon_1_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_1_SecClip_Value,	pkvWeapon_1_SecClipAmmo_Value );

				//Give our player the Weapon_2 slots.
				Msg ("Weapon is %s\n",pkvWeapon_2_Value);
				Msg ("Weapon ammo count is %i\n",Weapon_2_PriClip_Value);
				Msg ("Weapon ammo type is %s\n",pkvWeapon_2_PriClipAmmo_Value);
				pPlayer->GiveNamedItem( (pkvWeapon_2_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_2_PriClip_Value,	pkvWeapon_2_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_2_SecClip_Value,	pkvWeapon_2_SecClipAmmo_Value );

				//Give our player the Weapon_3 slots.
				Msg ("Weapon is %s\n",pkvWeapon_3_Value);
				Msg ("Weapon ammo count is %i\n",Weapon_3_PriClip_Value);
				Msg ("Weapon ammo type is %s\n",pkvWeapon_3_PriClipAmmo_Value);
				pPlayer->GiveNamedItem( (pkvWeapon_3_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_3_PriClip_Value,	pkvWeapon_3_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_3_SecClip_Value,	pkvWeapon_3_SecClipAmmo_Value );

				//Give our player the Weapon_4 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_4_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_4_PriClip_Value,	pkvWeapon_4_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_4_SecClip_Value,	pkvWeapon_4_SecClipAmmo_Value );

				//Give our player the Weapon_5 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_5_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_5_PriClip_Value,	pkvWeapon_5_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_5_SecClip_Value,	pkvWeapon_5_SecClipAmmo_Value );

				//Give our player the Weapon_6 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_6_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_6_PriClip_Value,	pkvWeapon_6_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_6_SecClip_Value,	pkvWeapon_6_SecClipAmmo_Value );

				//Give our player the Weapon_7 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_7_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_7_PriClip_Value,	pkvWeapon_7_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_7_SecClip_Value,	pkvWeapon_7_SecClipAmmo_Value );

				//Give our player the Weapon_8 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_8_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_8_PriClip_Value,	pkvWeapon_8_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_8_SecClip_Value,	pkvWeapon_8_SecClipAmmo_Value );

				//Give our player the Weapon_9 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_9_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_9_PriClip_Value,	pkvWeapon_9_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_9_SecClip_Value,	pkvWeapon_9_SecClipAmmo_Value );

				//Give our player the Weapon_10 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_10_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_10_PriClip_Value, pkvWeapon_10_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_10_SecClip_Value, pkvWeapon_10_SecClipAmmo_Value );

				//Give our player the Weapon_11 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_11_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_11_PriClip_Value, pkvWeapon_11_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_11_SecClip_Value, pkvWeapon_11_SecClipAmmo_Value );

				//Give our player the Weapon_12 slots.
				pPlayer->GiveNamedItem( (pkvWeapon_12_Value) );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_12_PriClip_Value, pkvWeapon_12_PriClipAmmo_Value );
				pPlayer->CBasePlayer::GiveAmmo( Weapon_12_SecClip_Value, pkvWeapon_12_SecClipAmmo_Value );

				//Now that our player has weapons, switch them to their last held weapon
				pPlayer->Weapon_Switch( pPlayer->Weapon_OwnsThisType( pkvActiveWep_Value ) );

				pPlayer->m_iHealth = PlayerHealthValue;
				pPlayer->m_iMaxHealth = 125;

				pPlayer->SetArmorValue(PlayerArmourValue);

				pPlayer->SetModel( "models/group03/male_06_sdk.mdl" );

			//PlayerClass bug fix.
			//pPlayer->EquipSuit();
			//pPlayer->StartSprinting();
			//pPlayer->StopSprinting();
			//pPlayer->EquipSuit(false);
			}
			else
			{
			pPlayer->ShowViewPortPanel( PANEL_CLASS, true, NULL );
			}

		// Transitioned = false;
		 break;
        //pkvTransitionRestoreFile = pkvTransitionRestoreFile->GetNextKey();
    //  }
   }
}
else
{
	pPlayer->ShowViewPortPanel( PANEL_CLASS, true, NULL );
}

}

/*
===========
ClientPutInServer

called each time a player is spawned into the game
============
*/
void ClientPutInServer( edict_t *pEdict, const char *playername )
{
	// Allocate a CBaseTFPlayer for pev, and call spawn
	CHL2MP_Player *pPlayer = CHL2MP_Player::CreatePlayer( "player", pEdict );
	pPlayer->SetPlayerName( playername );
}


void ClientActive( edict_t *pEdict, bool bLoadGame )
{
	// Can't load games in CS!
	Assert( !bLoadGame );

	CHL2MP_Player *pPlayer = ToHL2MPPlayer( CBaseEntity::Instance( pEdict ) );
	FinishClientPutInServer( pPlayer );
}


/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if ( g_pGameRules ) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "Half-Life 2 Deathmatch";
}

//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that
//			classname that the player is nearest facing
//
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity* FindEntity( edict_t *pEdict, char *classname)
{
	// If no name was given set bits based on the picked
	if (FStrEq(classname,""))
	{
		return (FindPickerEntityClass( static_cast<CBasePlayer*>(GetContainingEntity(pEdict)), classname ));
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
void ClientGamePrecache( void )
{
	CBaseEntity::PrecacheModel("models/player.mdl");
	CBaseEntity::PrecacheModel("models/group03/male_06_sdk.mdl");
	CBaseEntity::PrecacheModel( "models/gibs/agibs.mdl" );
	CBaseEntity::PrecacheModel ("models/weapons/v_hands.mdl");

	CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowAmmo" );
	CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowHealth" );

	CBaseEntity::PrecacheScriptSound( "FX_AntlionImpact.ShellImpact" );
	CBaseEntity::PrecacheScriptSound( "Missile.ShotDown" );
	CBaseEntity::PrecacheScriptSound( "Bullets.DefaultNearmiss" );
	CBaseEntity::PrecacheScriptSound( "Bullets.GunshipNearmiss" );
	CBaseEntity::PrecacheScriptSound( "Bullets.StriderNearmiss" );

	CBaseEntity::PrecacheScriptSound( "Geiger.BeepHigh" );
	CBaseEntity::PrecacheScriptSound( "Geiger.BeepLow" );
}


// called by ClientKill and DeadThink
void respawn( CBaseEntity *pEdict, bool fCopyCorpse )
{
	CHL2MP_Player *pPlayer = ToHL2MPPlayer( pEdict );

	if ( pPlayer )
	{
		if ( gpGlobals->curtime > pPlayer->GetDeathTime() + DEATH_ANIMATION_TIME )
		{
			// respawn player
			pPlayer->Spawn();
		}
		else
		{
			pPlayer->SetNextThink( gpGlobals->curtime + 0.1f );
		}
	}
}

void GameStartFrame( void )
{
	VPROF("GameStartFrame()");
	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = (teamplay.GetInt() != 0);

#ifdef DEBUG
	extern void Bot_RunAll();
	Bot_RunAll();
#endif
}

//=========================================================
// instantiate the proper game rules object
//=========================================================
void InstallGameRules()
{
	// vanilla deathmatch
	CreateGameRulesObject( "CHL2MPRules" );
}
