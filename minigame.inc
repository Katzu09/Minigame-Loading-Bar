/*
    Minigame Loading Bar
    Author : Katzu
    Version : 1.0
    License : 
*/

#include <YSI_Hooks/y_hooks>

#if defined _minigame_included
    #endinput
#endif
#define _minigame_included

// Konstanta
#define KEY_PRESS_COOLDOWN 650 // Kurangi cooldown untuk respon lebih cepat
#define TARGET_TOLERANCE 3.0 // Kurangi toleransi untuk akurasi lebih tinggi

// Variabel per-player
enum E_PLAYER_MINIGAME {
    PlayerText:mainProgres,
    PlayerText:progresBar, 
    PlayerText:targetLine,
    bool:isLoading,
    Float:loadingValue,
    loadingTimer,  // Ubah ke timer standard SA-MP
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
    callbackName[32]
}
new g_playerMinigame[MAX_PLAYERS][E_PLAYER_MINIGAME];

// Forward declarations
forward OnLoadingUpdate(playerid);
forward OnMinigameComplete(playerid);

// API Functions
stock StartPlayerMinigame(playerid, hittowin = 5, Float:speedbar = 0.3, const successmsg[] = "Berhasil!", const failmsg[] = "Gagal! Coba lagi!", const winmsg[] = "Selamat! Kamu telah menang!", const callbackname[] = "") {
    // Validasi player
    if(!IsPlayerConnected(playerid)) return 0;
    if(g_playerMinigame[playerid][isLoading]) return 0;
    
    // Reset semua state minigame
    g_playerMinigame[playerid][hitCount] = 0;
    g_playerMinigame[playerid][hitsNeeded] = hittowin;
    g_playerMinigame[playerid][barSpeed] = speedbar;
    g_playerMinigame[playerid][loadingValue] = 0.0;
    g_playerMinigame[playerid][loadingDirection] = true;
    g_playerMinigame[playerid][lastKeyPress] = 0;
    g_playerMinigame[playerid][hitTarget] = false;
    g_playerMinigame[playerid][isLoading] = true;

    // Destroy TextDraw yang mungkin masih ada
    if(g_playerMinigame[playerid][mainProgres] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][mainProgres]);
    }
    if(g_playerMinigame[playerid][progresBar] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][progresBar]); 
    }
    if(g_playerMinigame[playerid][targetLine] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][targetLine]);
    }

    // Set random target dengan range yang lebih presisi
    g_playerMinigame[playerid][targetPos] = float(random(100)) + (float(random(100)) / 100.0); // Menghasilkan posisi dengan desimal
    
    // Simpan pesan-pesan
    format(g_playerMinigame[playerid][successMessage], 128, successmsg);
    format(g_playerMinigame[playerid][failMessage], 128, failmsg);
    format(g_playerMinigame[playerid][winMessage], 128, winmsg);
    
    // Copy nama callback jika ada
    if(strlen(callbackname) > 0) {
        format(g_playerMinigame[playerid][callbackName], 32, callbackname);
    } else {
        g_playerMinigame[playerid][callbackName][0] = EOS; // Set string kosong
    }
    
    // Validasi timer
    if(g_playerMinigame[playerid][loadingTimer]) {
        KillTimer(g_playerMinigame[playerid][loadingTimer]);
    }
    g_playerMinigame[playerid][loadingTimer] = SetTimerEx("OnLoadingUpdate", 50, true, "i", playerid);
    
    // Setup main background
    g_playerMinigame[playerid][mainProgres] = CreatePlayerTextDraw(playerid, 270.000000, 385.000000, "ld_bum:blkdot");
    PlayerTextDrawFont(playerid, g_playerMinigame[playerid][mainProgres], 4);
    PlayerTextDrawLetterSize(playerid, g_playerMinigame[playerid][mainProgres], 0.600000, 2.000000);
    PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][mainProgres], 115.000000, 20.000000);
    PlayerTextDrawSetOutline(playerid, g_playerMinigame[playerid][mainProgres], 1);
    PlayerTextDrawSetShadow(playerid, g_playerMinigame[playerid][mainProgres], 0);
    PlayerTextDrawAlignment(playerid, g_playerMinigame[playerid][mainProgres], 1);
    PlayerTextDrawColor(playerid, g_playerMinigame[playerid][mainProgres], 0xE2E2E2FF);
    PlayerTextDrawBackgroundColor(playerid, g_playerMinigame[playerid][mainProgres], 0x000000FF);
    PlayerTextDrawBoxColor(playerid, g_playerMinigame[playerid][mainProgres], 0x00000032);
    PlayerTextDrawUseBox(playerid, g_playerMinigame[playerid][mainProgres], 1);
    PlayerTextDrawSetProportional(playerid, g_playerMinigame[playerid][mainProgres], 1);
    PlayerTextDrawSetSelectable(playerid, g_playerMinigame[playerid][mainProgres], 0);
    
    // Progress bar
    g_playerMinigame[playerid][progresBar] = CreatePlayerTextDraw(playerid, 270.000000, 385.000000, "ld_bum:blkdot");
    PlayerTextDrawFont(playerid, g_playerMinigame[playerid][progresBar], 4);
    PlayerTextDrawLetterSize(playerid, g_playerMinigame[playerid][progresBar], 0.600000, 2.000000);
    PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][progresBar], 0.000000, 20.000000);
    PlayerTextDrawSetOutline(playerid, g_playerMinigame[playerid][progresBar], 1);
    PlayerTextDrawSetShadow(playerid, g_playerMinigame[playerid][progresBar], 0);
    PlayerTextDrawAlignment(playerid, g_playerMinigame[playerid][progresBar], 1);
    PlayerTextDrawColor(playerid, g_playerMinigame[playerid][progresBar], 0x32CD32FF);
    PlayerTextDrawBackgroundColor(playerid, g_playerMinigame[playerid][progresBar], 0x000000FF);
    PlayerTextDrawBoxColor(playerid, g_playerMinigame[playerid][progresBar], 0x00000032);
    PlayerTextDrawUseBox(playerid, g_playerMinigame[playerid][progresBar], 1);
    PlayerTextDrawSetProportional(playerid, g_playerMinigame[playerid][progresBar], 1);
    PlayerTextDrawSetSelectable(playerid, g_playerMinigame[playerid][progresBar], 0);
    
    // Target line dengan posisi random
    new Float:targetX = 270.0 + g_playerMinigame[playerid][targetPos];
    g_playerMinigame[playerid][targetLine] = CreatePlayerTextDraw(playerid, targetX, 385.000000, "ld_bum:blkdot");
    PlayerTextDrawFont(playerid, g_playerMinigame[playerid][targetLine], 4);
    PlayerTextDrawLetterSize(playerid, g_playerMinigame[playerid][targetLine], 0.600000, 2.000000);
    PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][targetLine], 8.000000, 20.000000);
    PlayerTextDrawSetOutline(playerid, g_playerMinigame[playerid][targetLine], 1);
    PlayerTextDrawSetShadow(playerid, g_playerMinigame[playerid][targetLine], 0);
    PlayerTextDrawAlignment(playerid, g_playerMinigame[playerid][targetLine], 1);
    PlayerTextDrawColor(playerid, g_playerMinigame[playerid][targetLine], 0x0000FFFF);
    PlayerTextDrawBackgroundColor(playerid, g_playerMinigame[playerid][targetLine], 0x000000FF);
    PlayerTextDrawBoxColor(playerid, g_playerMinigame[playerid][targetLine], 0x00000032);
    PlayerTextDrawUseBox(playerid, g_playerMinigame[playerid][targetLine], 1);
    PlayerTextDrawSetProportional(playerid, g_playerMinigame[playerid][targetLine], 1);
    PlayerTextDrawSetSelectable(playerid, g_playerMinigame[playerid][targetLine], 0);
    
    // Show textdraws
    PlayerTextDrawShow(playerid, g_playerMinigame[playerid][mainProgres]);
    PlayerTextDrawShow(playerid, g_playerMinigame[playerid][progresBar]);
    PlayerTextDrawShow(playerid, g_playerMinigame[playerid][targetLine]);
    
    return 1;
}

stock StopPlayerMinigame(playerid) {
    if(!IsPlayerConnected(playerid)) return 0;
    
    // Destroy semua TextDraw
    if(g_playerMinigame[playerid][mainProgres] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][mainProgres]);
    }
    if(g_playerMinigame[playerid][progresBar] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][progresBar]);
    }
    if(g_playerMinigame[playerid][targetLine] != PlayerText:INVALID_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][targetLine]);
    }
    
    // Reset timer
    if(g_playerMinigame[playerid][loadingTimer]) {
        KillTimer(g_playerMinigame[playerid][loadingTimer]);
        g_playerMinigame[playerid][loadingTimer] = 0;
    }
    
    // Reset state
    g_playerMinigame[playerid][isLoading] = false;
    g_playerMinigame[playerid][hitTarget] = false;
    g_playerMinigame[playerid][hitCount] = 0;
    g_playerMinigame[playerid][loadingValue] = 0.0;
    
    return 1;
}

// Hooks
hook OnPlayerDisconnect(playerid, reason) {
    StopPlayerMinigame(playerid);
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(g_playerMinigame[playerid][isLoading] && (newkeys & KEY_WALK)) {
        // Cek cooldown
        new tick = GetTickCount();
        if(tick - g_playerMinigame[playerid][lastKeyPress] < KEY_PRESS_COOLDOWN) {
            SendClientMessage(playerid, 0xFF0000FF, "Please don't spam!"); // Tambah pesan warning
            return 1;
        }
        g_playerMinigame[playerid][lastKeyPress] = tick;
        
        // Cek hit target dengan variabel yang benar
        g_playerMinigame[playerid][hitTarget] = (g_playerMinigame[playerid][loadingValue] >= g_playerMinigame[playerid][targetPos] - TARGET_TOLERANCE && 
            g_playerMinigame[playerid][loadingValue] <= g_playerMinigame[playerid][targetPos] + TARGET_TOLERANCE);
        
        if(g_playerMinigame[playerid][hitTarget]) {
            g_playerMinigame[playerid][hitCount]++;
            SendClientMessage(playerid, 0x00FF00FF, g_playerMinigame[playerid][successMessage]);
            
            if(g_playerMinigame[playerid][hitCount] >= g_playerMinigame[playerid][hitsNeeded]) {
                SendClientMessage(playerid, 0x00FF00FF, g_playerMinigame[playerid][winMessage]);
                
                if(strlen(g_playerMinigame[playerid][callbackName]) > 0) {
                    CallLocalFunction(g_playerMinigame[playerid][callbackName], "i", playerid);
                }
                
                StopPlayerMinigame(playerid);
            }
            else {
                // Spawn target baru
                g_playerMinigame[playerid][targetPos] = float(random(115));
                PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][targetLine]);
                
                g_playerMinigame[playerid][targetLine] = CreatePlayerTextDraw(playerid, 270.0 + g_playerMinigame[playerid][targetPos], 385.000000, "ld_bum:blkdot");
                // Setup target line baru
                PlayerTextDrawFont(playerid, g_playerMinigame[playerid][targetLine], 4);
                PlayerTextDrawLetterSize(playerid, g_playerMinigame[playerid][targetLine], 0.600000, 2.000000);
                PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][targetLine], 8.000000, 20.000000);
                PlayerTextDrawSetOutline(playerid, g_playerMinigame[playerid][targetLine], 1);
                PlayerTextDrawSetShadow(playerid, g_playerMinigame[playerid][targetLine], 0);
                PlayerTextDrawAlignment(playerid, g_playerMinigame[playerid][targetLine], 1);
                PlayerTextDrawColor(playerid, g_playerMinigame[playerid][targetLine], 0x0000FFFF);
                PlayerTextDrawBackgroundColor(playerid, g_playerMinigame[playerid][targetLine], 0x000000FF);
                PlayerTextDrawBoxColor(playerid, g_playerMinigame[playerid][targetLine], 0x00000032);
                PlayerTextDrawUseBox(playerid, g_playerMinigame[playerid][targetLine], 1);
                PlayerTextDrawSetProportional(playerid, g_playerMinigame[playerid][targetLine], 1);
                PlayerTextDrawSetSelectable(playerid, g_playerMinigame[playerid][targetLine], 0);
                PlayerTextDrawShow(playerid, g_playerMinigame[playerid][targetLine]);
            }
        }
        else {
            SendClientMessage(playerid, 0xFF0000FF, g_playerMinigame[playerid][failMessage]);
            // Spawn target baru saat gagal
            g_playerMinigame[playerid][targetPos] = float(random(115));
            PlayerTextDrawDestroy(playerid, g_playerMinigame[playerid][targetLine]);
            
            g_playerMinigame[playerid][targetLine] = CreatePlayerTextDraw(playerid, 270.0 + g_playerMinigame[playerid][targetPos], 385.000000, "ld_bum:blkdot");
            PlayerTextDrawFont(playerid, g_playerMinigame[playerid][targetLine], 4);
            PlayerTextDrawLetterSize(playerid, g_playerMinigame[playerid][targetLine], 0.600000, 2.000000);
            PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][targetLine], 8.000000, 20.000000);
            PlayerTextDrawSetOutline(playerid, g_playerMinigame[playerid][targetLine], 1);
            PlayerTextDrawSetShadow(playerid, g_playerMinigame[playerid][targetLine], 0);
            PlayerTextDrawAlignment(playerid, g_playerMinigame[playerid][targetLine], 1);
            PlayerTextDrawColor(playerid, g_playerMinigame[playerid][targetLine], 0x0000FFFF);
            PlayerTextDrawBackgroundColor(playerid, g_playerMinigame[playerid][targetLine], 0x000000FF);
            PlayerTextDrawBoxColor(playerid, g_playerMinigame[playerid][targetLine], 0x00000032);
            PlayerTextDrawUseBox(playerid, g_playerMinigame[playerid][targetLine], 1);
            PlayerTextDrawSetProportional(playerid, g_playerMinigame[playerid][targetLine], 1);
            PlayerTextDrawSetSelectable(playerid, g_playerMinigame[playerid][targetLine], 0);
            PlayerTextDrawShow(playerid, g_playerMinigame[playerid][targetLine]);
        }
    }
    return 1;
}

// Hooks
hook OnLoadingUpdate(playerid) {
    if(!g_playerMinigame[playerid][isLoading]) return 0;
    
    // Update loading bar dengan validasi
    if(g_playerMinigame[playerid][loadingDirection]) {
        g_playerMinigame[playerid][loadingValue] += g_playerMinigame[playerid][barSpeed];
        if(g_playerMinigame[playerid][loadingValue] >= 115.0) {
            g_playerMinigame[playerid][loadingValue] = 115.0;
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
    
    // Update TextDraw dengan validasi
    PlayerTextDrawTextSize(playerid, g_playerMinigame[playerid][progresBar], g_playerMinigame[playerid][loadingValue], 20.0);
    PlayerTextDrawShow(playerid, g_playerMinigame[playerid][progresBar]);
    
    return 1;
}

public OnMinigameComplete(playerid) {
    // Cek apakah ada callback yang perlu dipanggil
    if(strlen(g_playerMinigame[playerid][callbackName]) > 0) {
        CallLocalFunction(g_playerMinigame[playerid][callbackName], "i", playerid);
    }
    return 1;
} 