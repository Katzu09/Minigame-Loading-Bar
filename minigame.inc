/*
    Minigame Loading Bar
    Author : Katzu
    Version : 1.1
    License : MIT
*/

#if defined _minigame_included
    #endinput
#endif
#define _minigame_included

#if !defined _samp_included
	#tryinclude <a_samp>
	#if !defined _samp_included
		#error failed to find file a_samp.inc. Please check the 'include' path on your server.
	#endif
#endif

#define PROGRESS_BAR_UPDATE_INTERVAL 50
#define KEY_PRESS_COOLDOWN 650
#define BAR_TOTAL_WIDTH 100
#define TARGET_WIDTH 7.0  
#define TARGET_TOLERANCE 4.0 

new PlayerText:g_mainProgres[MAX_PLAYERS];
new PlayerText:g_progresBar[MAX_PLAYERS];
new PlayerText:g_targetLine[MAX_PLAYERS];
new PlayerText:g_hitingText[MAX_PLAYERS];

enum E_PLAYER_MINIGAME {
    bool:isLoading,
    Float:loadingValue,
    loadingTimer,
    bool:loadingDirection,
    Float:targetPos,
    bool:hitTarget,
    hitCount,
    hitsNeeded,
    Float:barSpeed,
    lastKeyPress,
    successMessage[128],
    failMessage[128], 
    winMessage[128],
    callbackName[32],
    bool:hasSuccessMessage,
    bool:hasFailMessage,
    bool:hasWinMessage,
    callbackParams[128],
    callbackValues[10],
    callbackCount
}
new g_playerMinigame[MAX_PLAYERS][E_PLAYER_MINIGAME];

forward OnLoadingMnUpdate(playerid);
forward OnMinigameComplete(playerid);

stock StartPlayerMinigame(playerid, hittowin = 5, Float:speedbar = 0.3, const successmsg[] = "", const failmsg[] = "", const winmsg[] = "", const callbackname[] = "", const callbackparams[] = "", {Float,_}:...) {
    if(!IsPlayerConnected(playerid)) return 0;
    if(g_playerMinigame[playerid][isLoading]) return 0;
    
    g_playerMinigame[playerid][hitCount] = 0;
    g_playerMinigame[playerid][hitsNeeded] = hittowin;
    
    if(hittowin > 1) {
        new str[128];
        format(str, sizeof(str), "(Mini Game) {FFFFFF}You have to do it %d times to complete this", hittowin);
        SendClientMessage(playerid, 0x62da92FF, str);
    }
    
    g_playerMinigame[playerid][barSpeed] = speedbar;
    g_playerMinigame[playerid][loadingValue] = 0.0;
    g_playerMinigame[playerid][loadingDirection] = true;
    g_playerMinigame[playerid][lastKeyPress] = 0;
    g_playerMinigame[playerid][hitTarget] = false;
    g_playerMinigame[playerid][isLoading] = true;
    g_playerMinigame[playerid][targetPos] = float(random(floatround(BAR_TOTAL_WIDTH - TARGET_WIDTH)));
    
    g_playerMinigame[playerid][hasSuccessMessage] = (strlen(successmsg) > 0);
    if(g_playerMinigame[playerid][hasSuccessMessage]) {
        format(g_playerMinigame[playerid][successMessage], 128, successmsg);
    }
    
    g_playerMinigame[playerid][hasFailMessage] = (strlen(failmsg) > 0);
    if(g_playerMinigame[playerid][hasFailMessage]) {
        format(g_playerMinigame[playerid][failMessage], 128, failmsg);
    }
    
    g_playerMinigame[playerid][hasWinMessage] = (strlen(winmsg) > 0);
    if(g_playerMinigame[playerid][hasWinMessage]) {
        format(g_playerMinigame[playerid][winMessage], 128, winmsg);
    }
    
    if(strlen(callbackname) > 0) {
        format(g_playerMinigame[playerid][callbackName], 32, "%s", callbackname);
        if(strlen(callbackparams) > 0) {
            format(g_playerMinigame[playerid][callbackParams], 128, "%s", callbackparams);
            
            new n = numargs();
            g_playerMinigame[playerid][callbackCount] = n - 8; 
            
            for(new i = 0; i < g_playerMinigame[playerid][callbackCount] && i < 10; i++) {
                g_playerMinigame[playerid][callbackValues][i] = getarg(8 + i);
            }
        } else {
            g_playerMinigame[playerid][callbackParams][0] = 'i';
            g_playerMinigame[playerid][callbackParams][1] = EOS;
            g_playerMinigame[playerid][callbackCount] = 0;
        }
    } else {
        g_playerMinigame[playerid][callbackName][0] = EOS;
        g_playerMinigame[playerid][callbackParams][0] = EOS;
        g_playerMinigame[playerid][callbackCount] = 0;
    }
    
    if(g_playerMinigame[playerid][loadingTimer]) {
        KillTimer(g_playerMinigame[playerid][loadingTimer]);
    }
    g_playerMinigame[playerid][loadingTimer] = SetTimerEx("OnLoadingMnUpdate", PROGRESS_BAR_UPDATE_INTERVAL, true, "i", playerid);
    
    g_mainProgres[playerid] = CreatePlayerTextDraw(playerid, 260.000, 340.000, "LD_BUM:blkdot");
    PlayerTextDrawTextSize(playerid, g_mainProgres[playerid], 100.000, 20.000);
    PlayerTextDrawAlignment(playerid, g_mainProgres[playerid], 1);
    PlayerTextDrawColor(playerid, g_mainProgres[playerid], 16843153);
    PlayerTextDrawSetShadow(playerid, g_mainProgres[playerid], 0);
    PlayerTextDrawSetOutline(playerid, g_mainProgres[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, g_mainProgres[playerid], 255);
    PlayerTextDrawFont(playerid, g_mainProgres[playerid], 4);
    PlayerTextDrawSetProportional(playerid, g_mainProgres[playerid], 1);
    
    g_progresBar[playerid] = CreatePlayerTextDraw(playerid, 260.000, 340.000, "LD_BUM:blkdot");
    PlayerTextDrawTextSize(playerid, g_progresBar[playerid], 0.000, 20.000); 
    PlayerTextDrawAlignment(playerid, g_progresBar[playerid], 1);
    PlayerTextDrawColor(playerid, g_progresBar[playerid], 16423679);
    PlayerTextDrawSetShadow(playerid, g_progresBar[playerid], 0);
    PlayerTextDrawSetOutline(playerid, g_progresBar[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, g_progresBar[playerid], 255);
    PlayerTextDrawFont(playerid, g_progresBar[playerid], 4);
    PlayerTextDrawSetProportional(playerid, g_progresBar[playerid], 1);
    
    new Float:targetX = 260.0 + g_playerMinigame[playerid][targetPos];
    g_targetLine[playerid] = CreatePlayerTextDraw(playerid, targetX, 340.000, "LD_BUM:blkdot");
    PlayerTextDrawTextSize(playerid, g_targetLine[playerid], 7.000, 20.000);
    PlayerTextDrawAlignment(playerid, g_targetLine[playerid], 1);
    PlayerTextDrawColor(playerid, g_targetLine[playerid], 5320);
    PlayerTextDrawSetShadow(playerid, g_targetLine[playerid], 0);
    PlayerTextDrawSetOutline(playerid, g_targetLine[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, g_targetLine[playerid], 255);
    PlayerTextDrawFont(playerid, g_targetLine[playerid], 4);
    PlayerTextDrawSetProportional(playerid, g_targetLine[playerid], 1);

    g_hitingText[playerid] = CreatePlayerTextDraw(playerid, 308.000, 343.000, "H - to hit taget");
    PlayerTextDrawLetterSize(playerid, g_hitingText[playerid], 0.159, 1.299);
    PlayerTextDrawAlignment(playerid, g_hitingText[playerid], 2);
    PlayerTextDrawColor(playerid, g_hitingText[playerid], -1);
    PlayerTextDrawSetShadow(playerid, g_hitingText[playerid], 0);
    PlayerTextDrawSetOutline(playerid, g_hitingText[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, g_hitingText[playerid], 150);
    PlayerTextDrawFont(playerid, g_hitingText[playerid], 1);
    PlayerTextDrawSetProportional(playerid, g_hitingText[playerid], 1);
    
    PlayerTextDrawShow(playerid, g_mainProgres[playerid]);
    PlayerTextDrawShow(playerid, g_progresBar[playerid]);
    PlayerTextDrawShow(playerid, g_targetLine[playerid]);
    PlayerTextDrawShow(playerid, g_hitingText[playerid]);
    
    return 1;
}

stock StopPlayerMinigame(playerid) {
    if(!IsPlayerConnected(playerid)) return 0;
    
    if(g_mainProgres[playerid] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_mainProgres[playerid]);
    }
    if(g_progresBar[playerid] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_progresBar[playerid]);
    }
    if(g_targetLine[playerid] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_targetLine[playerid]);
    }
    
    if(g_hitingText[playerid] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_hitingText[playerid]);
    }
    
    if(g_playerMinigame[playerid][loadingTimer]) {
        KillTimer(g_playerMinigame[playerid][loadingTimer]);
        g_playerMinigame[playerid][loadingTimer] = 0;
    }
    
    g_playerMinigame[playerid][isLoading] = false;
    g_playerMinigame[playerid][hitTarget] = false;
    g_playerMinigame[playerid][hitCount] = 0;
    g_playerMinigame[playerid][loadingValue] = 0.0;
    
    return 1;
}

stock StopPlayerLMiniGame(playerid) {
    if(!IsPlayerConnected(playerid)) return 0;
    if(!IsValidMiniGame(playerid)) return 0;
    
    StopPlayerMinigame(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    StopPlayerMinigame(playerid);

    #if defined kmngames_OnPlayerDisconnect
        return kmngames_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect kmngames_OnPlayerDisconnect
#if defined kmngames_OnPlayerDisconnect
    forward kmngames_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(g_playerMinigame[playerid][isLoading] && (newkeys & KEY_CTRL_BACK)) {

        new tick = GetTickCount();
        if(tick - g_playerMinigame[playerid][lastKeyPress] < KEY_PRESS_COOLDOWN) {
            if(g_playerMinigame[playerid][hasFailMessage]) {
                //SendClientMessage(playerid, 0xFF0000FF, "Please don't spam!");
            }
            return 1;
        }
        g_playerMinigame[playerid][lastKeyPress] = tick;
        
        new Float:currentPos = g_playerMinigame[playerid][loadingValue];
        new Float:targetCenter = g_playerMinigame[playerid][targetPos] + (TARGET_WIDTH / 2);
        new Float:barEnd = currentPos;
        new Float:distance = floatabs(barEnd - targetCenter);
        
        g_playerMinigame[playerid][hitTarget] = (distance <= TARGET_TOLERANCE);
        
        if(g_playerMinigame[playerid][hitTarget]) {
            g_playerMinigame[playerid][hitCount]++;
            if(g_playerMinigame[playerid][hasSuccessMessage]) {
                SendClientMessage(playerid, 0x00FF00FF, g_playerMinigame[playerid][successMessage]);
            }
            
            if(g_playerMinigame[playerid][hitCount] >= g_playerMinigame[playerid][hitsNeeded]) {
                if(g_playerMinigame[playerid][hasWinMessage]) {
                    SendClientMessage(playerid, 0x00FF00FF, g_playerMinigame[playerid][winMessage]);
                }
                
                new func[32];
                format(func, sizeof(func), "%s", g_playerMinigame[playerid][callbackName]);
                
                if(strlen(func) > 0) {
                    switch(g_playerMinigame[playerid][callbackCount]) {
                        case 0: {
                            CallLocalFunction(func, "i", playerid);
                        }
                        case 1: {
                            CallLocalFunction(func, "ii", playerid, 
                                g_playerMinigame[playerid][callbackValues][0]);
                        }
                        case 2: {
                            CallLocalFunction(func, "iii", playerid, 
                                g_playerMinigame[playerid][callbackValues][0],
                                g_playerMinigame[playerid][callbackValues][1]);
                        }
                        case 3: {
                            CallLocalFunction(func, "iiii", playerid, 
                                g_playerMinigame[playerid][callbackValues][0],
                                g_playerMinigame[playerid][callbackValues][1],
                                g_playerMinigame[playerid][callbackValues][2]);
                        }
                    }
                }
                
                StopPlayerMinigame(playerid);
            }
            else {
                g_playerMinigame[playerid][targetPos] = float(random(floatround(BAR_TOTAL_WIDTH - TARGET_WIDTH)));
                
                PlayerTextDrawDestroy(playerid, g_targetLine[playerid]);
                g_targetLine[playerid] = CreatePlayerTextDraw(playerid, 260.0 + g_playerMinigame[playerid][targetPos], 340.000, "LD_BUM:blkdot");
                
                PlayerTextDrawFont(playerid, g_targetLine[playerid], 4);
                PlayerTextDrawLetterSize(playerid, g_targetLine[playerid], 0.600000, 2.000000);
                PlayerTextDrawTextSize(playerid, g_targetLine[playerid], 7.000000, 20.000000);
                PlayerTextDrawSetOutline(playerid, g_targetLine[playerid], 1);
                PlayerTextDrawSetShadow(playerid, g_targetLine[playerid], 0);
                PlayerTextDrawAlignment(playerid, g_targetLine[playerid], 1);
                PlayerTextDrawColor(playerid, g_targetLine[playerid], 5320);
                PlayerTextDrawBackgroundColor(playerid, g_targetLine[playerid], 255);
                PlayerTextDrawBoxColor(playerid, g_targetLine[playerid], 0x00000032);
                PlayerTextDrawUseBox(playerid, g_targetLine[playerid], 1);
                PlayerTextDrawSetProportional(playerid, g_targetLine[playerid], 1);
                PlayerTextDrawSetSelectable(playerid, g_targetLine[playerid], 0);
                PlayerTextDrawShow(playerid, g_targetLine[playerid]);
            }
        }
        else {
            g_playerMinigame[playerid][hitCount] = 0;
            if(g_playerMinigame[playerid][hasFailMessage]) {
                SendClientMessage(playerid, 0xFF0000FF, g_playerMinigame[playerid][failMessage]);
            }
            g_playerMinigame[playerid][targetPos] = float(random(floatround(BAR_TOTAL_WIDTH - TARGET_WIDTH)));
            
            PlayerTextDrawDestroy(playerid, g_targetLine[playerid]);
            
            g_targetLine[playerid] = CreatePlayerTextDraw(playerid, 260.0 + g_playerMinigame[playerid][targetPos], 340.000, "LD_BUM:blkdot");
            PlayerTextDrawFont(playerid, g_targetLine[playerid], 4);
            PlayerTextDrawLetterSize(playerid, g_targetLine[playerid], 0.600000, 2.000000);
            PlayerTextDrawTextSize(playerid, g_targetLine[playerid], 7.000000, 20.000000);
            PlayerTextDrawSetOutline(playerid, g_targetLine[playerid], 1);
            PlayerTextDrawSetShadow(playerid, g_targetLine[playerid], 0);
            PlayerTextDrawAlignment(playerid, g_targetLine[playerid], 1);
            PlayerTextDrawColor(playerid, g_targetLine[playerid], 5320);
            PlayerTextDrawBackgroundColor(playerid, g_targetLine[playerid], 255);
            PlayerTextDrawBoxColor(playerid, g_targetLine[playerid], 0x00000032);
            PlayerTextDrawUseBox(playerid, g_targetLine[playerid], 1);
            PlayerTextDrawSetProportional(playerid, g_targetLine[playerid], 1);
            PlayerTextDrawSetSelectable(playerid, g_targetLine[playerid], 0);
            PlayerTextDrawShow(playerid, g_targetLine[playerid]);
        }
    }
    #if defined kmngames_OnPlayerKeyStateChange
        return kmngames_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange kmngames_OnPlayerKeyStateChange
#if defined kmngames_OnPlayerKeyStateChange
    forward kmngames_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif

public OnLoadingMnUpdate(playerid) {
    if(!g_playerMinigame[playerid][isLoading]) return 0;
    
    if(g_playerMinigame[playerid][loadingDirection]) {
        g_playerMinigame[playerid][loadingValue] += g_playerMinigame[playerid][barSpeed];
        if(g_playerMinigame[playerid][loadingValue] >= BAR_TOTAL_WIDTH) {
            g_playerMinigame[playerid][loadingValue] = BAR_TOTAL_WIDTH;
            g_playerMinigame[playerid][loadingDirection] = false;
        }
    }
    else {
        g_playerMinigame[playerid][loadingValue] -= g_playerMinigame[playerid][barSpeed];
        if(g_playerMinigame[playerid][loadingValue] <= 0.0) {
            g_playerMinigame[playerid][loadingValue] = 0.0;
            g_playerMinigame[playerid][loadingDirection] = true;
        }
    }
    
    PlayerTextDrawTextSize(playerid, g_progresBar[playerid], g_playerMinigame[playerid][loadingValue], 20.0);
    PlayerTextDrawShow(playerid, g_progresBar[playerid]);
    
    return 1;
}

public OnMinigameComplete(playerid) {
    if(strlen(g_playerMinigame[playerid][callbackName]) > 0) {
        CallLocalFunction(g_playerMinigame[playerid][callbackName], "i", playerid);
    }
    return 1;
}

stock IsValidMiniGame(playerid) {
    if(!IsPlayerConnected(playerid)) return false;
    return g_playerMinigame[playerid][isLoading];
}
